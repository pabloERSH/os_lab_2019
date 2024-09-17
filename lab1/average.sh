#!/bin/bash

count=0
sum=0

if [ -t 0 ]; then
  if [ "$#" -eq 0 ]; then
      echo "Необходимо передать хотя бы одно число."
      exit 1
  else
    for number in "$@"; do
        sum=$(echo "$sum + $number" | bc)
        count=$((count + 1))
    done
  fi
else
  while read -r number; do
      sum=$(echo "$sum + $number" | bc)
      count=$((count + 1))
  done
fi

average=$(echo "scale=2; $sum / $count" | bc)

echo "Количество аргументов: $count"
echo "Среднее арифметическое: $average"