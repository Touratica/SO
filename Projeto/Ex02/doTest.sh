#!/bin/bash
if [ "$#" -ne 2 ]; then
    echo "Missing arguments"
	echo "Number of threads and input file required"
else
	(cd ./CircuitRouter-SeqSolver && make && exec ./CircuitRouter-SeqSolver "../$2")
	
	tSeq=$(grep Elapsed "$2.res" | cut -f 7 -d ' ')

	echo "#threads,exec_time,speedup" > "$2.speedups.csv"
	echo "1S,$tSeq,1" >> "$2.speedups.csv"

	for ((N=1; N<=$1; N++))
	do
		(cd ./CircuitRouter-ParSolver && make && exec ./CircuitRouter-ParSolver ${N} "../$2")
		
		tPar=$(grep Elapsed "$2.res" | cut -f 7 -d ' ')

		SpUp=$(echo "scale=6; $tPar / $tSeq" | bc)

		echo "$N,$tPar,$SpUp" >> "$2.speedups.csv"
	done
	(cd ./CircuitRouter-SeqSolver && make clean)

	(cd ./CircuitRouter-ParSolver && make clean)
fi