/* leitura.c — PESSOA 1: leitura da entrada e parsing */

#include <unistd.h>   /* read, STDIN_FILENO */
#include <string.h>   /* strtok */
#include "shell.h"

ssize_t ler_comando(char *buffer, size_t tamanho) {
    /*
     * TODO:
     * 1. Chamar read(STDIN_FILENO, buffer, tamanho - 1).
     * 2. Se retornar 0  -> EOF (usuário apertou Ctrl+D): retornar 0.
     * 3. Se retornar -1 -> erro: retornar -1 (o loop decide o que fazer).
     * 4. Colocar '\0' no fim do que foi lido (buffer[n] = '\0').
     * 5. Se o último caractere for '\n', trocar por '\0'.
     * 6. Retornar n.
     */
    (void)buffer;
    (void)tamanho;
    return 0;
}

int parse_comando(char *linha, char **argv) {
    /*
     * TODO:
     * 1. Usar strtok(linha, " \t") para pegar o primeiro token.
     * 2. Em loop, guardar cada token em argv[i] e chamar strtok(NULL, " \t").
     * 3. Parar quando o token for NULL ou i chegar a MAX_ARGS - 1.
     * 4. Colocar argv[i] = NULL no final (obrigatório para o execvp).
     * 5. Retornar i (quantidade de argumentos).
     *
     * Testar: "ls -l", "   echo   Ola   mundo  ", linha vazia, só espaços.
     */
    (void)linha;
    argv[0] = NULL;
    return 0;
}
