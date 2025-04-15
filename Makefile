CC=gcc
NVCC=nvcc

all: my_program

my_program: main.o kernel.o
	$(NVCC) main.o kernel.o -o my_program

main.o: mpi_checker_recursive_captures.c
	$(CC) -c mpi_checker_recursive_captures.c -o main.o

kernel.o: mpi-cuda.cu
	$(NVCC) -c mpi-cuda.cu -o kernel.o

clean:
	rm -f *.o my_program

