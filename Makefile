all: clean gcc

clean:
	rm -rf perg

gcc:
	g++ -std=c++11 -O3 -o perg -fopenmp perg.cpp

icc: clean
	icc -std=c++11 -openmp -O3 -o perg perg.cpp
