# Ошибки на разных этапах
## 1. Препроцессор
### Ошибка №1
Использование несуществующего файла
```
# include <stdio.h>
# include "non_existing_file.h" // Ошибка препроцессора № 1 (подключение несуществующего файла)

int main() {
    return 0;
}
```
Ошибка:

```
gcc -E main.c
# 1 "main.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 484 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "main.c" 2
main.c:1:11: fatal error: 'fucs.h' file not found
    1 | # include "fucs.h" // Ошибка препроцессора № 1 (подключение несуществующего файла)
      |           ^~~~~~~~
                                                                                                                                                                         
# 1 "./funcs.h" 1
struct Point {
    int x;
    int y;
};
# 3 "main.c" 2

int main() {
    return 0;
}
1 error generated.
```
Исправление:
Для того чтобы исправить ошибку можно либо создать файл non_existing_file.h, либо удалить/заменить этот #include, если заголовочный файл не нужен.
### Ошибка №2
Незакрытая условная директива
В файле main.c:
```
#ifdef FUNC

int main() {
    return 0;
}

```
Ошибка:
```
gcc -E main.c
# 1 "main.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 484 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "main.c" 2
main.c:1:2: error: unterminated conditional directive
    1 | #ifdef FUNC
      |  ^
                                                                                                                                                                         
1 error generated.
```
Исправление: для того, чтобы исправить ошибку необходимо закрыть условную директиву
```
#ifdef FUNC

int main() {
    return 0;
}

#endif
```
Положительный результат (ошибка пропала):
```
gcc -E main.c
# 1 "main.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 484 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "main.c" 2
```
## 2. Компилятор
### Ошибка 1.
Отсутствие точки с запятой <br>

main.c:
```
int main() {
    int a = 1 + 2
    return 0;
}
```
Ошибка:
```
gcc -S main.c
2:18: error: expected ';' at end of declaration
    2 |     int a = 1 + 2
      |                  ^
      |                  ;        
1 error generated.      
```
Исправление: добавить ; в конец строки

### Ошибка 2.
Сложение несовместимых типов (например: int и char *) <br>
main.c:
```
int main() {
    int a = 1 + "text"
    return 0;
}
```
Ошибка:
```
gcc -S main.c
main.c:2:15: warning: adding 'int' to a string does not append to the string [-Wstring-plus-int]
    2 |     int a = 1 + "text";
      |             ~~^~~~~~~~
main.c:2:15: note: use array indexing to silence this warning                                                                    
main.c:2:9: error: incompatible pointer to integer conversion initializing 'int' with an expression of type 'char *' [-Wint-conversion]
    2 |     int a = 1 + "text";
      |         ^   ~~~~~~~~~~
1 warning and 1 error generated.  
```
## 3. Ассемблер
### Ошибка 1.
Несуществующая инструкция <br>

main.c:
```
int main() {
    // Намеренно пишем несуществующую инструкцию процессора
    __asm__("fake_instruction_xyz");
    return 0;
}
```
Ошибка:
```
gcc -c main.c
main.c:3:13: error: unrecognized instruction mnemonic
    3 |     __asm__("fake_instruction_xyz");
      |             ^
<inline asm>:1:2: note: instantiated into assembly here
    1 |         fake_instruction_xyz
      |         ^
1 error generated.     
```
Исправление: если необходимо, использовать существующие инструкции
## 4. Линковка
### Ошибка 1.
Использование функции без тела

main.c:
```
void function(); // Функция без тела

int main() {
    function(); // Вызываем эту функцию
    return 0;
}
```
gcc -S  main.c - ошибка при компиляции не возникает <br>

Ошибка:
```
gcc main.c    
Undefined symbols for architecture arm64:
  "_not_function", referenced from:
      _main in main-94fa70.o
ld: symbol(s) not found for architecture arm64
clang: error: linker command failed with exit code 1 (use -v to see invocation)
```
Исправление: заполнить тело функции
### Ошибка 2.
Использование двух версий одной и той же функции (не обязательно с разным содержимым)
Заполним файл funcs.h:
```
#pragma once
void function(void);
```
Создадим и заполним funcs.c:
```
#include <stdio.h>
#include "funcs.h"

// То же название функции, что и в main
void function(void) {
    printf("Res 1"); // То же тело что и в main.c
}
```
main.c:
```
#include <stdio.h>
#include "funcs.h"

// Определение функции function в файле main
void function() {
    printf("Res 1");
}

int main() {
    function();
    return 0;
}
```
gcc main.c funcs.c -S  - ошибка при компиляции не возникает <br>

