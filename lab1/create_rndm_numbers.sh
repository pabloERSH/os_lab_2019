#!/bin/bash
count=$1
> numbers.txt

for i in $(seq 1 $count); do
  random_number=$(od -An -N2 -i /dev/urandom | tr -d ' ')
  echo $random_number >> numbers.txt
done