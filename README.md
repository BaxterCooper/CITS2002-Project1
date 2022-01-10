# CITS2002-Project1
A command line utility program written in C that emulates the execution of a small C like programming language on a stack-based machine with an implemented write-back cache. The program reads opcode instructions of the program to be executed and returns a reports of the number of main-memory reads and writes and the number of cache hits and misses.

## Compilation
```bash
cc -std=c11 -Wall -Werror -o runcool runcool.c
```

## Usage
```bash
./runcool filepath-of-program
```


