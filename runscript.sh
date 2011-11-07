#!/bin/bash
#PBS -l nodes=1:ppn=8
#PBS -l walltime=00:05:00
#PBS -l pmem=20mb
#PBS -m abe
#PBS -M jeremy.wright@asu.edu

cd $PBS_O_WORKDIR
time ./build/buddyDriver 32
