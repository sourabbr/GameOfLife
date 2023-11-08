#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// #include <mpi.h>


#include "../common/args.h"

Arguments args;

int main(int argc, char** argv) {
    
    parseArguments(argc, argv, &args);
    printArguments(&args);

    return 0;
}
