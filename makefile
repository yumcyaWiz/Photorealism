all:
	g++ -std=c++17 -Wall -fopenmp -g main.cpp

build:
	g++ -std=c++17 -Wall -fopenmp -O3 -msse2 -mtune=native -march=native -mfpmath=both main.cpp
