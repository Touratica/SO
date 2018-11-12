# Readme

This folder contains source code and scripts necessary to compile and run Circuit Router's Sequential Solver, Parallel Solver and Simple Shell. It also has a folder with input files to be run. Everytime the executables calculate a route for a circuit from an input file, the result will be saved on the same folder.

## CircuitRouter-ParSolver

Contains source code and Makefile to compile the parallel version of the Circuit Router. To compile and run, for example, with a file from the Inputs folder and with 4 threads, do as follows from the project's root folder:

```bash
cd CircuitRouter-ParSolver/
make
./CircuitRouter-ParSolver -t 4 ../Inputs/random-x32-y32-z3-n64.txt
```

To delete the binary files and executables, do:

```bash
cd CircuitRouter-ParSolver/
make clean
```

## CircuitRouter-SeqSolver

Contains source code and Makefile to compile the sequential version of the Circuit Router. To compile and run, for example, with a file from the Inputs folder, do as follows from the project's root folder:

```bash
cd CircuitRouter-SeqSolver/
make
./CircuitRouter-SeqSolver ../inputs/random-x32-y32-z3-n64.txt
```

To delete the binary files and executables, do:

```bash
cd CircuitRouter-SeqSolver/
make clean
```

## CircuitRouter-SimpleShell

Contains source code and Makefile to compile both the sequential version and the Simple Shell of the Circuit Router. To compile and run, for example, with a maximum number of 4 child processes, do as follows from the project's root folder:

```bash
cd CircuitRouter-SimpleShell/
make
./CircuitRouter-SimpleShell 4
```

Within the Simple shell, to run the sequential version with a file from the Inputs folder and exit the shell, just do:

```
run ../inputs/random-x32-y32-z3-n64.txt
exit
```

To delete the binary files and executables, do:

```bash
cd CircuitRouter-SeqSolver/
make clean
```

## Testing

To test both the sequential and the parallel versions of the Circuit Router, run the [doTest.sh](doTest.sh) script, specifying the maximum number of threads the parallel version will run and the input file. This will compile both versions, create a file within the same folder as the input file with the results of the test and then delete de executables and binary files.

```bash
./doTest.sh 4 inputs/random-x32-y32-z3-n64.txt
```

## Results

The results folder has the results of all the input files with a circuit with a size equal or inferior to 256x256, that were run in a computer with the following specifications:

* 8 cores (using hyper-threading);
* Linux UBUNTU-U9625J0I 4.18.0-10-generic #11-Ubuntu SMP Thu Oct 11 15:13:55 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux
