#!/bin/bash

mkdir -p ${PWD}/build
cd ${PWD}/build

cmake ..
make 

./vmem