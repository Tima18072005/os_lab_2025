#include <stdio.h>
#include <stdlib.h>
#include "factorial.h"

int main() {
    int k, pnum;
    long long mod;
    
    // Ввод данных от пользователя
    printf("Enter k (number for factorial): ");
    scanf("%d", &k);
    
    printf("Enter pnum (number of threads): ");
    scanf("%d", &pnum);
    
    printf("Enter mod (modulus): ");
    scanf("%lld", &mod);
    
    // Проверка входных данных
    if (k < 0) {
        printf("Error: k cannot be negative\n");
        return 1;
    }
    
    if (pnum <= 0) {
        printf("Error: pnum must be positive\n");
        return 1;
    }
    
    if (mod <= 0) {
        printf("Error: mod must be positive\n");
        return 1;
    }
    
    // Вычисление
    printf("\nCalculating %d! mod %lld using %d threads...\n", k, mod, pnum);
    
    long long result = factorial_mod_parallel(k, pnum, mod);
    
    printf("Result: %lld\n", result);
    
    return 0;
}
