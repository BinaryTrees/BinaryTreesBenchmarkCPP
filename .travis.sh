set -e
cd ./source
echo "Testing this implementation"
export CC=gcc-10 && export CXX=g++-10
$CXX --version
$CXX -DNDEBUG -fno-exceptions -fno-rtti -fopenmp -g0 -march=native -O3 -std=c++17 -o ./binarytrees ./binarytrees.cpp ;
time ./binarytrees 21
rm -rf ./binarytrees
export CC=clang-10 && export CXX=clang++-10
$CXX --version
$CXX -DNDEBUG -fno-exceptions -fno-rtti -fopenmp=libgomp -g0 -march=native -O3 -std=c++17 -o ./binarytrees ./binarytrees.cpp ;
time ./binarytrees 21
rm -rf ./binarytrees
echo "Testing the BenchmarksGame implementation"
export CC=gcc-10 && export CXX=g++-10
$CXX --version
$CXX -DNDEBUG -fno-exceptions -fno-rtti -fopenmp -g0 -march=native -O3 -std=c++17 -o ./binarytrees_benchmarksgame ./binarytrees_benchmarksgame.cpp ;
time ./binarytrees_benchmarksgame 21
rm -rf ./binarytrees_benchmarksgame
export CC=clang-10 && export CXX=clang++-10
$CXX --version
$CXX -DNDEBUG -fno-exceptions -fno-rtti -fopenmp=libgomp -g0 -march=native -O3 -std=c++17 -o ./binarytrees_benchmarksgame ./binarytrees_benchmarksgame.cpp ;
time ./binarytrees_benchmarksgame 21
rm -rf ./binarytrees_benchmarksgame
