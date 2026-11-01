/* execucao.c — PESSOA 2: criação e execução de processos */

#include <unistd.h>     /* fork, execvp, _exit */
#include <sys/wait.h>   /* wait, WIFEXITED, WEXITSTATUS */
#include <errno.h>      /* errno, ENOENT */
#include <string.h>     /* strerror */
#include "shell.h"

int executar_comando(char **argv) {
    /*
     * TODO:
     * 1. pid_t pid = fork();
     *
     * 2. Se pid < 0  -> erro no fork:
     *      escrever_erro("erro ao criar processo\n"); retornar -1.
     *
     * 3. Se pid == 0 -> PROCESSO FILHO:
     *      execvp(argv[0], argv);
     *      Se chegou na linha seguinte, o execvp falhou. Então:
     *        - se errno == ENOENT: escrever_erro("<comando>: comando não encontrado\n")
     *        - senão: escrever_erro com strerror(errno)
     *      _exit(127);   (usar _exit, não exit, dentro do filho)
     *
     * 4. Se pid > 0  -> PROCESSO PAI:
     *      int status; wait(&status);
     *      Se WIFEXITED(status), retornar WEXITSTATUS(status).
     *      Senão, retornar -1.
     *
     * Testar: "ls -l", "cat arquivo.txt", "echo Ola mundo",
     *         "comandoquenaoexiste", "cat arquivoquenaoexiste".
     */
    (void)argv;
    return 0;
}
