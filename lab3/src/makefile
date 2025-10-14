CC=gcc
CFLAGS=-I.

# Сборка всех программ
all: sequential_min_max parallel_min_max exec_sequential

# Последовательная версия
sequential_min_max: sequential_min_max.c utils.o find_min_max.o
	$(CC) -o sequential_min_max sequential_min_max.c utils.o find_min_max.o $(CFLAGS)

# Параллельная версия  
parallel_min_max: parallel_min_max.c utils.o find_min_max.o
	$(CC) -o parallel_min_max parallel_min_max.c utils.o find_min_max.o $(CFLAGS)

# Запуск поиска минимума/максимума в  отдельном процессе
exec_sequential: exec_sequential.c
	$(CC) -o exec_sequential exec_sequential.c $(CFLAGS)

# Объектные файлы
utils.o: utils.c utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

# Очистка
clean:
	rm -f utils.o find_min_max.o sequential_min_max parallel_min_max

# Объявление псевдо-целей
.PHONY: all clean

