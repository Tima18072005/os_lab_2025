#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

// Флаг для завершения программы
volatile int timeout = 0;

// Обработчик сигнала таймаута
void timeout_handler(int sig) {
    timeout = 1;
    printf("\n Timeout! Deadlock detected after 10 seconds.\n");
    printf("Forcing program termination...\n");
    exit(1);
}

// Поток 1: захватывает mutex1, потом пытается захватить mutex2
void* thread1_func(void* arg) {
    printf("Thread 1: Trying to lock mutex1...\n");
    pthread_mutex_lock(&mutex1);
    printf("Thread 1: Locked mutex1\n");
    
    // Имитация работы
    sleep(1);
    
    printf("Thread 1: Trying to lock mutex2...\n");
    pthread_mutex_lock(&mutex2);
    printf("Thread 1: Locked mutex2\n");
    
    // Критическая секция
    printf("Thread 1: Entering critical section\n");
    sleep(1);
    printf("Thread 1: Leaving critical section\n");
    
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    
    return NULL;
}

// Поток 2: захватывает mutex2, потом пытается захватить mutex1 (обратный порядок!)
void* thread2_func(void* arg) {
    printf("Thread 2: Trying to lock mutex2...\n");
    pthread_mutex_lock(&mutex2);
    printf("Thread 2: Locked mutex2\n");
    
    // Имитация работы
    sleep(1);
    
    printf("Thread 2: Trying to lock mutex1...\n");
    pthread_mutex_lock(&mutex1);
    printf("Thread 2: Locked mutex1\n");
    
    // Критическая секция
    printf("Thread 2: Entering critical section\n");
    sleep(1);
    printf("Thread 2: Leaving critical section\n");
    
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    
    printf(" Deadlock Demonstration Program \n");
    printf("This program will deadlock in 10 seconds...\n\n");
    
    // Устанавливаем таймер на 10 секунд
    signal(SIGALRM, timeout_handler);
    alarm(10);
    
    // Создаем потоки
    pthread_create(&thread1, NULL, thread1_func, NULL);
    pthread_create(&thread2, NULL, thread2_func, NULL);
    
    // Ждем завершения потоков (которого никогда не произойдет из-за deadlock)
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    printf("This line should never be printed!\n");
    
    return 0;
}
