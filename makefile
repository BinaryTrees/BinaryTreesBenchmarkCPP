ifeq ($(CXX),)
	$(error The CXX environment variable must be set!)
endif

ifeq ($(OS), Windows_NT)
	EXT = .exe
else
	EXT =
endif

build:
	$(CXX) -DNDEBUG -fno-exceptions -fno-rtti -fopenmp -march=native -O3 -std=c++14 -o ./binarytrees$(EXT) ./source/binarytrees.cpp
