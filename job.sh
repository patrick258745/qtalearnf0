#!/bin/bash

#Submit this script with: sbatch thefilename

# load modules
module load gcc/6.2.0
module load gnuplot/4.6.1

#SBATCH -p broadwell
#SBATCH --time=8:00:00   # walltime
#SBATCH --nodes=1   # number of nodes
#SBATCH --tasks-per-node=1
#SBATCH --cpus-per-task=28
#SBATCH --mem-per-cpu=300M   # memory per CPU core
#SBATCH --mail-user=patrick.schmager@tu-dresden.de   # email address
#SBATCH --mail-type=BEGIN,END,FAIL,REQUEUE,TIME_LIMIT,TIME_LIMIT_90

# Set the max number of threads to use for programs using OpenMP. Should be <= ppn. Does nothing if the program doesn't use OpenMP.
export OMP_NUM_THREADS=$SLURM_CPUS_ON_NODE
OUTFILE=""

/scratch/s5517125/qtalearnf0/learn/learn.sh
