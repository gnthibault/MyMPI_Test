# daintSkeleton

## Purpose of the project
Serves as a project skeleton

## Pre-requisite
If you want to enjoy all features of this small project builder, please consider installing the following packages (example for ubuntu):  
sudo apt-get install doxygen graphviz cppcheck libopenmpi-dev libboost-dev

## How to build for Piz Daint ?
cd daintSkeleton  
mkdir build; cd build  
mkdir $SCRATCH/MyProject  
source ../scripts/initBuild.sh  
CXX=CC CC=cc cmake -DBINDIR=$SCRATCH/MyProject -DUSE_CUDA=ON -DUSE_NVTX=ON -DCMAKE_BUILD_TYPE=Release -DTHRUST_BACKEND:STRING=CUDA ..  
make -j8 install  

## How to test
In the build directory, do:  
make test  

## How to run on Piz daint
in the directory  $SCRATCH/MyProject, do:  
sbatch ./launchApp1.sh

## How to run a profiling version
in the directory  $SCRATCH/MyProject, do:  
sbatch ./launchApp1WithProfiling.sh

## How to generate doxygen documentation
In the build directory, do:  
make doc  
The documentation should be build in the "doc" directory

## How to perform static code analysis with cppcheck
In the build directory, do:  
make cppcheck  
The cppcheck report can be found in the directory cppcheckdir-report

Test123
