rm -r ./CSV/*

g++ main.cpp -o main.out #-fopenmp 
./main.out

python plot.py 0 0
