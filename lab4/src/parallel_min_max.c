#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <getopt.h>
#include <signal.h>

#include "find_min_max.h"
#include "utils.h"

// Глобальная переменная для хранения PID дочерних процессов
pid_t *child_pids = NULL;
int child_count = 0;

// Обработчик сигнала таймаута
void timeout_handler(int sig) {
    printf("Timeout reached! Sending SIGKILL to all child processes.\n");
    for (int i = 0; i < child_count; i++) {
        if (child_pids[i] > 0) {
            kill(child_pids[i], SIGKILL);
        }
    }
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout = -1; // Таймаут в секундах
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"timeout", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
                printf("seed must be a positive number\n");
                return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) {
                printf("array_size must be a positive number\n");
                return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0) {
                printf("pnum must be a positive number\n");
                return 1;
            }
            break;
          case 3:
            timeout = atoi(optarg);
            if (timeout <= 0) {
                printf("timeout must be a positive number\n");
                return 1;
            }
            break;
          case 4:
            with_files = true;
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"seconds\"]\n",
           argv[0]);
    return 1;
  }

  // Выделяем память для хранения PID дочерних процессов
  child_pids = malloc(sizeof(pid_t) * pnum);
  if (child_pids == NULL) {
    printf("Memory allocation failed for child_pids\n");
    return 1;
  }
  child_count = pnum;

  // Настраиваем обработчик сигнала таймаута
  if (timeout > 0) {
    struct sigaction sa;
    sa.sa_handler = timeout_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
      perror("sigaction");
      free(child_pids);
      return 1;
    }
    
    // Устанавливаем таймер
    alarm(timeout);
    printf("Timeout set to %d seconds\n", timeout);
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  
  // Создаем pipes для коммуникации (если не используем файлы)
  int pipes[pnum][2];
  if (!with_files) {
    for (int i = 0; i < pnum; i++) {
      if (pipe(pipes[i]) == -1) {
        printf("Pipe creation failed!\n");
        free(array);
        free(child_pids);
        return 1;
      }
    }
  }

  int active_child_processes = 0;
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  // Создаем дочерние процессы
  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    child_pids[i] = child_pid; // Сохраняем PID дочернего процесса
    
    if (child_pid >= 0) {
      active_child_processes += 1;
      if (child_pid == 0) {
        // Дочерний процесс
        int start = i * (array_size / pnum);
        int end = (i == pnum - 1) ? array_size : (i + 1) * (array_size / pnum);
        
        struct MinMax local_min_max = GetMinMax(array, start, end);
        
        if (with_files) {
          // Используем файлы для коммуникации
          char filename[30];
          sprintf(filename, "min_max_%d.txt", i);
          FILE *file = fopen(filename, "w");
          if (file != NULL) {
            fprintf(file, "%d %d", local_min_max.min, local_min_max.max);
            fclose(file);
          }
        } else {
          // Используем pipe для коммуникации
          close(pipes[i][0]); // закрываем чтение в дочернем процессе
          write(pipes[i][1], &local_min_max.min, sizeof(int));
          write(pipes[i][1], &local_min_max.max, sizeof(int));
          close(pipes[i][1]);
        }
        free(array);
        exit(0);
      }
    } else {
      printf("Fork failed!\n");
      free(array);
      free(child_pids);
      return 1;
    }
  }

  // Родительский процесс ожидает завершения всех дочерних
  int status;
  pid_t pid;
  int completed_children = 0;
  int killed_children = 0;
  
  while (active_child_processes > 0) {
    pid = wait(&status);
    
    if (pid > 0) {
      active_child_processes -= 1;
      completed_children++;
      
      // Проверяем, был ли процесс убит по таймауту
      if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL) {
        killed_children++;
        printf("Child process %d was killed by timeout\n", pid);
      }
    }
  }

  // Отменяем таймер, если все процессы завершились до таймаута
  if (timeout > 0) {
    alarm(0);
  }

  // Собираем результаты только от процессов, которые завершились нормально
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  int successful_processes = 0;
  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;
    bool valid_result = false;

    if (with_files) {
      // Чтение из файлов
      char filename[30];
      sprintf(filename, "min_max_%d.txt", i);
      FILE *file = fopen(filename, "r");
      if (file != NULL) {
        if (fscanf(file, "%d %d", &min, &max) == 2) {
          valid_result = true;
        }
        fclose(file);
        remove(filename); // удаляем временный файл
      }
    } else {
      // Чтение из pipes
      close(pipes[i][1]); // закрываем запись в родительском процессе
      
      // Пытаемся прочитать данные, но они могут быть недоступны если процесс был убит
      ssize_t bytes_read_min = read(pipes[i][0], &min, sizeof(int));
      ssize_t bytes_read_max = read(pipes[i][0], &max, sizeof(int));
      
      if (bytes_read_min == sizeof(int) && bytes_read_max == sizeof(int)) {
        valid_result = true;
      }
      close(pipes[i][0]);
    }

    if (valid_result) {
      if (min < min_max.min) min_max.min = min;
      if (max > min_max.max) min_max.max = max;
      successful_processes++;
    }
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  free(child_pids);

  // Выводим статистику
  printf("\nResults:\n");
  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  printf("Successful processes: %d/%d\n", successful_processes, pnum);
  if (killed_children > 0) {
    printf("Processes killed by timeout: %d\n", killed_children);
  }
  
  fflush(NULL);
  
  // Если ни один процесс не завершился успешно, возвращаем ошибку
  if (successful_processes == 0) {
    printf("Error: No processes completed successfully\n");
    return 1;
  }
  
  return 0;
} 


