#ifndef SUM_H
#define SUM_H

struct SumArgs {
    int *array;
    int begin;
    int end;
};

// Функция для вычисления суммы части массива
int Sum(const struct SumArgs *args);

#endif
