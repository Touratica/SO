#!/bin/bash
myApp=CircuitRouter-SeqSolver
if [ ! -f ${myApp} ]; then
	echo "Program ${myApp} does not exit"
	exit 2
fi
for input in inputs/*.txt
do
	echo ==== ${input} ====
	echo Line count: $(wc -l < "${input}")
	./${myApp} < "${input}" | grep "Paths routed" 
done