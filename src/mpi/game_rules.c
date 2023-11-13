#include <stdio.h>
#include <stdlib.h>
#include "game_rules.h"

int game_execution_rules(int i, int j, int alive, char** prev, char** next){
    int modified = 0;
    //Rules for a dead cell
    //if 3 neighbors are alive, 
    // the current cell becomes alive
    if(prev[i][j] != '1'){
        if(alive == 3){
            next[i][j] = '1';
            modified = 1;
        }else{
            next[i][j] = '0';
        }
    }
    //Rules for an alive cell
    else{
        if(alive != 3 && alive !=2){
            next[i][j] = '0';
            modified = 1;
        }else{
            next[i][j] = '1';
        }
    }
    return modified;
}

int compute_inner(SubGrid* sub_grid, char** prev, char** next){
    int modified = 0;

    #pragma omp parallel for collapse(2) schedule(static)

    for(int i = 2;i <= sub_grid->rows-1;i++){
        for(int j = 2;j <= sub_grid->cols-1;j++){

            //Calculate the structures near the cell that 
            //we are going yo evolve

            int alive = process_neighbors(i, j, prev);
            if(game_execution_rules(i, j, alive, prev, next) ==1 ){
                modified = 1;
            }
        }
    }
    return modified;
}

int process_neighbors(int i, int j, char** prev){
    int alive = (prev[i-1][j-1] == '1') + (prev[i-1][j] == '1') +  //top left + top
                        (prev[i-1][j+1] == '1') + (prev[i][j-1] == '1') +  //left right + left
                        (prev[i][j+1] == '1') + (prev[i+1][j-1] == '1') +  //right + bottom left
                        (prev[i+1][j] == '1') + (prev[i+1][j+1] == '1');   // bottom + bottom right
    return alive;
}

int compute_outer(SubGrid* sub_grid, char** prev, char** next){
    int modified = 0;
    
    //first row
    #pragma omp parallel for schedule(static)
    for(int j=1;j<=sub_grid->cols;++j){
        int i = 1;
        int alive = process_neighbors(i, j, prev);

        if(game_execution_rules(i , j, alive, prev, next) == 1){
            modified = 1;
        }
    }

    //first column
    #pragma omp parallel for schedule(static)
    for(int i=1; i<=sub_grid->rows;++i){
        int j = 1;
        int alive = process_neighbors(i, j, prev);
        if(game_execution_rules(i , j, alive, prev, next) == 1){
            modified = 1;
        }
    }

    //last row
    #pragma omp parallel for schedule(static)
    for(int j=1;j<=sub_grid->cols;++j){
        int i = sub_grid->rows;
        int alive = process_neighbors(i, j, prev);
        if(game_execution_rules(i , j, alive, prev, next) == 1){
            modified = 1;
        }
    }

    //last column
    #pragma omp parallel for schedule(static)
    for(int i=1;i<=sub_grid->rows;++i){
        int j = sub_grid->cols;
        int alive = process_neighbors(i, j, prev);
        if(game_execution_rules(i , j, alive, prev, next) == 1){
            modified = 1;
        }
    }
    return modified;
}

