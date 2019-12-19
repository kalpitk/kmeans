#
# Author : Kalpit Kothari, Arjit Arora
#
# CS 359 Parallel Computing Project
#

rm -r ./CSV/
rm -r ./Dataset/

mkdir CSV
mkdir Dataset

# Get Dataset from respective sources
wget https://www.kaggle.com/hdriss/xclara/download -O Dataset/kg.csv
wget http://cs.joensuu.fi/sipu/datasets/s1.txt -O Dataset/birch100k.txt

g++ parallel.cpp -o parallel.out -fopenmp 
g++ sequential.cpp -o sequential.out

./parallel.out
./sequential.out

# python plot.py
