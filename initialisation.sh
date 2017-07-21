#!/bin/bash
rm -rf build
rm -rf data_out
mkdir build
mkdir data_out
cd data_out
mkdir imgPO2 imgState imgTimer imgCycle
cd ../build
ccmake ..
cmake .
make
cd ..
printf "%b\n" '\n\n\t-- Lancement de la simulation --\n\n'
./simulation.sh
