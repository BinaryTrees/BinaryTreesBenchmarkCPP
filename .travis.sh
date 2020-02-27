set -e
cd ./source
if [ "$CC" = "gcc" ]; then g++ -fopenmp -march=native -O3 -std=gnu++11 -o ./binarytrees_benchmark ./binarytrees.cpp ; fi
if [ "$CC" = "clang" ]; then clang++ -fopenmp -march=native -O3 -std=gnu++11 -o ./binarytrees_benchmark ./binarytrees.cpp ; fi
time ./binarytrees_benchmark 21
