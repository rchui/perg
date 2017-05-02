all: clean gcc

clean:
	rm -rf perg

gcc:
	g++ -std=c++11 -fopenmp -O3 -o perg perg.cpp

icc: clean
	icc -std=c++11 -openmp -O3 -o perg perg.cpp
