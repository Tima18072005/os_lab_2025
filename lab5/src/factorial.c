#include "factorial.h"
#include <pthread.h>

// Структура для передачи данных в поток
typedef struct {
    int start;
    int end;
    long long mod;
    long long partial_result;
} ThreadData;

// Глобальные переменные для синхронизации
long long total_result = 1;
pthread_mutex_t mutex;

// Функция потока
void* calculate_part(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->partial_result = 1;
    
    // Вычисляем свою часть
    for (int i = data->start; i <= data->end; i++) {
        data->partial_result = (data->partial_result * i) % data->mod;
    }
    
    // Синхронизируем запись в общий результат
    pthread_mutex_lock(&mutex);
    total_result = (total_result * data->partial_result) % data->mod;
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

long long factorial_mod_parallel(int k, int pnum, long long mod) {
    if (k < 0) return 0;
    if (k == 0 || k == 1) return 1 % mod;
    
    total_result = 1;
    pthread_mutex_init(&mutex, NULL);
    
    pthread_t threads[pnum];
    ThreadData thread_data[pnum];
    
    // Распределяем числа по потокам
    int numbers_per_thread = k / pnum;
    int extra_numbers = k % pnum;
    int current = 1;
    
    for (int i = 0; i < pnum; i++) {
        int end = current + numbers_per_thread - 1;
        if (extra_numbers > 0) {
            end++;
            extra_numbers--;
        }
        
        thread_data[i].start = current;
        thread_data[i].end = end;
        thread_data[i].mod = mod;
        
        pthread_create(&threads[i], NULL, calculate_part, &thread_data[i]);
        current = end + 1;
    }
    
    // Ждем завершения всех потоков
    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    return total_result;
}
