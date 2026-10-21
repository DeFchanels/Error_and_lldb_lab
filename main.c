#include <stdio.h>
#include "funcs.h"


int main() {
    printf("Прямая рекурсия\n");
    int res = factorial(3);
    printf("Факториал %d: %d\n", 3, res);
    printf("Косвенная рекурсия\n");
    indirectA(3);
    printf("Бесконечная рекурсия\n");
    infinite(1);
    return 0;
}