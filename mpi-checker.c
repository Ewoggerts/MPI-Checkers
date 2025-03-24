#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 8
#define MAX_DEPTH 8
#define MAX_RANKS 64

// 2d array representing checker board
typedef struct {
    int board[BOARD_SIZE][BOARD_SIZE]; // 8x8 board
} Board;



int main(int argc, char * argv[]){

    // MPI Initalization
    MPI_Init(&argc, &argv);
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Rank 0 is dynamically distributing and collecting the results
    if (rank == 0){

    }

}