Ошибка:
```
gcc main.c funcs.c              
duplicate symbol '_function' in:
    /private/var/folders/cr/w91f4jns6bz3x5qxwjw5tcgm0000gn/T/main-a65403.o
    /private/var/folders/cr/w91f4jns6bz3x5qxwjw5tcgm0000gn/T/funcs-839bfa.o
ld: 1 duplicate symbols
clang: error: linker command failed with exit code 1 (use -v to see invocation)
```
#### Интересный факт.
Во время попытки вызвать ошибку линковки с фалом main.c (вызов несуществующей функции):
```
int main(void) {
    not_function(); // Вызываем то, чего нет
    return 0;
}
```
Вызывалась не необходимая ошибка линковки, а ошибка компиляции
```
gcc main.c -S                   
main.c:2:5: error: call to undeclared function 'not_function'; ISO C99 and later do not support implicit function declarations
      [-Wimplicit-function-declaration]
    2 |     not_function(); // Вызываем то, чего нет
      |     ^
1 error generated.    
```
Это произошло из-за того, то я использую новый компилятор clang, который автоматически обрабатывает это и вызывает ошибку компиляции, которая в более ранних компиляторах бы не появилась, а вызвалась бы ошибка линковки


# Дебаггер
Запишем в файл main легкую программу для примера:
```
#include <stdio.h>

int sum(int a, int b) {
    int result = a + b;
    return result;
}

int main() {
    int x = 5;
    int y = 10;
    int res = sum(x, y);
    printf("Result: %d\n", res);
    return 0;
}
```
## Интересный факт
На системе macos дебаггер gdb изначально не установлен, но есть его аналог lldb, который я и буду использовать
<br><br>
### 1. gcc -g main.c -o a.out 
- компилируем код и сохраняем результат компиляции в a.out <br> <br>
   Чтобы дебаггер видел имена переменных и строки кода, компилируем программу с флагом -g
### 2. lldb ./a.out
- запусаем отладчик
### 3. list 
- Позволяет посмотреть код программы прямо в терминале <br> <br>
(main в данном случае означает что list просмотрит код функции main и ближайшие к ней строки, если в функции меньше 10 строк)
```
list main
File: /Users/georgy/Documents/C_projects/Errors_lab/main.c
   3    int sum(int a, int b) {
   4        int result = a + b;
   5        return result;
   6    }
   7    
   8    int main() {
   9        int x = 5;
   10       int y = 10;
   11       int res = sum(x, y);
   12       printf("Result: %d\n", res);
   13       return 0;
```
вот просмотр функции sum (и ближайших строк, если функция короче 10)
```
(lldb) list sum
File: /Users/georgy/Documents/C_projects/Errors_lab/main.c
   1    #include <stdio.h>
   2    
   3    int sum(int a, int b) {
   4        int result = a + b;
   5        return result;
   6    }
   7    
   8    int main() {
   9        int x = 5;
   10       int y = 10;
```
Можно также просмотреть строки, начиная с определенной:
```
(lldb) list 3
   3    int sum(int a, int b) {
   4        int result = a + b;
   5        return result;
   6    }
   7    
   8    int main() {
   9        int x = 5;
   10       int y = 10;
   11       int res = sum(x, y);
   12       printf("Result: %d\n", res);
```
По дефолту команда list (без аргументов) показывает 10 строк, начиная с текущей позиции указателя. Чтобы просмотреть следующие 10 (если есть) строк, необходимо нажать enter <br> <br>
Важно: list выводит 10 строк, так что если, например, функция, которую вы хотите посмотреть, длиннее 10 строк, она покажется не полностью и придется нажимать enter
### 4. break
- Позволяет создать breakpoint, на котором выполнение программы приостановитс для дальнейшего деббагинга
```
(lldb) breakpoint set --name main'
Breakpoint 1: where = a.out`main + 24 at main.c:10:9, address = 0x00000001000004a0
(lldb) breakpoint list
Current breakpoints:
1: name = 'main', locations = 1
  1.1: where = a.out`main + 24 at main.c:10:9, address = a.out[0x00000001000004a0], unresolved, hit count = 0 
```
Теперь run остановится на первой строке функции main 
### 5. run
- позволяет запустить программу в треминале
Если запустить без breakpoint:
```
run
Process 6018 launched: '/Users/georgy/Documents/C_projects/Errors_lab/a.out' (arm64)
Result: 15
Process 6018 exited with status = 0 (0x00000000) 
```
Если запустить после создания breakpoint
```
(lldb) run
Process 6018 launched: '/Users/georgy/Documents/C_projects/Errors_lab/a.out' (arm64)
Process 6018 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 1.1
       frame #0: 0x00000001000004a0 a.out`main at main.c:10:9
   7    
   8    
   9    int main() {
-> 10       int x = 5;
   11       int y = 10;
   12       int res = sum(x, y);
   13       printf("Result: %d\n", res);
Target 0: (a.out) stopped.
```
### 6. next
- Продвигает выполнение программы на следующую строку
```
(lldb) next
Process 6018 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000004a8 a.out`main at main.c:11:9
   8    
   9    int main() {
   10       int x = 5;
-> 11       int y = 10;
   12       int res = sum(x, y);
   13       printf("Result: %d\n", res);
   14       return 0;
Target 0: (a.out) stopped.

(lldb) next
Process 6018 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000004b0 a.out`main at main.c:12:19
   9    int main() {
   10       int x = 5;
   11       int y = 10;
-> 12       int res = sum(x, y);
   13       printf("Result: %d\n", res);
   14       return 0;
   15   }
Target 0: (a.out) stopped.

(lldb) next
Process 6018 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000004c0 a.out`main at main.c:13:28
   10       int x = 5;
   11       int y = 10;
   12       int res = sum(x, y);
-> 13       printf("Result: %d\n", res);
   14       return 0;
   15   }
Target 0: (a.out) stopped.

(lldb) next
Result: 15
Process 6018 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000004dc a.out`main at main.c:14:5
   11       int y = 10;
   12       int res = sum(x, y);
   13       printf("Result: %d\n", res);
-> 14       return 0;
   15   }
