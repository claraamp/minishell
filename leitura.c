/* leitura.c — PESSOA 1: leitura da entrada e parsing */

#include <unistd.h>   /* read, STDIN_FILENO */
#include <string.h>   /* strtok */
#include <errno.h>    /* errno, EINTR */
#include "shell.h"

/*
 * Lê UMA linha da entrada padrão usando a chamada ao sistema read().
 *
 * Lemos 1 byte por vez até encontrar '\n'. Por que não um único
 * read(STDIN_FILENO, buffer, tamanho - 1)?
 *   - No terminal funcionaria (o terminal entrega uma linha por vez),
 *     mas quando a entrada vem de um pipe ou arquivo
 *     (ex.: printf "ls\necho oi\n" | ./minishell) um único read()
 *     pode trazer várias linhas juntas, e as seguintes se perderiam.
 *   - Lendo byte a byte, nunca consumimos além do '\n' da linha atual.
 *
 * Retorna: número de bytes lidos (>0), 0 em EOF (Ctrl+D), -1 em erro.
 */
ssize_t ler_comando(char *buffer, size_t tamanho) {
    size_t  n = 0;          /* caracteres guardados no buffer      */
    ssize_t total = 0;      /* bytes lidos da entrada nesta linha   */
    int     estourou = 0;   /* a linha passou do tamanho do buffer? */
    char    c;

    if (buffer == NULL || tamanho == 0)
        return -1;

    while (1) {
        ssize_t r = read(STDIN_FILENO, &c, 1);

        if (r == -1) {
            if (errno == EINTR)     /* interrompido por um sinal: tenta de novo */
                continue;
            return -1;              /* erro de verdade: o loop decide o que fazer */
        }

        if (r == 0) {               /* EOF (Ctrl+D) */
            if (total == 0)
                return 0;           /* nada foi digitado: fim da entrada */
            break;                  /* EOF no meio da linha: usa o que veio */
        }

        total++;

        if (c == '\n')              /* fim da linha: não guardamos o '\n' */
            break;

        if (n < tamanho - 1)        /* guarda se ainda cabe (sobra 1 p/ '\0') */
            buffer[n++] = c;
        else
            estourou = 1;           /* continua lendo só para descartar o resto */
    }

    buffer[n] = '\0';

    if (estourou) {
        escrever_erro("minishell: linha muito longa (ignorada)\n");
        buffer[0] = '\0';           /* vira linha vazia: o parse retorna 0 */
    }

    return total;
}

/*
 * Quebra a linha em tokens separados por espaço, tab, '\r' ou '\n',
 * usando strtok(). strtok modifica a própria linha: troca cada separador
 * por '\0', e argv[i] passa a apontar para dentro de `linha` (não há cópia).
 * argv termina com NULL, que é exigência do execvp().
 *
 * Cabem no máximo MAX_ARGS - 1 argumentos (1 posição é do NULL). Se a
 * linha tiver mais, o comando inteiro é descartado com um aviso, em vez
 * de rodar com menos argumentos do que o usuário digitou
 * (ex.: um rm com 100 arquivos apagaria só os primeiros).
 *
 * Retorna: quantidade de argumentos (argc). 0 = linha vazia ou descartada.
 */
int parse_comando(char *linha, char **argv) {
    const char *separadores = " \t\r\n";  /* \r: arquivos salvos no Windows */
    int i = 0;
    char *token;

    if (linha == NULL || argv == NULL) {
        if (argv != NULL)
            argv[0] = NULL;
        return 0;
    }

    token = strtok(linha, separadores);
    while (token != NULL) {
        if (i == MAX_ARGS - 1) {    /* só sobrou a posição do NULL */
            escrever_erro("minishell: argumentos demais (comando ignorado)\n");
            argv[0] = NULL;
            return 0;
        }
        argv[i++] = token;
        token = strtok(NULL, separadores);
    }

    argv[i] = NULL;
    return i;
}
