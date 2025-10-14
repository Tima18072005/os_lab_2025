#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <seed> <array_size>\n", argv[0]);
        printf("Example: %s 42 1000000\n", argv[0]);
        return 1;
    }

    printf("Родительский процесс (PID: %d) запущен\n", getpid());
    
    pid_t pid = fork();
    
    if (pid < 0) {
        // Ошибка создания процесса
        printf("Fork failed!\n");
        return 1;
    }
    else if (pid == 0) {
        // Дочерний процесс - запускаем sequential_min_max
        printf("Дочерний процесс (PID: %d) запускает sequential_min_max...\n", getpid());
        
        // Заменяем текущую программу на sequential_min_max
        execl("./sequential_min_max", "sequential_min_max", argv[1], argv[2], NULL);
        
        // Если дошли сюда - exec не сработал
        printf("Exec failed! sequential_min_max не найден\n");
        exit(1);
    }
    else {
        // Родительский процесс - ждем завершения дочернего
        printf("Родительский процесс создал дочерний с PID: %d\n", pid);
        
        int status;
        waitpid(pid, &status, 0);  // ждем завершения дочернего процесса
        
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("Дочерний процесс завершился с кодом: %d\n", exit_code);
            
            if (exit_code == 0) {
                printf("sequential_min_max выполнен успешно!\n");
            } else {
                printf("sequential_min_max завершился с ошибкой!\n");
            }
        }
        else {
            printf("Дочерний процесс завершился аварийно\n");
        }
        
        printf("Родительский процесс завершает работу\n");
    }
    
    return 0;
}
