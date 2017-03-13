# PERG - Parallel Exploritory Recursive Grep

##Motivation
The grep command line utility was first introduced in 1974 as a method of searching files and directories using regular expressions. With the penetration of big data into every facet of the computing space datasets have grown exponentially in size. The original single-threaded grep implementation is too slow to effectively parse and search multi-gigabyte files and directories. We introduce perg which is a C++ multi-threaded implementation of grep developed at the National Center fo Supercomputing Applciations.

##Requirements
Need at least GNU C Compiler (4.9) with OpenMP.

##Installation
###GNU C Compiler
    g++ -std=c++11 -o perg -fopenmp perg.cpp

###Makefile
    make

##Usage
###Command Syntax:
    perg [-f <file> | -r | -v | -V | -w] <search term>

###Modes:
    -f    Single File Search    Signals perg to only search the <file> for the <search term>. If -f is not
                                used, perg will search the entire directory from where perg is called from.
                                
    -r    Recursive Search      Recursively searches through the directory and all sub directories for the 
                                given <search term>. Will not do anything if the [-f <file>] flag is given.

    -v    Search Inversion      Search for every line that does not include the <search term>.

    -V    Enable Verbose        The file path to the file will be printed along with the search result.
    
    -w    File Parallelism      Signals perg to perform single-threaded searches of multiple files. Default
                                search behavior is to search files one at a time with mulitple threads.
                                This is optimal when the files are small, similar size, or there are many.
