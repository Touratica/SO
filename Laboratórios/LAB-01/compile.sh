gcc -c coordinate.c -o coordinate.o
gcc -c grid.c -o grid.o
gcc -c CircuitRouter-SeqSolver.c -o CircuitRouter-SeqSolver.o
gcc -c maze.c -o maze.o
gcc -c router.c -o router.o

gcc -c lib/list.c -o lib/list.o
gcc -c lib/pair.c -o lib/pair.o
gcc -c lib/queue.c -o lib/queue.o
gcc -c lib/vector.c -o lib/vector.o
gcc coordinate.o grid.o CircuitRouter-SeqSolver.o maze.o router.o lib/list.o lib/pair.o lib/queue.o lib/vector.o -lm -o CircuitRouter-SeqSolver
./CircuitRouter-SeqSolver < inputs/random-x64-y64-z3-n64.txt
