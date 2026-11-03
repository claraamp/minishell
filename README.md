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
loop_principal: escrever("> ") -> ler_comando -> parse_comando -> exit? -> cd? -> executar_comando -> repete
```

## Chamadas ao sistema utilizadas

| Chamada    | Onde                | Para quê |
|------------|---------------------|----------|
| `read()`   | `ler_comando`       | Lê a entrada padrão (fd 0) um byte por vez até o `\n`. Assim nunca consome a linha seguinte quando a entrada vem de um pipe ou arquivo. |
| `write()`  | `escrever`, `escrever_erro` | Escreve o prompt e as mensagens na saída padrão (fd 1) ou na saída de erro (fd 2). Um laço trata escrita parcial e interrupção por sinal (`EINTR`). |
| `fork()`   | `executar_comando`  | Cria um processo filho, cópia do shell, para rodar o comando sem substituir o próprio shell. |
| `open()`   | `redirecionar_saida` | No filho, abre (ou cria) o arquivo do `>` para escrita, apagando o conteúdo anterior (`O_WRONLY \| O_CREAT \| O_TRUNC`). |
| `dup2()`   | `redirecionar_saida` | Copia o descritor do arquivo para o fd 1, para que a saída padrão do comando vá para o arquivo. |
| `close()`  | `redirecionar_saida` | Fecha o descritor original do arquivo depois do `dup2()`, já que o fd 1 passou a apontar para ele. |
| `execvp()` | `executar_comando`  | No filho, substitui o programa pelo comando digitado, procurando o executável no `PATH`. É uma função da libc sobre a chamada `execve()`. |
| `wait()`   | `executar_comando`  | O pai espera o filho terminar e lê o código de saída. Evita processos zumbis e garante que o prompt só volte depois da saída do comando. |
| `chdir()`  | `executar_cd`       | Muda o diretório atual do próprio shell no builtin `cd` (sem argumento, vai para `$HOME`). |
| `getcwd()` | `executar_cd`       | Lê o diretório atual depois do `chdir()` para atualizar a variável `PWD`, que os comandos executados depois herdam. |
| `_exit()`  | `executar_comando`  | Encerra o filho quando o `execvp()` falha (126 sem permissão, 127 comando não encontrado) ou quando o `>` dá errado (2 erro de sintaxe, 1 arquivo não pôde ser aberto), sem esvaziar buffers herdados do pai. |
| `signal()` | `main`, `executar_comando` | O shell ignora `SIGINT` (Ctrl+C) e `SIGQUIT` (Ctrl+\) para não ser encerrado; o filho restaura o comportamento padrão antes do `execvp()`, porque sinais ignorados continuam ignorados depois do `exec`. É uma função da libc sobre a chamada `sigaction()`. |

O `exit` é tratado dentro do próprio loop, sem `fork()`: quem precisa terminar é o processo do shell, e não existe um programa `exit` para o `execvp()` encontrar.

O `cd` também é builtin pelo mesmo motivo: o diretório atual é de cada processo. Se o `cd` rodasse em um filho, só o filho mudaria de diretório, e o shell continuaria onde estava.

O redirecionamento `>` é feito só no filho, depois do `fork()` e antes do `execvp()`: o comando escreve no arquivo, mas o shell continua escrevendo o prompt no terminal. Como o `execvp()` mantém os descritores abertos, o programa novo já começa com a saída padrão apontando para o arquivo.

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

Builtin `cd` (o `printenv PWD` mostra que a variável `PWD` também é atualizada):

```
> pwd
/tmp
> cd /usr/share
> pwd
/usr/share
> cd ..
> pwd
/usr
> printenv PWD
/usr
> cd naoexiste
minishell: cd: naoexiste: No such file or directory
> cd
> pwd
/home/jujujuh
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

Redirecionamento de saída (`>`). Só a saída padrão (fd 1) vai para o arquivo; a saída de erro (fd 2) continua no terminal, por isso a mensagem do `ls` aparece:

```
> echo Ola arquivo > saida.txt
> cat saida.txt
Ola arquivo
> ls naoexiste > erro.txt
ls: cannot access 'naoexiste': No such file or directory
> ls >
minishell: erro de sintaxe: falta o arquivo depois de '>'
> ls > /naoexiste/x.txt
minishell: /naoexiste/x.txt: No such file or directory
```

## Limitações conhecidas

- Sem pipes (`|`), redirecionamento de entrada (`<`) ou execução em segundo plano (`&`).
- O `>` precisa de espaços em volta (`ls > a.txt`, não `ls>a.txt`) e deve ficar no fim do comando; não há `>>` (anexar) nem `2>`, e ele não vale para os builtins `cd` e `exit`.
- Sem aspas nem escapes: `echo "a b"` imprime as aspas, e argumentos com espaço não são possíveis.
- Sem expansão de variáveis, `~` ou curingas: `$HOME`, `~` e `*.c` são passados literalmente (`cd ~` não funciona; use só `cd`).
- `cd -` (voltar ao diretório anterior) não é suportado.
- `exit` ignora argumentos (`exit 3` encerra com código 0).
- Linhas com mais de 1023 caracteres são descartadas com aviso; comandos com mais de 63 argumentos também.
- Ctrl+C e Ctrl+\ no prompt só descartam o que foi digitado: o shell não pula linha nem mostra um novo prompt, como o bash faz.
- As mensagens de erro do sistema saem em inglês (ex.: `Permission denied`), porque vêm do `strerror()`; só a de comando não encontrado é nossa.
- Sem histórico nem edição de linha com as setas.

## Vídeo de demonstração

TODO: link (máx. 5 min)
