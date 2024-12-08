#!/bin/bash

./bin/calculate_groundtruth 20
./bin/filtered_main_load 20 40 filtered_graph_k=20L=40 ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=20.bin