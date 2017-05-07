all: clean gcc

clean:
	rm -rf bin/

gcc:
	mkdir bin
	g++ -std=c++11 -fopenmp -O3 -o bin/perg src/perg.cpp
	chmod -R 751 bin/

icc: clean
	mkdir bin
	icc -std=c++11 -openmp -O3 -o bin/perg src/perg.cpp
	chmod -R 751 bin/

install:
	cp bin/perg /usr/bin/perg
