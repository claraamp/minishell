# Mini Shell com Chamadas ao Sistema

Projeto Integrador — II Unidade — Prof. Dra. Larissa Barbosa Leôncio Pinheiro

Integrantes: Amanda Bastos de Melo Correia, Júlia Teixeira Gonçalves, Maria Clara Andrade Magalhães Paternostro D’Oliveira

Um interpretador de comandos simplificado, em C, que mostra um prompt, lê o comando digitado e o executa em um processo filho, usando diretamente as chamadas ao sistema do Linux.

## Como compilar e rodar

Requisitos: Linux (ou WSL), `gcc` e `make`.

```bash
make        # compila
./minishell # executa (ou: make run)
make clean  # remove os binários
```

Para sair do shell, digite `exit` ou pressione Ctrl+D em um prompt vazio.

## Estrutura do código

| Arquivo      | Responsabilidade                                   |
|--------------|----------------------------------------------------|
| `shell.h`    | Constantes e protótipos das funções (o "contrato" entre os módulos) |
| `leitura.c`  | `ler_comando()` e `parse_comando()`: leitura e parsing |
| `execucao.c` | `executar_comando()`: `fork`, `execvp` e `wait`     |
| `main.c`     | `escrever()`, `escrever_erro()` e `loop_principal()`: saída e controle do loop |

Fluxo de cada comando:

```
loop_principal: escrever("> ") -> ler_comando -> parse_comando -> exit? -> executar_comando -> repete
```

## Chamadas ao sistema utilizadas

| Chamada    | Onde                | Para quê |
|------------|---------------------|----------|
| `read()`   | `ler_comando`       | Lê a entrada padrão (fd 0) um byte por vez até o `\n`. Assim nunca consome a linha seguinte quando a entrada vem de um pipe ou arquivo. |
| `write()`  | `escrever`, `escrever_erro` | Escreve o prompt e as mensagens na saída padrão (fd 1) ou na saída de erro (fd 2). Um laço trata escrita parcial e interrupção por sinal (`EINTR`). |
| `fork()`   | `executar_comando`  | Cria um processo filho, cópia do shell, para rodar o comando sem substituir o próprio shell. |
| `execvp()` | `executar_comando`  | No filho, substitui o programa pelo comando digitado, procurando o executável no `PATH`. É uma função da libc sobre a chamada `execve()`. |
| `wait()`   | `executar_comando`  | O pai espera o filho terminar e lê o código de saída. Evita processos zumbis e garante que o prompt só volte depois da saída do comando. |
| `_exit()`  | `executar_comando`  | Encerra o filho quando o `execvp()` falha (126 sem permissão, 127 comando não encontrado), sem esvaziar buffers herdados do pai. |
| `signal()` | `main`, `executar_comando` | O shell ignora `SIGINT` (Ctrl+C) e `SIGQUIT` (Ctrl+\) para não ser encerrado; o filho restaura o comportamento padrão antes do `execvp()`, porque sinais ignorados continuam ignorados depois do `exec`. É uma função da libc sobre a chamada `sigaction()`. |

O `exit` é tratado dentro do próprio loop, sem `fork()`: quem precisa terminar é o processo do shell, e não existe um programa `exit` para o `execvp()` encontrar.

## Exemplos de comandos testados

Sessão interativa:

```
> echo Ola mundo
Ola mundo
> ls -l
total 56
-rw-rw-r-- 1 clara clara  2634 Nov  1 11:16 execucao.c
-rw-rw-r-- 1 clara clara  2880 Nov  1 11:18 execucao.o
-rw-rw-r-- 1 clara clara  3549 Nov  1 11:16 leitura.c
-rw-rw-r-- 1 clara clara  2608 Nov  1 11:18 leitura.o
-rw-rw-r-- 1 clara clara  2100 Nov  1 11:17 main.c
-rw-rw-r-- 1 clara clara  3072 Nov  1 11:18 main.o
-r--r--r-- 1 clara clara   281 Oct 26 18:15 Makefile
-rwxrwxr-x 1 clara clara 16784 Nov  1 11:18 minishell
-rw-r--r-- 1 clara clara  1616 Oct 31 20:47 README.md
-rw-rw-r-- 1 clara clara  1499 Nov  1 11:16 shell.h
> cat Makefile
CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L
OBJS   = main.o leitura.o execucao.o

minishell: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c shell.h
	$(CC) $(CFLAGS) -c $<

run: minishell
	./minishell

clean:
	rm -f $(OBJS) minishell

.PHONY: run clean
>
> comandoquenaoexiste
minishell: comandoquenaoexiste: comando não encontrado
> exit
```

Entrada por pipe (os prompts aparecem juntos porque não há terminal ecoando o que foi digitado; o comando depois do `exit` não é executado):

```
$ printf 'echo Ola mundo\nls -l shell.h\n\n    \ncomandoquenaoexiste\nexit\necho nao deve aparecer\n' | ./minishell
> Ola mundo
> -rw-rw-r-- 1 clara clara 1499 Nov  1 11:16 shell.h
> > > minishell: comandoquenaoexiste: comando não encontrado
>
```

## Limitações conhecidas

- Sem pipes (`|`), redirecionamento (`>`, `<`) ou execução em segundo plano (`&`).
- Sem aspas nem escapes: `echo "a b"` imprime as aspas, e argumentos com espaço não são possíveis.
- Sem expansão de variáveis ou curingas: `$HOME` e `*.c` são passados literalmente.
- `cd` não é builtin: rodar `cd` não muda o diretório do shell.
- `exit` ignora argumentos (`exit 3` encerra com código 0).
- Linhas com mais de 1023 caracteres são descartadas com aviso; comandos com mais de 63 argumentos também.
- Ctrl+C e Ctrl+\ no prompt só descartam o que foi digitado: o shell não pula linha nem mostra um novo prompt, como o bash faz.
- As mensagens de erro do sistema saem em inglês (ex.: `Permission denied`), porque vêm do `strerror()`; só a de comando não encontrado é nossa.
- Sem histórico nem edição de linha com as setas.

## Vídeo de demonstração

TODO: link (máx. 5 min)
