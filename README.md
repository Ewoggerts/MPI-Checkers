## CSCI4320 Project Guideline

### How to run:

First, load modules using ``module load xl_r spectrum-mpi cuda/11.2``

Then, using the Makefile provided, run ``make mpi-cuda``

Then, allocate 3 nodes on the el8-rpi part with ``sbatch -N 3 --partition=el8-rpi --gres=gpu:4 -t 20
./slurmSpectrum.sh``
Note: 20 minutes should be more than enough, but is a recommendation just to be safe.

### Running Weak Scaling
The submitted code will run strong scaling. To run weak scaling, change lines 451-457 from 
```
    int boardsPerRank = TOTAL_BOARDS / size;
    //int boardsPerRank = TOTAL_BOARDS;
    
    int left = TOTAL_BOARDS % size;
    if (rank < left) {
        boardsPerRank++;
    }
```
to 
```
    //int boardsPerRank = TOTAL_BOARDS / size;
    int boardsPerRank = TOTAL_BOARDS;
    
    //int left = TOTAL_BOARDS % size;
    //if (rank < left) {
    //    boardsPerRank++;
    //}
```
and line 10 from 
```
#define TOTAL_BOARDS 1024
```
to 
```
#define TOTAL_BOARDS 500
```

### Running CPU Only
To run CPU-Only, change line 477 from 
```
runCudaAnalysis(&board_results, &likelihood);
```
to 
```
runCudaAnalysis_serial(&board_results, &likelihood);
```

### Viewing output

Due to the files being interpreted as binary, the best way to view the results is by using the ``cat`` command on the txt files generated. Alternatively you could export them and put them in a binary to UTF converter. 
