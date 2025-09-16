#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 number1 number2 ... numberN"
    exit 1
fi

count=$#
sum=0

for num in "$@"; do
    sum=$((sum + num))
done

average=$((sum / count))

echo "Количество чисел: $count"
echo "Сумма чисел: $sum"
echo "Среднее арифметическое: $average"
