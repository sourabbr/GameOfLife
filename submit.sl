#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=2
#SBATCH --time=00:00:59
#SBATCH --output=game_of_life.out
#SBATCH -A anakano_429


mkdir -p output

mpirun -bind-to none -n $SLURM_NTASKS ./bin/game_of_life -l 10 -n 4 -i input/16.txt -o output/out.txt