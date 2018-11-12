#!/bin/bash
if [ "$#" -ne 2 ]; then
    echo "Missing arguments"
	echo "Number of threads and input file required"
else
	(cd ./CircuitRouter-SeqSolver && make)
	echo "Executing single-threaded..."
	(cd ./CircuitRouter-SeqSolver && exec ./CircuitRouter-SeqSolver "../$2")
	
	tSeq=$(grep Elapsed "$2.res" | cut -f 7 -d ' ')

	echo "#threads,exec_time,speedup" > "$2.speedups.csv"
	echo "1S,$tSeq,1" >> "$2.speedups.csv"

	(cd ./CircuitRouter-ParSolver && make)
	for ((N=1; N<=$1; N++))
	do
		echo "Executing with $N thread(s)..."
		(cd ./CircuitRouter-ParSolver && exec ./CircuitRouter-ParSolver -t ${N} "../$2")
		
		tPar=$(grep Elapsed "$2.res" | cut -f 7 -d ' ')

		SpUp=$(echo "scale=6; $tSeq / $tPar" | bc)

		echo "$N,$tPar,$SpUp" >> "$2.speedups.csv"
	done
	(cd ./CircuitRouter-SeqSolver && make clean)

	(cd ./CircuitRouter-ParSolver && make clean)
fi