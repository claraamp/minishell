# Mini Shell com Chamadas ao Sistema

Projeto Integrador — II Unidade — Prof. Dra. Larissa Barbosa Leôncio Pinheiro
Integrantes: Amanda Bastos de Melo Correia, Júlia Teixeira Gonçalves, Maria Clara Andrade Magalhães Paternostro D’Oliveira

## Como compilar e rodar

```bash
make        # compila
./minishell # executa (ou: make run)
make clean  # remove os binários
```

## Estrutura do código

| Arquivo      | Responsabilidade                                   |
|--------------|----------------------------------------------------|
| `shell.h`    | Constantes e protótipos das funções                |
| `leitura.c`  | `ler_comando()` e `parse_comando()`                |
| `execucao.c` | `executar_comando()` (fork, execvp, wait)          |
| `main.c`     | `escrever()`, `escrever_erro()`, `loop_principal()`|

## Chamadas ao sistema utilizadas

| Chamada    | Onde              | Para quê |
|------------|-------------------|----------|
| `read()`   | `ler_comando`     | TODO     |
| `write()`  | `escrever`        | TODO     |
| `fork()`   | `executar_comando`| TODO     |
| `execvp()` | `executar_comando`| TODO     |
| `wait()`   | `executar_comando`| TODO     |

## Exemplos de comandos testados

> Colar aqui as saídas REAIS do terminal.

```
> ls -l
TODO

> echo Ola mundo
TODO

> cat teste.txt
TODO

> comandoinexistente
TODO

> exit
```

## Limitações conhecidas

- TODO (ex.: sem pipes `|`, sem redirecionamento `>`/`<`, sem aspas, sem variáveis de ambiente, sem execução em background `&`, limite de MAX_LINHA caracteres)

## Vídeo de demonstração

TODO: link (máx. 5 min)
