intel:
	icc -o perg -openmp perg.cpp

gcc:
	g++ -o perg -fopenmp perg.cpp
