#ifndef MPI_UTILS_H
#define MPI_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

typedef struct NeighborRanks {
	int up;
	int down;
	int left;
	int right;
	int up_left;
	int up_right;
	int down_left;
	int down_right;
} NeighborRanks;

typedef struct SubGrid {
	int rows;
	int cols;
	int factor;
} SubGrid;

void determineAdjacentRanks(MPI_Comm comm, NeighborRanks* neighbor_ranks);
void splitGrid(int num_procs, int grid_size, SubGrid* subgrid);
char** create_2d_char_array(int row_count, int column_count);
char** parallelFileRead(const char* path, int rank, int num_procs, SubGrid* subgrid);
void parallelFileWrite(const char* destination_file, int rank, int total_procs, int grid_size, SubGrid* subgrid, char** proc_grid);


#endif // MPI_UTILS_H