PERG - Parallel Expression Reference Grep

Developed by Ryan Chui (2017)

perg is a custom multithreaded c++ implementation of grep to search multi gigabyte files, datasets,
and directories developed at the National Center for Supercomputing Applications.

Compile perg
------------

GNU C Compiler
g++ -o perg -fopenmp perg.cpp

Intel C Compiler
icc -o perg -openmp perg.cpp

Run perg
--------

Due to the nature of implementation the order of syntax is strict and must be adhered to or errors will
result.

The order flags must always be used in is [-r|-v|-V|-f].

    Usage:
    perg [-r|-v|-V|-f <file>] <search term>

    Modes:
    -r    Recursive Search.   Recursively searches through the directory and all sub directories for the 
                              given <search term>. Will not do anything if the [-f <file>] flag is given.

    -v    Search Inversion.   Search for every line that does not include the <search term>.

    -V    Enable Verbose.     The file path to the file will be printed along with the search result.

    -f    Single File Search. Signals perg to only search the <file> for the <search term>. If -f is not
                              used, perg will search the entire directory from where perg is called from.


