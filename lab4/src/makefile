CC=gcc
CFLAGS=-I.

# Сборка всех программ
all: parallel_min_max_timeout zombie_demonstration


# Параллельная версия  
parallel_min_max_timeout: parallel_min_max.c utils.o find_min_max.o
	$(CC) -o parallel_min_max_timeout parallel_min_max.c utils.o find_min_max.o $(CFLAGS)

# Зомби демонстрация
zombie_demonstration: zombie_demonstration.c
	$(CC) -o zombie_demonstration zombie_demonstration.c $(CFLAGS)

# Объектные файлы
utils.o: utils.c utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

# Очистка
clean:
	rm -f utils.o find_min_max.o zombie_demonstration parallel_min_max

# Объявление псевдо-целей
.PHONY: all clean.c $(CFLAGS)
