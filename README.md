# GameOfLife



## Benchmarking
The simulation of Game of Life is run for 100 iterations for different grid sizes with *1, 2, 4, 8, 16, 32, 64* MPI processes with *2* OpenMP threads per MPI process. The simulation is run on the HPC cluster with the following resource requisition:
```
#SBATCH --nodes=8
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=2
```
For smaller grid sizes upto *8192x8192*, it was noticed that the serial program outperformed the parallel program. This is because the overhead of parallelization is more than the time taken to run the simulation serially. For larger grid sizes, the parallel program outperformed the serial program. The run times for each grid can be found in *output/runtime_output* folder. The following graphs show the run time, speedup and efficiency for the simulation of Game of Life for different grid sizes and number of MPI processes. 

### Parallel Run Time
Parallel run time is the time taken to run the simulation using MPI and OpenMP.


### Speedup
Speedup is the ratio of the serial run time to the parallel run time.


### Efficiency
Efficiency is the ratio of the speedup to the number of MPI processes.


## OpenGL Simulations
### Overview
In this documentation, videos of various patterns in Conway's Game of Life are included, all rendered using OpenGL for enhanced graphical representation. These include demonstrations of patterns using both predefined and random input files across different grid sizes. The focus is on the following primary pattern types:

#### Still Lifes
Stable patterns that do not change over generations, symbolizing equilibrium.
Examples: *Block, Beehive, Loaf, Boat*

#### Oscillators
Patterns that return to their initial state after a finite number of generations, depicting periodic behavior.
Examples: *Pulsar*

#### Spaceships
Patterns that move across the grid, maintaining their shape but changing location, demonstrating motion.
Examples: *Glider gun, MSM Breeder*

| Still Lifes | Oscillators | Spaceships |
|:-----------:|:-----------:|:----------:|
| ![idle](openGL/videos/idle.gif) | ![oscillator](openGL/videos/oscillator.gif) | ![glider-gun](openGL/videos/glider-gun.gif) |
| | | ![msm-breeder](openGL/videos/breeder1.gif) |

### Visualization of Patterns with Random Inputs

| Grid Size: 16x16 | Grid Size: 32x32 | Grid Size: 64x64 |
|:----------------:|:----------------:|:----------------:|
| ![16](openGL/videos/16.gif) | ![32](openGL/videos/32.gif) | ![64](openGL/videos/64.gif) |

| Grid Size: 128x128 | Grid Size: 256x256 |
|:------------------:|:------------------:|
| ![128](openGL/videos/128.gif) | ![256](openGL/videos/256.gif) |


## Compilation and Execution
### Compilation of MPI and OpenMP source code
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
### Submit job to HPC cluster using SLURM
The ***submit.sl*** file contains the SLURM script to submit the job to the HPC cluster. The script can be modified to change the number of nodes, number of tasks per node, number of threads per task, etc. It uses the following command to run the simulation:
```
mpirun -bind-to none -n no_of_procs ./bin/game_of_life -l number_of_iterations -n grid_size -i input_file_name -o output_file_name
```
The script can be submitted to the cluster using the following command:
```
sbatch submit.sl
```
Post completion of the job, the output file will be created, and the program output will be written in ***game_of_life.out***.


