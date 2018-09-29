#!/bin/bash
for input in inputs/*.txt
do
	echo ==== ${input} ====
	echo Line count: $(wc -l < "${input}")
	./CircuitRouter-SeqSolver < "${input}" | grep "Paths routed" 
done