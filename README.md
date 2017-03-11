# PERG - Parallel Expression Reference Grep

## RECURSIVE FUNCTION CURRENTLY BROKEN

##Motivation
The grep command line utility was first introduced in 1974 as a method of searching files and directories using regular expressions. With the penetration of big data into every facet of the computing space datasets have grown exponentially in size. The original single-threaded grep implementation is too slow to effectively parse and search multi-gigabyte file sizes. We introduce perg which is a C++ multi-threaded implementation of grep developed at the National Center fo Supercomputing Applciations.

##Installation
###GNU C Compiler
    g++ -std=c++11 -o perg -fopenmp perg.cpp

##Usage
###Command Syntax:
    perg [-r | -v | -V | -f <file>] <search term>

###Modes:
    -r    Recursive Search.     Recursively searches through the directory and all sub directories for the 
                                given <search term>. Will not do anything if the [-f <file>] flag is given.

    -v    Search Inversion.     Search for every line that does not include the <search term>.

    -V    Enable Verbose.       The file path to the file will be printed along with the search result.

    -f    Single File Search.   Signals perg to only search the <file> for the <search term>. If -f is not
                                used, perg will search the entire directory from where perg is called from.
