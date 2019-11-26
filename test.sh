rm -r ./CSV/*

g++ main.cpp -o main.out -fopenmp 
./main.out

# g++ seq.cpp -o seq.out
# ./seq.out

python plot.py 0 0
