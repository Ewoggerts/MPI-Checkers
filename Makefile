MPICC=mpicc
NVCC=nvcc

# Target
TARGET=my_program

# Files
OBJS=main.o kernel.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(NVCC) $(OBJS) -o $(TARGET) -lmpi

main.o: mpi_checker_recursive_captures.c
	$(MPICC) -c mpi_checker_recursive_captures.c -o main.o

kernel.o: mpi-cuda.cu
	$(NVCC) -c mpi-cuda.cu -o kernel.o

clean:
	rm -f *.o $(TARGET)
