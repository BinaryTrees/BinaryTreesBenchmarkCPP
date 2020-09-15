set -e
cd ./source
export CC=gcc-10 && export CXX=g++-10
echo $CXX --version
$CXX -DNDEBUG -fno-exceptions -fno-rtti -fopenmp -march=native -O3 -std=c++14 -o ./binarytrees_benchmark ./binarytrees.cpp ;
time ./binarytrees_benchmark 21
rm -rf ./binarytrees_benchmark
export CC=clang-10 && export CXX=clang++-10
echo $CXX --version
$CXX -DNDEBUG -fno-exceptions -fno-rtti -fopenmp=libgomp -march=native -O3 -std=c++14 -o ./binarytrees_benchmark ./binarytrees.cpp ;
time ./binarytrees_benchmark 21
rm -rf ./binarytrees_benchmark
