#!/usr/bin/env bash

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output

bin/PA1 testcases/bunny.txt output/bunny1.bmp 10
# bin/PA1 testcases/texture.txt output/texture1.bmp 
# # bin/PA1 testcases/depth.txt output/depth1.bmp 10
# bin/PA1 testcases/balls.txt output/balls1.bmp 10


bin/PA1 testcases/bunny.txt output/bunny.bmp 1000
# bin/PA1 testcases/texture.txt output/texture.bmp 2000
# bin/PA1 testcases/balls.txt output/balls.bmp 2000
# bin/PA1 testcases/depth.txt output/depth.bmp 2000