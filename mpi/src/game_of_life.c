#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// #include <mpi.h>


#include "../../utils/src/headers/args.h"

Arguments args;

int main(int argc, char** argv) {
    
    parseArguments(argc, argv, &args);
    printf("***** Arguments *****\n");
    printf("Iterations: %d\n", args.iterations);
    printf("Grid Size: %d\n", args.grid_size);
    printf("Input File Name: %s\n", args.input_file_name);
    printf("Output File Name: %s\n", args.output_file_name);
    printf("*********************\n");

    return 0;
}
