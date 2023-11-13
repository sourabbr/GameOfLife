#include "mpi_utils.h"
#include "game_rules.h"

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

    // Adjust for any ranks that are invalid to MPI_PROC_NULL
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
            buffer_row[col] = (char)proc_grid[current_row + 1][col + 1];
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

void simulateGOL(char** cur_step_grid, int my_rank, int num_procs, MPI_Comm comm, NeighborRanks* neighbors, SubGrid* subgrid, Arguments* args) {

    MPI_Datatype column, row;
    MPI_Request send[8], recv[8];
    MPI_Status status[8];

    MPI_Type_vector(subgrid->rows, 1, subgrid->cols + 2, MPI_CHAR, &column);
    MPI_Type_commit(&column);
    MPI_Type_contiguous(subgrid->cols, MPI_CHAR, &row);
    MPI_Type_commit(&row);

    char** next_step_grid = create_2d_char_array(subgrid->rows + 2, subgrid->cols + 2);
    char** temp;

    for (int i = 0; i < args->iterations; ++i) {
        // Receive and send data to each neighbor process

        // Receive and send: UP
        MPI_Irecv(&cur_step_grid[0][1], 1, row, neighbors->up, 0, comm, &recv[0]);
        MPI_Isend(&cur_step_grid[1][1], 1, row, neighbors->up, 0, comm, &send[0]);

        // Receive and send: DOWN
        MPI_Irecv(&cur_step_grid[subgrid->rows + 1][1], 1, row, neighbors->down, 0, comm, &recv[1]);
        MPI_Isend(&cur_step_grid[subgrid->rows][1], 1, row, neighbors->down, 0, comm, &send[1]);

        // Receive and send: LEFT
        MPI_Irecv(&cur_step_grid[1][0], 1, column, neighbors->left, 0, comm, &recv[2]);
        MPI_Isend(&cur_step_grid[1][1], 1, column, neighbors->left, 0, comm, &send[2]);

        // Receive and send: RIGHT
        MPI_Irecv(&cur_step_grid[1][subgrid->cols + 1], 1, column, neighbors->right, 0, comm, &recv[3]);
        MPI_Isend(&cur_step_grid[1][subgrid->cols], 1, column, neighbors->right, 0, comm, &send[3]);

        // Receive and send: UP LEFT
        MPI_Irecv(&cur_step_grid[0][0], 1, MPI_CHAR, neighbors->up_left, 0, comm, &recv[4]);
        MPI_Isend(&cur_step_grid[1][1], 1, MPI_CHAR, neighbors->up_left, 0, comm, &send[4]);

        // Receive and send: UP RIGHT
        MPI_Irecv(&cur_step_grid[0][subgrid->cols + 1], 1, MPI_CHAR, neighbors->up_right, 0, comm, &recv[5]);
        MPI_Isend(&cur_step_grid[1][subgrid->cols], 1, MPI_CHAR, neighbors->up_right, 0, comm, &send[5]);

        // Receive and send: DOWN LEFT
        MPI_Irecv(&cur_step_grid[subgrid->rows + 1][0], 1, MPI_CHAR, neighbors->down_left, 0, comm, &recv[6]);
        MPI_Isend(&cur_step_grid[subgrid->rows][1], 1, MPI_CHAR, neighbors->down_left, 0, comm, &send[6]);

        // Receive and send: DOWN RIGHT
        MPI_Irecv(&cur_step_grid[subgrid->rows + 1][subgrid->cols + 1], 1, MPI_CHAR, neighbors->down_right, 0, comm, &recv[7]);
        MPI_Isend(&cur_step_grid[subgrid->rows][subgrid->cols], 1, MPI_CHAR, neighbors->down_right, 0, comm, &send[7]);

        // No need to wait for receive from all neighbor processes for calculating next step for inner grid
        compute_inner( subgrid, cur_step_grid, next_step_grid);

        // Wait to receive from all neighbor processes.
        MPI_Waitall(8, recv, status);

        // Now, we can calculate the next step for outer grid
        compute_outer( subgrid, cur_step_grid, next_step_grid);

        // Update current step grid with calculated next step grid
        temp = cur_step_grid;
        cur_step_grid = next_step_grid;
        next_step_grid = temp;

        // Wait to send to all neighbor processes.
        MPI_Waitall(8, send, status);

        // Output current step grid
        parallelFileWrite(args->output_file_name, my_rank, num_procs, args->grid_size, subgrid, cur_step_grid);

        // Wait for all processes to finish writing to output file
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Type_free(&column);
    MPI_Type_free(&row);
    // Free the entrie grid
    free(next_step_grid[0]);
    // Free pointers to rows in the grid
    free(next_step_grid);
}

