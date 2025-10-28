#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pthread.h>

#include "sum.h"
#include "utils5.h"

// Функция для парсинга аргументов командной строки
int parse_arguments(int argc, char **argv, uint32_t *threads_num, uint32_t *seed, uint32_t *array_size) {
    if (argc != 7) {
        printf("Usage: %s --threads_num <num> --seed <num> --array_size <num>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "--threads_num") == 0) {
            *threads_num = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "--seed") == 0) {
            *seed = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "--array_size") == 0) {
            *array_size = atoi(argv[i + 1]);
        } else {
            printf("Unknown argument: %s\n", argv[i]);
            return 1;
        }
    }

    if (*threads_num <= 0 || *seed <= 0 || *array_size <= 0) {
        printf("All arguments must be positive numbers\n");
        return 1;
    }

    return 0;
}

void *ThreadSum(void *args) {
    struct SumArgs *sum_args = (struct SumArgs *)args;
    return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
    uint32_t threads_num = 0;
    uint32_t array_size = 0;
    uint32_t seed = 0;

    // Парсим аргументы командной строки
    if (parse_arguments(argc, argv, &threads_num, &seed, &array_size) != 0) {
        return 1;
    }

    printf("Threads: %u, Seed: %u, Array Size: %u\n", threads_num, seed, array_size);

    // Выделяем память под массив
    int *array = malloc(sizeof(int) * array_size);
    if (array == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    // Генерируем массив (не входит в замер времени)
    GenerateArray(array, array_size, seed);

    // Создаем массив потоков и аргументов
    pthread_t threads[threads_num];
    struct SumArgs args[threads_num];

    // Вычисляем размер части массива для каждого потока
    int chunk_size = array_size / threads_num;

    // Начинаем замер времени
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Создаем потоки
    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * chunk_size;
        args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * chunk_size;

        if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i]) != 0) {
            printf("Error: pthread_create failed!\n");
            free(array);
            return 1;
        }
    }

    // Собираем результаты
    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) {
        int sum = 0;
        pthread_join(threads[i], (void **)&sum);
        total_sum += sum;
    }

    // Завершаем замер времени
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    // Освобождаем память и выводим результаты
    free(array);
    printf("Total sum: %d\n", total_sum);
    printf("Time taken: %.6f seconds\n", time_taken);

    return 0;
} 
