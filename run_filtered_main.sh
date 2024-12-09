#!/bin/bash

./bin/calculate_groundtruth 20
./bin/filtered_main 20 40 80 1.2 2 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=20.bin