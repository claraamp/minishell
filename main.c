/* main.c — PESSOA 3: saída, loop principal e integração */

#include <unistd.h>   /* write, STDOUT_FILENO, STDERR_FILENO */
#include <string.h>   /* strlen, strcmp */
#include "shell.h"

void escrever(const char *msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
    (void)msg;
}

void escrever_erro(const char *msg) {
    write(STDERR_FILENO, msg, strlen(msg));
    (void)msg;
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