Target 0: (a.out) stopped.
```
### 7. print и expr (аналог set var из gbd)
Перезапустим run:
```
(lldb) run
There is a running process, kill it and restart?: [Y/n] y
Process 6102 exited with status = 9 (0x00000009) killed
Process 6107 launched: '/Users/georgy/Documents/C_projects/Errors_lab/a.out' (arm64)
Process 6107 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 1.1
       frame #0: 0x00000001000004a0 a.out`main at main.c:10:9
   7    
   8    
   9    int main() {
-> 10       int x = 5;
   11       int y = 10;
   12       int res = sum(x, y);
   13       printf("Result: %d\n", res);
Target 0: (a.out) stopped.
(lldb) next
Process 6107 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000004a8 a.out`main at main.c:11:9
   8    
   9    int main() {
   10       int x = 5;
-> 11       int y = 10;
   12       int res = sum(x, y);
   13       printf("Result: %d\n", res);
   14       return 0;
Target 0: (a.out) stopped.
(lldb) print x
(int) 5
```
- Команда print вывела значение переменной x
Изменим в режиме отладки значение x:
```
(lldb) expr x = 10
(int) $1 = 10
```
### 8. step
- в отличие от next, заходит внутрь функции
```
(lldb) next
Process 6180 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
       frame #0: 0x00000001000004b0 a.out`main at main.c:12:19
   9    int main() {
   10       int x = 5;
   11       int y = 10;
-> 12       int res = sum(x, y);
   13       printf("Result: %d\n", res);
   14       return 0;
   15   }
Target 0: (a.out) stopped.
(lldb) step
Process 6180 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step in
       frame #0: 0x000000010000046c a.out`sum(a=10, b=10) at main.c:4:18
   1    #include <stdio.h>
   2    
   3    int sum(int a, int b) {
-> 4        int result = a + b;
   5        return result;
   6    }
   7    
Target 0: (a.out) stopped.
``` 
Таким образом мы зашли внутрь функции sum 
### 9. bt
- Позволяет посмотреть стек вызовов функции
```
(lldb) bt
* thread #1, queue = 'com.apple.main-thread', stop reason = step in
   * frame #0: 0x000000010000046c a.out`sum(a=10, b=10) at main.c:4:18
     frame #1: 0x00000001000004bc a.out`main at main.c:12:15
```
Видим, что в sum передался измененный нами x
### 10. continue
- Завершает отладку и сдвигает ее до следующего брейкпоинта (если он есть) или же до конца программы
```
(lldb) continue
Process 6200 resuming
Result: 20
Process 6200 exited with status = 0 (0x00000000)
(lldb) exit
```
### 11. Работа с рекурсиями
Напишем в файле funcs.c:
```
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
```
В файле funcs.h:
```
#pragma once
int factorial(int n);
void indirectA(int n);
void indirectB(int n);
void infinite(int n);
```
В файле main.c:
```
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
```
Если пробовать компилировать код через `gcc -g main.c -o a.out`, то выведется ошибка:
```
georgy@MacBook-Air-Georgij Errors_lab % gcc -g main.c -o a.out            
Undefined symbols for architecture arm64:
  "_factorial", referenced from:
      _main in main-f59258.o
  "_indirectA", referenced from:
      _main in main-f59258.o
  "_infinite", referenced from:
      _main in main-f59258.o
ld: symbol(s) not found for architecture arm64
clang: error: linker command failed with exit code 1 (use -v to see invocation)
```
Это происходит так как мы компилируем только файл main.c, а funcs.c, функции из которого используются в main, не компилировали <br>
В данном случае надо использовать `gcc -g main.c funcs.c -o a.out`, что скомпилирует и funcs.c тоже <br>
Заходим в дебаггер: `lldb ./a.out` 
#### Прямая рекурсия
- Функция, вызывающая сама себя, имеющая базовый случай (завершение рекурсии) <br> <br>
Установим брейкпоинт на функции factorial:
```
(lldb) b factorial
Breakpoint 1: where = a.out`factorial + 16 at funcs.c:6:9, address = 0x00000001000004fc
```
Запустим программу, она остановится на функции:
```
run
Process 7506 launched: '/Users/georgy/Documents/C_projects/Errors_lab/a.out' (arm64)
Прямая рекурсия
Process 7506 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 1.1
       frame #0: 0x00000001000004fc a.out`factorial(n=3) at funcs.c:6:9
   3    
   4    
   5    int factorial(int n) { // пример прямой рекурсии (вычисление факториала)
-> 6        if (n <= 1) return 1;
   7        return n * factorial(n - 1);
   8    }
   9    
Target 0: (a.out) stopped.
(lldb) print n
(int) 3
```
Напишем continue - это пройдет одну итерацию функции (n станет равно 2):
```
continue
Process 7506 resuming
Process 7506 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 1.1
       frame #0: 0x00000001000004fc a.out`factorial(n=2) at funcs.c:6:9
   3    
   4    
   5    int factorial(int n) { // пример прямой рекурсии (вычисление факториала)
-> 6        if (n <= 1) return 1;
   7        return n * factorial(n - 1);
   8    }
   9    
Target 0: (a.out) stopped.
(lldb) print n
(int) 2
```
Еще итерация:
```
(lldb) continue
Process 7506 resuming
Process 7506 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 1.1
       frame #0: 0x00000001000004fc a.out`factorial(n=1) at funcs.c:6:9
   3    
   4    
   5    int factorial(int n) { // пример прямой рекурсии (вычисление факториала)
-> 6        if (n <= 1) return 1;
   7        return n * factorial(n - 1);
   8    }
   9    
Target 0: (a.out) stopped.
(lldb) print n
(int) 1
```
Используем bt, чтобы осмотреть стек вызовов функции:
```
(lldb) bt
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 1.1
   * frame #0: 0x00000001000004fc a.out`factorial(n=1) at funcs.c:6:9
     frame #1: 0x000000010000052c a.out`factorial(n=2) at funcs.c:7:16
     frame #2: 0x000000010000052c a.out`factorial(n=3) at funcs.c:7:16
     frame #3: 0x0000000100000490 a.out`main at main.c:7:15
     frame #4: 0x0000000180597e80 dyld`start + 6688
(lldb) 
```
Можем вернуться на второй фрейм (когда вызывалась функция с n = 3)
```
(lldb) frame select 2
frame #2: 0x000000010000052c a.out`factorial(n=3) at funcs.c:7:16
   4    
   5    int factorial(int n) { // пример прямой рекурсии (вычисление факториала)
   6        if (n <= 1) return 1;
-> 7        return n * factorial(n - 1);
   8    }
   9    
   10   // Косвенная рекурсия
(lldb) print n
(int) 3
```
Уберем брейкпоинт: `breakpoint delete`
#### Косвенная рекурсия
- Две (или более) функции, вызывающие друг друга (Функция А вызывает B, а B вызывает А)
Установим breakpoint-ы на двух функциях и сдвинемся в первую из них:
```
(lldb) breakpoint set --name indirectA
Breakpoint 2: where = a.out`indirectA + 16 at funcs.c:12:9, address = 0x000000010000055c
(lldb) breakpoint set --name indirectB
Breakpoint 3: where = a.out`indirectB + 16 at funcs.c:18:9, address = 0x00000001000005b4
(lldb) continue
Process 7506 resuming
Факториал 3: 6
Косвенная рекурсия
Process 7506 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 2.1
       frame #0: 0x000000010000055c a.out`indirectA(n=3) at funcs.c:12:9
   9    
   10   // Косвенная рекурсия
   11   void indirectA(int n) {
-> 12       if (n <= 0) return;
   13       printf("A: %d\n", n);
   14       indirectB(n - 1);
   15   }
Target 0: (a.out) stopped.
(lldb) print n
(int) 3
```
Когда напишем continue, попадем в функцию B:
```
(lldb) continue
Process 7506 resuming
A: 3
Process 7506 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 3.1
       frame #0: 0x00000001000005b4 a.out`indirectB(n=2) at funcs.c:18:9
   15   }
   16   
   17   void indirectB(int n) {
-> 18       if (n <= 0) return;
   19       printf("B: %d\n", n);
   20       indirectA(n - 1);
   21   }
