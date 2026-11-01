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
