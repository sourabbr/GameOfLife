#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "headers/args.h"

void usage() {
    fprintf(stderr,
        "Usage: ./game_of_life -l <Number of Interations> -n <Grid size> -i <Input File Name> -o <Output File Name>\n"
    );
    exit(EXIT_FAILURE);
}

void parseArguments(int argc, char** argv, Arguments* args) {
    int opt;

    // Initialize args
    args->iterations = 0;
    args->grid_size = 0;
    args->input_file_name = NULL;
    args->output_file_name = NULL;

    // Loop over all of the options
    while ((opt = getopt(argc, argv, "l:n:i:o:")) != -1) {
        switch (opt) {
            case 'l': // Number of Iterations
                args->iterations = atoi(optarg);
                break;
            case 'n': // Grid Size
                args->grid_size = atoi(optarg);
                break;
            case 'i': // Input File Name
                args->input_file_name = optarg;
                break;
            case 'o': // Output File Name
                args->output_file_name = optarg;
                break;
            default: // Unknown option
                usage();
        }
    }

    // Check for required options and their arguments
    if (args->iterations <= 0 || args->grid_size <= 0 || args->input_file_name == NULL || args->output_file_name == NULL) {
        fprintf(stderr, "All options -l, -n, -i, and -o are required\n");
        usage();
    }
}