CC = mpicc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm

# Trova automaticamente tutti i file .c nella directory
SRCS = $(wildcard *.c)

# Genera i nomi degli eseguibili rimuovendo l'estensione .c
TARGETS = $(SRCS:.c=)

.PHONY: all clean

# Compila tutti gli eseguibili (target predefinito)
all: $(TARGETS)

# Regola generica per compilare ogni singolo file .c nel rispettivo eseguibile
%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Rimuove tutti gli eseguibili compilati
clean:
	rm -f $(TARGETS)
