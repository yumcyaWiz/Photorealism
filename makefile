all:
	g++ -std=c++17 -Wall -fopenmp -g main.cpp

build:
	g++ -std=c++17 -Wall -fopenmp -O3 -mtune=native -march=native -mfpmath=both main.cpp

intel:
	icpc -std=c++14 -Wall -fast -qopenmp main.cpp

rtcamp:
	g++ -c -std=c++17 -Wall -fopenmp -O3 -mtune=native -march=native -mfpmath=both main.cpp
	g++ -c -std=c++17 -Wall -fopenmp -O3 -mtune=native -march=native -mfpmath=both tinypngout/TinyPngOut.cpp
	g++ -fopenmp main.o TinyPngOut.o -o main


