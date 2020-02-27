set -e
cd ./source
if [ "$CC" = "gcc" ]; then g++ -fopenmp -march=native -O3 -std=gnu++11 -o ./binarytrees_benchmark ./binarytrees.cpp
if [ "$CC" = "clang" ]; then clang++ -fopenmp=libgomp -march=native -O3 -std=gnu++11 -o ./binarytrees_benchmark ./binarytrees.cpp
time ./binarytrees_benchmark 21