Target 0: (a.out) stopped.
(lldb) print n
(int) 2
```
Снова continue, снова в функции А:
```
(lldb) continue
Process 7506 resuming
B: 2
Process 7506 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 2.1
       frame #0: 0x000000010000055c a.out`indirectA(n=1) at funcs.c:12:9
   9    
   10   // Косвенная рекурсия
   11   void indirectA(int n) {
-> 12       if (n <= 0) return;
   13       printf("A: %d\n", n);
   14       indirectB(n - 1);
   15   }
Target 0: (a.out) stopped.
(lldb) print n
(int) 1
```
Стек вызовов:
```
(lldb) bt
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 2.1
   * frame #0: 0x000000010000055c a.out`indirectA(n=1) at funcs.c:12:9
     frame #1: 0x00000001000005ec a.out`indirectB(n=2) at funcs.c:20:5
     frame #2: 0x0000000100000594 a.out`indirectA(n=3) at funcs.c:14:5
     frame #3: 0x00000001000004c8 a.out`main at main.c:10:5
     frame #4: 0x0000000180597e80 dyld`start + 6688
```
Наглядно видим чередование функций
#### Бесконечная рекурсия
- Прямая рекурсия, но без базового случая, а следовательно никогда не завершится, и программа выкинет ошибку
Удаляем брейкпоинты 
```
(lldb) breakpoint delete
About to delete all breakpoints, do you want to do that?: [Y/n] 
All breakpoints removed. (2 breakpoints)
```
Вводим continue, программа входит в бесконечную рекурсию и аварийно завершится:
```
(lldb) continue
Process 7506 resuming
A: 1
Бесконечная рекурсия
Process 7506 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = EXC_BAD_ACCESS (code=2, address=0x16f603ff0)
       frame #0: 0x0000000100000600 a.out`infinite(n=261505) at funcs.c:24
   21   }
   22   
   23   // Бесконечная рекурсия
