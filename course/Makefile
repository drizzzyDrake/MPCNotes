MPICC = mpicc
CC = gcc

CFLAGS = -Wall -Wextra -O2
OMPFLAGS = -fopenmp

LDFLAGS = -lm

# Trova automaticamente i sorgenti
MPI_SRCS = $(wildcard *_mpi.c)
OMP_SRCS = $(wildcard *_omp.c)
C_SRCS   = $(filter-out $(MPI_SRCS) $(OMP_SRCS), $(wildcard *.c))

# Genera gli eseguibili togliendo .c
MPI_TARGETS = $(MPI_SRCS:.c=)
OMP_TARGETS = $(OMP_SRCS:.c=)
C_TARGETS   = $(C_SRCS:.c=)

.PHONY: all mpi openmp c clean

# Compila tutto
all: mpi openmp c

# Solo MPI
mpi: $(MPI_TARGETS)

# Solo OpenMP
openmp: $(OMP_TARGETS)

# Solo C semplice
c: $(C_TARGETS)

# Regola compilazione MPI
%_mpi: %_mpi.c
	$(MPICC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Regola compilazione OpenMP
%_omp: %_omp.c
	$(CC) $(CFLAGS) $(OMPFLAGS) $< -o $@ $(LDFLAGS)

# Regola compilazione C normale
%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(MPI_TARGETS) $(OMP_TARGETS) $(C_TARGETS)
