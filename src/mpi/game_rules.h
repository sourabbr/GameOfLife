#ifndef __GAME_RULES__
    #define __GAME_RULES__

    #include "mpi_utils.h"

    int game_execution_rules(int i, int j, int alive, char** prev, char** next);
    int compute_inner(SubGrid* sub_grid, char** prev, char** next);
    int compute_outer(SubGrid* sub_grid, char** prev, char** next);

    #endif