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


./swe_2d 100 0.008 10 1.5
./swe_2d 100 0.008 10 1.5
./swe_2d 100 0.008 10 1.5
./swe_2d 400 0.002 10 1.5
./swe_2d 400 0.002 10 1.5
./swe_2d 400 0.002 10 1.5
./swe_2d 800 0.001 10 1.5
./swe_2d 800 0.001 10 1.5
./swe_2d 800 0.001 10 1.5

