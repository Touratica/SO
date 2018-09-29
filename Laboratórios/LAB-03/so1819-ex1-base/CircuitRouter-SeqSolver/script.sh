#!/bin/bash
if [ $# -eq 0 ] || [ -z "${1// }" ] ; then
	echo "No arguments supplied"
	exit 2
fi
myApp=$1
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