#! /bin/bash


for word in $(cat $1)
do
	echo $word
	../bcrypt_hash $word >> hashes_$2.txt

done
