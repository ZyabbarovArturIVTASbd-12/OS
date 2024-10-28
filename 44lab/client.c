#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

// Структура данных для передачи аргументов в задачи
struct data
{
    int a; // Первое число
    int b; // Второе число
};

// Функция для выполнения сложения
void add(void* param)
{
    struct data* temp;
    temp = (struct data*)param; // Преобразование параметра в указатель на struct data
    printf("I add two values %d and %d result = %d\n", temp->a, temp->b, temp->a + temp->b); // Вывод результата сложения
}

// Функция для выполнения вычитания
void sub(void* param)
{
    struct data* temp;
    temp = (struct data*)param; // Преобразование параметра в указатель на struct data
    printf("I sub from value %d value %d result = %d\n", temp->a, temp->b, temp->a - temp->b); // Вывод результата вычитания
}

// Функция для выполнения умножения
void mul(void* param)
{
    struct data* temp;
    temp = (struct data*)param; // Преобразование параметра в указатель на struct data
    printf("I mul two values %d and %d result = %d\n", temp->a, temp->b, temp->a * temp->b); // Вывод результата умножения
}

// Функция для выполнения деления
void div(void* param)
{
    struct data* temp;
    temp = (struct data*)param; // Преобразование параметра в указатель на struct data
    printf("I div value %d by value %d result = %d\n", temp->a, temp->b, temp->a / temp->b); // Вывод результата деления
}

// Функция для инкрементации значений до 1000000
void inc(void* param)
{
    struct data* temp;
    temp = (struct data*)param; // Преобразование параметра в указатель на struct data
    for (int i = 0; temp->a < 1000000; i++) {
        printf("Inc value %d inc value %d\n", temp->a++, temp->b++); // Увеличение значений a и b и вывод результата
    }
}

int main(void)
{
    // Создание переменных для передачи данных в задачи
    struct data work, work1, work2, work3;
    work.a = 0;   // Инициализация значений первой структуры
    work.b = 0;
    work1.a = 55; // Инициализация значений второй структуры
    work1.b = 100;
    work2.a = 33; // Инициализация значений третьей структуры
    work2.b = 22;
    work3.a = 99; // Инициализация значений четвертой структуры
    work3.b = 77;

    // Инициализация пула потоков
    pool_init();

    // Отправка задач в пул потоков
    pool_submit(&add, &work);  // Добавление задачи сложения для work
    pool_submit(&sub, &work);  // Добавление задачи вычитания для work
    // pool_submit(&mul, &work); 
    // pool_submit(&div, &work); 

    pool_submit(&add, &work1); // Добавление задачи сложения для work1
    pool_submit(&sub, &work1); // Добавление задачи вычитания для work1
    // pool_submit(&mul, &work1); 
    // pool_submit(&div, &work1); 

    // pool_submit(&add, &work2); 
    pool_submit(&sub, &work2);  // Добавление задачи вычитания для work2
    // pool_submit(&mul, &work2); 
    // pool_submit(&div, &work2); 

    pool_submit(&add, &work3);  // Добавление задачи сложения для work3
    pool_submit(&sub, &work3);  // Добавление задачи вычитания для work3
    // pool_submit(&mul, &work3); 
    // pool_submit(&div, &work3); 
    // // may be helpful
    // sleep(3); 

    // Завершение работы пула потоков
    pool_shutdown();
    return 0;
}
