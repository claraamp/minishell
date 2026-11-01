/* main.c — PESSOA 3: saída, loop principal e integração */

#include <unistd.h>   /* write, STDOUT_FILENO, STDERR_FILENO */
#include <string.h>   /* strlen, strcmp */
#include <errno.h>    /* errno, EINTR */
#include "shell.h"

/*
* Escreve a string inteira no descritor fd usando write().
*
* write() pode escrever MENOS bytes do que foi pedido (escrita parcial,
* comum em pipes), então repetimos a partir de onde parou até acabar.
* Se for interrompido por um sinal (EINTR), tentamos de novo.
*
* Retorna: 0 se escreveu tudo, -1 em erro.
*/
static int escrever_tudo(int fd, const char *msg) {
    size_t restante = strlen(msg);

    while (restante > 0) {
        ssize_t r = write(fd, msg, restante);

        if (r == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        }

        msg      += r;   /* avança até onde já foi escrito */
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
    char linha[MAX_LINHA];
    char *argv[MAX_ARGS];

    /*
     * TODO (dentro de um while (1)):
     * 1. escrever("> ");
     * 2. ssize_t n = ler_comando(linha, sizeof(linha));
     *      n == 0  -> EOF: escrever("\n") e sair do loop (break).
     *      n == -1 -> escrever_erro("erro de leitura\n") e continue.
     * 3. int argc = parse_comando(linha, argv);
     *      argc == 0 -> linha vazia: continue.
     * 4. Se strcmp(argv[0], "exit") == 0 -> sair do loop (break).
     * 5. (Opcional/extra) Se argv[0] for "cd" -> chdir(argv[1]) aqui,
     *    SEM fork. Explicar na apresentação por que cd precisa ser builtin.
     * 6. executar_comando(argv);
     */
    (void)linha;
    (void)argv;
}

int main(void) {
    loop_principal();
    return 0;
}
