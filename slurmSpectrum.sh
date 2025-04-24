#!/bin/bash
module load xl_r spectrum-mpi cuda/11.2
mpirun --map-by node --bind-to core --report-bindings -np 1 ./mpi-cuda-exe
mpirun --map-by node --bind-to core --report-bindings -np 2 ./mpi-cuda-exe
mpirun --map-by node --bind-to core --report-bindings -np 3 ./mpi-cuda-exe
mpirun --map-by node --bind-to core --report-bindings -np 4 ./mpi-cuda-exe
mpirun --map-by node --bind-to core --report-bindings -np 5 ./mpi-cuda-exe
mpirun --map-by node --bind-to core --report-bindings -np 6 ./mpi-cuda-exe
mpirun --map-by node --bind-to core --report-bindings -np 8 ./mpi-cuda-exe
mpirun --map-by node --bind-to core --report-bindings -np 12 ./mpi-cuda-exe
