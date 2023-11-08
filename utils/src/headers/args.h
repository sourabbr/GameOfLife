#ifndef __ARGS__
#define __ARGS__

typedef struct CommandLineArguments {
    int iterations;
    int grid_size;
    char * input_file_name;
    char * output_file_name;
} Arguments;

void usage();
void parseArguments(int argc, char** argv, Arguments* args);
void printArguments(Arguments* args);

#endif // __ARGS__