set -e
cd ./source
if [ "$CC" = "gcc" ]; then 
  g++ -DNDEBUG -fno-exceptions -fno-rtti -fopenmp -march=native -O3 -std=gnu++11 -o ./binarytrees_benchmark ./binarytrees.cpp ;
fi
if [ "$CC" = "clang" ]; then
  clang++ -DNDEBUG -fno-exceptions -fno-rtti -fopenmp=libgomp -march=native -O3 -std=gnu++11 -o ./binarytrees_benchmark ./binarytrees.cpp ;
fi
time ./binarytrees_benchmark 21
