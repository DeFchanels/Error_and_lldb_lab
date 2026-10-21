#include "funcs.h"
#include <stdio.h>


int factorial(int n) { // пример прямой рекурсии (вычисление факториала)
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// Косвенная рекурсия
void indirectA(int n) {
    if (n <= 0) return;
    printf("A: %d\n", n);
    indirectB(n - 1);
}

void indirectB(int n) {
    if (n <= 0) return;
    printf("B: %d\n", n);
    indirectA(n - 1);
}

// Бесконечная рекурсия
void infinite(int n) {
    int a = n;
    infinite(n + 1);
}
