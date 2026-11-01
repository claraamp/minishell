#ifndef SHELL_H
#define SHELL_H

#include <sys/types.h>  /* ssize_t */
#include <stddef.h>     /* size_t  */

#define MAX_LINHA 1024  /* tamanho máximo de uma linha digitada   */
#define MAX_ARGS  64    /* número máximo de argumentos por comando */

/* ================= leitura.c — PESSOA 1 ================= */

/*
 * Lê uma linha da entrada padrão usando read().
 * Retorna: número de bytes lidos (>0), 0 em EOF (Ctrl+D), -1 em erro.
 */
ssize_t ler_comando(char *buffer, size_t tamanho);

/*
 * Quebra a linha em tokens separados por espaço/tab.
 * Preenche argv[] e termina com NULL (exigência do execvp).
 * Retorna: quantidade de argumentos (argc). 0 = linha vazia.
 */
int parse_comando(char *linha, char **argv);

/* ================= execucao.c — PESSOA 2 ================= */

/*
 * Cria um processo filho com fork(), executa argv com execvp()
 * e faz o pai aguardar com wait().
 * Retorna: código de saída do filho, ou -1 se o fork falhar.
 */
int executar_comando(char **argv);

/* ================= main.c — PESSOA 3 ================= */

/* Escreve uma string na saída padrão usando write(). */
void escrever(const char *msg);

/* Escreve uma string na saída de erro (STDERR) usando write(). */
void escrever_erro(const char *msg);

/* Loop: prompt -> ler -> parse -> exit? -> executar -> repete. */
void loop_principal(void);

#endif
