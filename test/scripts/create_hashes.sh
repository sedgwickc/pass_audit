#! /bin/bash


for word in $(cat words.txt)
do
	echo $word
	./bcrypt_hash $word >> hashes2.txt
done
