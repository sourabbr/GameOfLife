#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <mpi.h>

#include "../common/args.h"
#include "mpi_utils.h"

Arguments args;

int main(int argc, char** argv) {
    int ndims = 2, reorder = 1;
    int dims[ndims], periods[ndims];
    int my_rank, num_procs;
    MPI_Comm comm;

    // MPI initialization
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    // Parse command line arguments
    parseArguments(argc, argv, &args);
    printArguments(&args);

    // Create cartesian topology for processes and assign neighbours
    dims[0] = dims[1] = (int) sqrt(num_procs);
    periods[0] = periods[1] = 0;
    int err = 0;
    MPI_Dims_create(num_procs, ndims, dims);
    err = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &comm);
    if (err != 0) {
        printf("ERROR[%d] creating cartesian topology of processes\n", err);
    }
    NeighborRanks neighbors;
    determineAdjacentRanks(comm, &neighbors);

    // Split input grid into subgrid for each process
    SubGrid subgrid;
    splitGrid(num_procs, args.grid_size, &subgrid);

    // Each process reads its subgrid from the input file
    char** grid = parallelFileRead(args.input_file_name, my_rank, args.grid_size, &subgrid);
    parallelFileWrite(args.output_file_name, my_rank, num_procs, args.grid_size, &subgrid, grid);


    // Wait for all processes to finish reading the input file
    MPI_Barrier(MPI_COMM_WORLD);


    MPI_Finalize();

    return 0;
}
