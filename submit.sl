#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=2
#SBATCH --time=00:00:59
#SBATCH --output=game_of_life.out
#SBATCH -A anakano_429
export OMP_NUM_THREADS=2

mkdir -p output

mpirun -bind-to none -n 4 ./bin/game_of_life -l 200 -n 1024 -i input/random/1024.txt -o output/1024.txt