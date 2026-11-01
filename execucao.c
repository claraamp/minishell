/* execucao.c — PESSOA 2: criação e execução de processos */

#include <unistd.h>     /* fork, execvp, _exit */
#include <sys/wait.h>   /* wait, WIFEXITED, WEXITSTATUS */
#include <errno.h>      /* errno, ENOENT, EINTR */
#include <string.h>     /* strerror */
#include "shell.h"

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

    if (argv == NULL || argv[0] == NULL)
        return -1;

    pid = fork();

    if (pid < 0) {                  /* erro: nenhum processo foi criado */
        escrever_erro("minishell: erro ao criar processo: ");
        escrever_erro(strerror(errno));
        escrever_erro("\n");
        return -1;
    }

    if (pid == 0) {                 /* PROCESSO FILHO */
        execvp(argv[0], argv);

        /* Só chega aqui se o execvp() falhou. */
        escrever_erro("minishell: ");
        escrever_erro(argv[0]);
        if (errno == ENOENT) {
            escrever_erro(": comando não encontrado\n");
        } else {
            escrever_erro(": ");
            escrever_erro(strerror(errno));
            escrever_erro("\n");
        }
        _exit(127);                 /* 127: convenção de "comando não encontrado" */
    }

    /* PROCESSO PAI: espera o filho terminar. */
    while (1) {
        pid_t r = wait(&status);

        if (r == pid)
            break;
        if (r == -1 && errno != EINTR)  /* EINTR: interrompido por sinal, espera de novo */
            return -1;
    }

    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    return -1;                      /* terminou por sinal (ex.: kill) */
}
