#include <cuda_runtime.h>
#include <curand.h>
#include <iostream>
#include <mpi_checker_recursive_captures>

#define THREADS_PER_BLOCK 64  // Define the number of threads per block

extern "C" 
{
    void runCudaAnalysis(BoardList *boards,  int *winners);
}

// CUDA kernel to count points inside the unit circle
__global__ void analyze(BoardList *boards, int *winner) {
    int local_tid = threadIdx.x;
    int boardIdx = blockIdx.x;

    int row = local_tid / 8;
    int col = local_tid % 8;

    __shared__ int sharedRed;
    __shared__ int sharedBlack;

    if (local_tid == 0) {
        sharedRed = 0;
        sharedBlack = 0;
    }
    __syncthreads();

    Board current_board = boards->board[boardIdx];
    char piece = current_board.board[row][col];

    if (piece == 'r' || piece == 'R') {
        atomicAdd(&sharedRed, 1);
    } else if (piece == 'b' || piece == 'B') {
        atomicAdd(&sharedBlack, 1);
    }

    __syncthreads();

    if (local_tid == 0) {
        int result;
        if (sharedRed > sharedBlack) {
            result = -1;  // red wins
        } else if (sharedBlack > sharedRed) {
            result = 1;  // black wins
        } else {
            result = 0;  // tie
        }
        winner[boardIdx] = result;
    }
}

// Reduction kernel to aggregate results
__global__ void reduce(int *d_winners, int *d_finalResult, int BATCH_SIZE) {
    __shared__ int shared_data[THREADS_PER_BLOCK];
    
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    
    // Initialize the shared data array
    if (tid < BATCH_SIZE) {
        shared_data[threadIdx.x] = d_winners[tid];
    } else {
        shared_data[threadIdx.x] = 0; // out of bounds elements contribute 0
    }
    __syncthreads();

    // Parallel reduction within the block
    for (int stride = blockDim.x / 2; stride > 0; stride /= 2) {
        if (threadIdx.x < stride && (tid + stride) < BATCH_SIZE) {
            shared_data[threadIdx.x] += shared_data[threadIdx.x + stride];
        }
        __syncthreads();
    }

    // Write the block's result into global memory
    if (threadIdx.x == 0) {
        atomicAdd(d_finalResult, shared_data[0]);
    }
}

// Function to run analysis
void runCudaAnalysis(BoardList *boards, int *likelihood) {
    const unsigned long long BATCH_SIZE = boards->count;

    BoardList *d_boards;
    cudaMalloc(&d_boards, sizeof(BoardList));
    cudaMemcpy(d_boards, boards, sizeof(BoardList), cudaMemcpyHostToDevice);

    int *d_winners;
    cudaMalloc(&d_winners, sizeof(int) * BATCH_SIZE);
    cudaMemset(d_winners, 0, sizeof(int) * BATCH_SIZE);

    analyze<<<BATCH_SIZE, 64>>>(d_boards, d_winners);

    cudaDeviceSynchronize();

    // Reduction ----------------------------------------------------------------------------------------------------------- 

    int *d_finalResult;
    cudaMalloc(&d_finalResult, sizeof(int));
    cudaMemset(d_finalResult, 0, sizeof(int));

    // Launch the reduction kernel
    int threadsPerBlock = 64;
    int numBlocks = (BATCH_SIZE + threadsPerBlock - 1) / threadsPerBlock;
    reduce<<<numBlocks, threadsPerBlock>>>(d_winners, d_finalResult, BATCH_SIZE);

    cudaDeviceSynchronize();

    // Copy the final result back to host
    int finalResult = 0;
    cudaMemcpy(&finalResult, d_finalResult, sizeof(int), cudaMemcpyDeviceToHost);

    // Store the result in the output array (e.g., total wins)
    *likelihood = finalResult;

    // Free allocated memory
    cudaFree(d_boards);
    cudaFree(d_winners);
    cudaFree(d_finalResult);
}
