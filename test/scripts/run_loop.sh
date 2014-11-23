#! /bin/bash
# Charles Sedgwick
# 1684807

NUM=$2
T=$1

echo
echo "Running pw_crack $NUM times with $T threads"
echo "---------------------------------------------"
for num in $( seq $NUM ) 
do
	./pw_crack_debug /usr/share/cracklib/cracklib-small hashes.txt
done
