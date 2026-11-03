/* execucao.c — PESSOA 2: criação e execução de processos */

#include <unistd.h>     /* fork, execvp, _exit, dup2, close */
#include <fcntl.h>      /* open, O_WRONLY, O_CREAT, O_TRUNC */
#include <sys/wait.h>   /* wait, WIFEXITED, WEXITSTATUS */
#include <errno.h>      /* errno, ENOENT, EACCES, EINTR */
#include <string.h>     /* strerror, strcmp */
#include <signal.h>     /* signal, SIGINT, SIGQUIT, SIG_DFL */
#include "shell.h"

/*
 * Redirecionamento de saída: "comando [args] > arquivo".
 *
 * Roda SÓ no processo filho, antes do execvp(). Procura o token ">" em
 * argv e, se achar:
 *   1. open() abre o arquivo para escrita: cria se não existir e apaga o
 *      conteúdo se existir (permissão 0666, filtrada pela umask);
 *   2. dup2() copia esse descritor para o fd 1 (STDOUT_FILENO): daí em
 *      diante, o que o comando escrever na saída padrão vai para o arquivo;
 *   3. close() fecha o descritor original, que não é mais necessário;
 *   4. argv é cortado no ">" (argv[i] = NULL), para o comando não receber
 *      ">" e o nome do arquivo como argumentos.
 *
 * Como só o filho é redirecionado, o prompt do shell continua indo para
 * o terminal. E como o execvp() mantém os descritores abertos, o programa
 * novo já começa com a saída padrão apontando para o arquivo.
 *
 * Só aceita a forma "comando > arquivo" no fim da linha, com o ">"
 * separado por espaços (o parse_comando separa os tokens por espaço).
 *
 * Retorna: 0 se deu certo ou se não há ">"; 2 em erro de sintaxe;
 * 1 se o arquivo não pôde ser aberto. O filho usa esse valor no _exit().
 */
static int redirecionar_saida(char **argv) {
    int i;
    int fd;
    int erro;

    for (i = 0; argv[i] != NULL; i++)
        if (strcmp(argv[i], ">") == 0)
            break;

    if (argv[i] == NULL)            /* não há ">": nada a fazer */
        return 0;

    if (i == 0) {
        escrever_erro("minishell: erro de sintaxe: falta o comando antes de '>'\n");
        return 2;
    }
    if (argv[i + 1] == NULL) {
        escrever_erro("minishell: erro de sintaxe: falta o arquivo depois de '>'\n");
        return 2;
    }
    if (argv[i + 2] != NULL) {      /* ex.: "ls > a b" ou "ls > a > b" */
        escrever_erro("minishell: erro de sintaxe: '>' e o arquivo devem ficar no fim do comando\n");
        return 2;
    }

    fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        erro = errno;               /* guarda antes: o write() pode alterar o errno */
        escrever_erro("minishell: ");
        escrever_erro(argv[i + 1]);
        escrever_erro(": ");
        escrever_erro(strerror(erro));
        escrever_erro("\n");
        return 1;
    }

    if (fd != STDOUT_FILENO) {      /* só é igual se o fd 1 estava fechado */
        if (dup2(fd, STDOUT_FILENO) == -1) {
            erro = errno;
            escrever_erro("minishell: dup2: ");
            escrever_erro(strerror(erro));
            escrever_erro("\n");
            close(fd);
            return 1;
        }
        close(fd);
    }

    argv[i] = NULL;                 /* o comando termina antes do ">" */
    return 0;
}

/*
 * Executa argv em um processo filho usando fork() + execvp() + wait().
 *
 * fork() duplica o processo atual: o filho recebe 0 e o pai recebe o PID
 * do filho. O filho chama execvp(), que SUBSTITUI o programa dele pelo
 * comando pedido (procurando no PATH). Se o execvp() der certo, ele não
 * retorna nunca; por isso o código logo depois dele só roda em caso de erro.
 *
 * No filho usamos _exit() e não exit(): exit() executaria as rotinas de
 * saída e esvaziaria buffers herdados do pai, que seriam repetidos.
 *
 * O pai espera o filho com wait(). Sem isso o filho terminado viraria
 * um processo zumbi, e o prompt apareceria antes da saída do comando.
 *
 * Retorna: código de saída do filho, ou -1 se o fork falhar.
 */
int executar_comando(char **argv) {
    pid_t pid;
    int   status;
    int   erro;

    if (argv == NULL || argv[0] == NULL)
        return -1;

    pid = fork();

    if (pid < 0) {                  /* erro: nenhum processo foi criado */
        erro = errno;               /* guarda antes: o write() pode alterar o errno */
        escrever_erro("minishell: erro ao criar processo: ");
        escrever_erro(strerror(erro));
        escrever_erro("\n");
        return -1;
    }

    if (pid == 0) {                 /* PROCESSO FILHO */
        /* O execvp() mantém sinais ignorados como ignorados: sem isto,
         * o comando também ignoraria Ctrl+C e Ctrl+\ e não daria para pará-lo. */
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

        /* "comando > arquivo": aponta a saída para o arquivo e corta o argv. */
        erro = redirecionar_saida(argv);
        if (erro != 0)
            _exit(erro);

        execvp(argv[0], argv);

        /* Só chega aqui se o execvp() falhou. */
        erro = errno;               /* guarda antes: o write() pode alterar o errno */
        escrever_erro("minishell: ");
        escrever_erro(argv[0]);
        if (erro == ENOENT) {
            escrever_erro(": comando não encontrado\n");
        } else {
            escrever_erro(": ");
            escrever_erro(strerror(erro));
            escrever_erro("\n");
        }

        /* Mesma convenção do bash: 126 = achou mas não pode executar
         * (ex.: sem permissão), 127 = comando não encontrado. */
        _exit(erro == EACCES ? 126 : 127);
    }

    /* PROCESSO PAI: espera o filho terminar. */
    while (1) {
        pid_t r = wait(&status);

        if (r == pid)
            break;
        if (r == -1 && errno != EINTR)  /* EINTR: interrompido por sinal, espera de novo */
            return -1;
    }

    if (WIFSIGNALED(status) &&
        (WTERMSIG(status) == SIGINT || WTERMSIG(status) == SIGQUIT))
        escrever("\n");            /* o terminal mostra ^C ou ^\ sem quebrar a linha */

    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    return -1;                      /* terminou por sinal (ex.: kill) */
}
