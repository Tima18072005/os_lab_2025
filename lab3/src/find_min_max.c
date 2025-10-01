#include "find_min_max.h"
#include <limits.h>

struct MinMax {
    int min;
    int max;
};

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    // Проверка на пустой диапазон
    if (begin >= end) {
        return min_max;
    }
    
    // Инициализация как вы предложили
    min_max.min = array[begin];      // первый элемент как минимальный
    min_max.max = array[end - 1];    // последний элемент как максимальный
    
    unsigned int i = begin;          // индекс для поиска минимума (идёт слева направо)
    unsigned int j = end - 1;        // индекс для поиска максимума (идёт справа налево)
    
    // Цикл пока оба индекса не достигли своих границ
    while (i != end - 1 && j != begin) {
        // Поиск минимума
        if (array[i] < min_max.min) {
            min_max.min = array[i];
        }
        i++;  // двигаемся вправо
        
        // Поиск максимума
        if (array[j] > min_max.max) {
            min_max.max = array[j];
        }
        j--;  // двигаемся влево
    }
    
    return min_max;
}
