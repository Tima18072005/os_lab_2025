#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void create_zombie() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // Дочерний процесс
        printf("Дочерний процесс (PID: %d) запущен\n", getpid());
        printf("Дочерний процесс (PID: %d) завершается, становясь зомби\n", getpid());
        exit(0); // Завершаем дочерний процесс
    } else if (pid > 0) {
        // Родительский процесс
        printf("Родительский процесс (PID: %d) создал дочерний (PID: %d)\n", getpid(), pid);
        printf("Родительский процесс НЕ вызывает wait(), дочерний станет зомби\n");
        
        // Ждем 30 секунд, чтобы можно было увидеть зомби в системе
        sleep(30);
        
        // Теперь вызываем wait() чтобы убрать зомби
        printf("Родительский процесс вызывает wait() для очистки зомби\n");
        wait(NULL);
        printf("Зомби процесс очищен\n");
    } else {
        perror("Ошибка при создании процесса");
        exit(1);
    }
}

void prevent_zombie_with_wait() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // Дочерний процесс
        printf("Дочерний процесс (PID: %d) запущен\n", getpid());
        sleep(2);
        printf("Дочерний процесс (PID: %d) завершается\n", getpid());
        exit(0);
    } else if (pid > 0) {
        // Родительский процесс
        printf("Родительский процесс (PID: %d) создал дочерний (PID: %d)\n", getpid(), pid);
        printf("Родительский процесс ожидает завершения дочернего...\n");
        
        wait(NULL); // Ожидаем завершение дочернего процесса
        printf("Дочерний процесс корректно завершен, зомби не создан\n");
    } else {
        perror("Ошибка при создании процесса");
        exit(1);
    }
}

void prevent_zombie_with_signal() {
    // Устанавливаем обработчик для SIGCHLD
    signal(SIGCHLD, SIG_IGN); // Игнорируем SIGCHLD - система автоматически убирает зомби
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Дочерний процесс
        printf("Дочерний процесс (PID: %d) запущен\n", getpid());
        sleep(2);
        printf("Дочерний процесс (PID: %d) завершается\n", getpid());
        exit(0);
    } else if (pid > 0) {
        // Родительский процесс
        printf("Родительский процесс (PID: %d) создал дочерний (PID: %d)\n", getpid(), pid);
        printf("SIGCHLD игнорируется - система автоматически уберет зомби\n");
        
        sleep(4); // Даем время для завершения дочернего процесса
        printf("Дочерний процесс завершен, зомби автоматически очищен системой\n");
    } else {
        perror("Ошибка при создании процесса");
        exit(1);
    }
}

int main() {
    printf("=== Демонстрация зомби-процессов ===\n\n");
    
    int choice;
    printf("Выберите вариант:\n");
    printf("1 - Создать зомби-процесс\n");
    printf("2 - Предотвратить зомби с помощью wait()\n");
    printf("3 - Предотвратить зомби с помощью сигнала\n");
    printf("Ваш выбор: ");
    scanf("%d", &choice);
    
    switch(choice) {
        case 1:
            create_zombie();
            break;
        case 2:
            prevent_zombie_with_wait();
            break;
        case 3:
            prevent_zombie_with_signal();
            break;
        default:
            printf("Неверный выбор\n");
            return 1;
    }
    
    return 0;
} 