-> 24   void infinite(int n) {
   25       int a = n;
   26       infinite(n + 1);
   27   }
Target 0: (a.out) stopped.
```
Стек забит огромным количеством вызовов, поэтому чтобы он не завис, просмотрим только последние 15:
```
(lldb) bt 15
* thread #1, queue = 'com.apple.main-thread', stop reason = EXC_BAD_ACCESS (code=2, address=0x16f603ff0)
   * frame #0: 0x0000000100000600 a.out`infinite(n=261505) at funcs.c:24
     frame #1: 0x0000000100000620 a.out`infinite(n=261505) at funcs.c:26:5
     frame #2: 0x0000000100000620 a.out`infinite(n=261504) at funcs.c:26:5
     frame #3: 0x0000000100000620 a.out`infinite(n=261503) at funcs.c:26:5
     frame #4: 0x0000000100000620 a.out`infinite(n=261502) at funcs.c:26:5
     frame #5: 0x0000000100000620 a.out`infinite(n=261501) at funcs.c:26:5
     frame #6: 0x0000000100000620 a.out`infinite(n=261500) at funcs.c:26:5
     frame #7: 0x0000000100000620 a.out`infinite(n=261499) at funcs.c:26:5
     frame #8: 0x0000000100000620 a.out`infinite(n=261498) at funcs.c:26:5
     frame #9: 0x0000000100000620 a.out`infinite(n=261497) at funcs.c:26:5
     frame #10: 0x0000000100000620 a.out`infinite(n=261496) at funcs.c:26:5
     frame #11: 0x0000000100000620 a.out`infinite(n=261495) at funcs.c:26:5
     frame #12: 0x0000000100000620 a.out`infinite(n=261494) at funcs.c:26:5
     frame #13: 0x0000000100000620 a.out`infinite(n=261493) at funcs.c:26:5
     frame #14: 0x0000000100000620 a.out`infinite(n=261492) at funcs.c:26:5
```