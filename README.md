# GameOfLife

## Compilation of MPI and OpenMP source code
In the project directory, run the following command:
```
./build.sh compile
```

The ***build.sh*** script compiles the source code using the following command:
```
mpicc -O -o $OUTPUT_BINARY $SOURCE_FILES -lm -fopenmp
```
The enviroment variables ***OUTPUT_BINARY*** and ***SOURCE_FILES*** are defined in the ***build.sh*** script. The executable named ***game_of_life*** will be created in the bin folder.

The binary and output files can be removed by running:
```
./build.sh clean
```

## Submit job to HPC cluster using SLURM
The ***submit.sl*** file contains the SLURM script to submit the job to the HPC cluster. The script can be modified to change the number of nodes, number of tasks per node, number of threads per task, etc. It uses the following command to run the simulation:
```
mpirun -bind-to none -n no_of_procs ./bin/game_of_life -l number_of_iterations -n grid_size -i input_file_name -o output_file_name
```
The script can be submitted to the cluster using the following command:
```
sbatch submit.sl
```
Post completion of the job, the output file will be created, and the program output will be written in ***game_of_life.out***.

