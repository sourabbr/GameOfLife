#include "mpi_utils.h"

void determineAdjacentRanks(MPI_Comm comm, NeighborRanks* neighbor_ranks) {
    // Shifts for the Cartesian topology
    int row_shift = 0;
    int col_shift = 1;
    int displacement = 1;

    // Find the rank of the neighboring processes in the row and column
    MPI_Cart_shift(comm, row_shift, displacement, &neighbor_ranks->up, &neighbor_ranks->down);
    MPI_Cart_shift(comm, col_shift, displacement, &neighbor_ranks->left, &neighbor_ranks->right);

    // Determine the diagonal neighbors based on the existing ones
    neighbor_ranks->up_left = (neighbor_ranks->left != MPI_PROC_NULL) ? neighbor_ranks->up - 1 : MPI_PROC_NULL;
    neighbor_ranks->up_right = (neighbor_ranks->right != MPI_PROC_NULL) ? neighbor_ranks->up + 1 : MPI_PROC_NULL;
    neighbor_ranks->down_left = (neighbor_ranks->left != MPI_PROC_NULL) ? neighbor_ranks->down - 1 : MPI_PROC_NULL;
    neighbor_ranks->down_right = (neighbor_ranks->right != MPI_PROC_NULL) ? neighbor_ranks->down + 1 : MPI_PROC_NULL;

    // Adjust for any ranks that are invalid (-1 or less) to MPI_PROC_NULL
    if (neighbor_ranks->down < 0) neighbor_ranks->down = MPI_PROC_NULL;
    if (neighbor_ranks->up < 0) neighbor_ranks->up = MPI_PROC_NULL;
    if (neighbor_ranks->left < 0) neighbor_ranks->left = MPI_PROC_NULL;
    if (neighbor_ranks->right < 0) neighbor_ranks->right = MPI_PROC_NULL;
    if (neighbor_ranks->down_left < 0) neighbor_ranks->down_left = MPI_PROC_NULL;
    if (neighbor_ranks->down_right < 0) neighbor_ranks->down_right = MPI_PROC_NULL;
    if (neighbor_ranks->up_left < 0) neighbor_ranks->up_left = MPI_PROC_NULL;
    if (neighbor_ranks->up_right < 0) neighbor_ranks->up_right = MPI_PROC_NULL;
}

void splitGrid(int num_procs, int grid_size, SubGrid* subgrid) {
    subgrid->factor = sqrt((double)num_procs);
    subgrid->rows = subgrid->cols = grid_size / subgrid->factor;
}

char** create_2d_char_array(int row_count, int column_count) {
    char* block = calloc(row_count * column_count, sizeof(char));
    memset(block, '0', row_count * column_count);
    char** grid_matrix = calloc(row_count, sizeof(char*));
    for (int idx = 0; idx < row_count; ++idx) {
        grid_matrix[idx] = &block[idx * column_count];
    }
    return grid_matrix;
}

char** parallelFileRead(const char* path, int rank, int grid_size, SubGrid* subgrid) {
    int initial_row = (rank / subgrid->factor) * subgrid->rows;
    int initial_col = (rank % subgrid->factor) * subgrid->cols;

    MPI_File mpi_input_file;
    MPI_File_open(MPI_COMM_WORLD, path, MPI_MODE_RDONLY, MPI_INFO_NULL, &mpi_input_file);

    char* temp_line = (char*)malloc(subgrid->cols * sizeof(char));
    char** proc_grid = create_2d_char_array(subgrid->rows + 2, subgrid->cols + 2);

    for (int current_row = 0; current_row < subgrid->rows; ++current_row) {
        int file_row_offset = (initial_row + current_row) * grid_size;
        int file_offset = file_row_offset + initial_col;

        MPI_File_seek(mpi_input_file, file_offset, MPI_SEEK_SET);
        MPI_File_read(mpi_input_file, temp_line, subgrid->cols, MPI_CHAR, MPI_STATUS_IGNORE);

        for (int col = 0; col < subgrid->cols; ++col) {
            proc_grid[current_row + 1][col + 1] = temp_line[col];
        }
    }

    free(temp_line);
    MPI_File_close(&mpi_input_file);

    return proc_grid;
}

void parallelFileWrite(const char* destination_file, int rank, int total_procs, int grid_size, SubGrid* subgrid, char** proc_grid) {
    int lead_row = (rank / subgrid->factor) * subgrid->rows;
    int lead_column = (rank % subgrid->factor) * subgrid->cols;

    MPI_File fd;
    MPI_Offset file_offset;
    MPI_File_open(MPI_COMM_WORLD, destination_file, MPI_MODE_CREATE | MPI_MODE_WRONLY | MPI_MODE_APPEND, MPI_INFO_NULL, &fd);
    int err = MPI_File_get_position(fd, &file_offset);
    if (err != 0) {
        printf("ERROR[%d] getting position of output file\n", err);
    }

    char* buffer_row = (char*)malloc(subgrid->cols * sizeof(char));

    for (int current_row = 0; current_row < subgrid->rows; ++current_row) {
        int row_position = (lead_row + current_row) * grid_size;
        int position_to_write = file_offset + (row_position + lead_column);

        MPI_File_seek(fd, position_to_write, MPI_SEEK_SET);

        for (int col = 0; col < subgrid->cols; ++col) {
            buffer_row[col] = proc_grid[current_row + 1][col + 1];
        }

        MPI_File_write(fd, buffer_row, subgrid->cols, MPI_CHAR, MPI_STATUS_IGNORE);
    }

    // Write a newline at the end of the last process's data
    if (rank == total_procs - 1) {
        MPI_File_write(fd, "\n", 1, MPI_CHAR, MPI_STATUS_IGNORE);
    }

    free(buffer_row);
    MPI_File_close(&fd);
}

