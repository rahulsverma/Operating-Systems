

#!/bin/bash

make clean
make

sudo rmmod partF
sudo insmod partF.ko
sudo ./demo
