all: clean gcc

clean:
	rm -rf perg

gcc:
	g++ -std=c++11 -o perg -fopenmp perg.cpp
