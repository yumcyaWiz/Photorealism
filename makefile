all:
	g++ -std=c++17 -Wall -fopenmp -g main.cpp

build:
	g++ -std=c++17 -Wall -fopenmp -O3 -mtune=native -march=native -mfpmath=both main.cpp

intel:
	icpc -std=c++14 -Wall -fast -qopenmp main.cpp

