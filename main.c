/* main.c — PESSOA 3: saída, loop principal e integração */

#include <unistd.h>   /* write, chdir, getcwd, STDOUT_FILENO, STDERR_FILENO */
#include <string.h>   /* strlen, strcmp, strerror */
#include <stdlib.h>   /* getenv, setenv */
#include <limits.h>   /* PATH_MAX */
#include <errno.h>    /* errno, EINTR */
#include "shell.h"

/*
 * escrever_tudo: escreve a string msg inteira no descritor fd.
 *
 * O write() pode escrever menos bytes do que o pedido (escrita parcial)
 * ou ser interrompido por um sinal antes de escrever (errno == EINTR).
 * Por isso o laço repete até não restar nada.
 *
 * Retorna: 0 se escreveu tudo, -1 em erro.
 */
static int escrever_tudo(int fd, const char *msg) {
    size_t restante = strlen(msg);

    while (restante > 0) {
        ssize_t r = write(fd, msg, restante);

        if (r <= 0) {
            if (r == -1 && errno == EINTR)
                continue;   /* interrompido por sinal: tenta de novo */
            return -1;      /* erro real, ou write() retornou 0 */
        }

        msg      += r;      /* avança até onde já foi escrito */
        restante -= (size_t)r;
    }
    return 0;
}

void escrever(const char *msg) {
    /* Se a saída falhar, não há onde avisar: ignoramos de forma explícita. */
    (void)escrever_tudo(STDOUT_FILENO, msg);
}

void escrever_erro(const char *msg) {
    (void)escrever_tudo(STDERR_FILENO, msg);
}

/*
 * cd: muda o diretório atual do PRÓPRIO shell com chdir().
 *
 * Precisa ser builtin (rodar no shell, sem fork()): o diretório atual é
 * um atributo de cada processo. Se o cd rodasse num filho, só o filho
 * mudaria de diretório e, quando ele terminasse, o shell continuaria
 * onde estava.
 *
 * Sem argumento, vai para $HOME, como no bash. Depois de mudar,
 * atualiza a variável de ambiente PWD.
 */
static void executar_cd(int argc, char **argv) {
    const char *destino;
    char        atual[PATH_MAX];
    int         erro;

    if (argc > 2) {
        escrever_erro("minishell: cd: argumentos demais\n");
        return;
    }

    destino = (argc == 2) ? argv[1] : getenv("HOME");
    if (destino == NULL) {
        escrever_erro("minishell: cd: HOME não definido\n");
        return;
    }

    if (chdir(destino) == -1) {
        erro = errno;               /* guarda antes: o write() pode alterar o errno */
        escrever_erro("minishell: cd: ");
        escrever_erro(destino);
        escrever_erro(": ");
        escrever_erro(strerror(erro));
        escrever_erro("\n");
        return;
    }

    /* Atualiza a variável PWD, que os filhos herdam (ex.: printenv PWD).
     * O chdir() muda só o diretório do processo, não a variável. */
    if (getcwd(atual, sizeof(atual)) != NULL)
        setenv("PWD", atual, 1);
}

void loop_principal(void) {
    char    linha[MAX_LINHA];
    char   *argv[MAX_ARGS];
    ssize_t n;
    int     argc;

    while (1) {
        escrever("> ");

        n = ler_comando(linha, sizeof(linha));
        if (n == 0) {                   /* EOF (Ctrl+D): encerra */
            escrever("\n");
            break;
        }
        if (n == -1) {                  /* erro real de leitura: encerra */
            escrever_erro("minishell: erro de leitura\n");
            break;
        }

        argc = parse_comando(linha, argv);
        if (argc == 0)                  /* linha vazia ou descartada */
            continue;

        if (strcmp(argv[0], "exit") == 0)   /* builtin: encerra o shell */
            break;

        if (strcmp(argv[0], "cd") == 0) {   /* builtin: roda no próprio shell */
            executar_cd(argc, argv);
            continue;
        }

        executar_comando(argv);
    }
}

int main(void) {
    loop_principal();
    return 0;
}
