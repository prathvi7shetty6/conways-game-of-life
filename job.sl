#!/bin/bash
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=2
#SBATCH --time=00:00:59
#SBATCH --output=conways_game.out
#SBATCH -A anakano_429
export OMP_NUM_THREADS=2

mkdir -p output

mpirun -bind-to none -n 4 ./bin/conways_game -g 200 -n 16 -i input/random/16.txt -o output/16.txt
