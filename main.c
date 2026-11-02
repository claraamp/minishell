/* main.c — PESSOA 3: saída, loop principal e integração */

#include <unistd.h>   /* write, STDOUT_FILENO, STDERR_FILENO */
#include <string.h>   /* strlen, strcmp */
#include <errno.h>    /* errno, EINTR */
#include <signal.h>   /* signal, SIGINT, SIG_IGN */
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

        executar_comando(argv);
    }
}

int main(void) {
    /* Ctrl+C (SIGINT) não deve derrubar o shell, só o comando em execução. */
    signal(SIGINT, SIG_IGN);

    loop_principal();
    return 0;
}
