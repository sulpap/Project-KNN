#!/bin/bash

./bin/calculate_groundtruth 20
./bin/main_load 20 filtered_graph_L=40_a=1.200000_R=80_taph=2 ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=20.bin