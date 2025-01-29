#!/bin/bash
###
###
#SBATCH --time=01:00:00
#SBATCH -N 1
#SBATCH -n 1
#SBATCH --partition=bsudfq
#SBATCH --job-name=swe_2d
#SBATCH --output=swe_2d.o%j

#module load gcc mpich slurm
#make swe_2d

./swe_2d 1600 0.0005 10 1.5

