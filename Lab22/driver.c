/**
 * Driver.c
 *
 * Schedule is in the format
 *
 *  [name] [priority] [CPU burst]
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"

#define SIZE 10000

int main(int argc, char* argv[]) {
    FILE* in; // Указатель на файл для чтения.
    char* temp; // Временная строка для хранения прочитанных данных.
    char task[SIZE]; // Буфер для хранения одной строки задачи.

    char* name; // Переменная для хранения имени задачи.
    int priority; // Переменная для хранения приоритета задачи.
    int burst; // Переменная для хранения времени выполнения CPU.

    // Открываем входной файл для чтения, имя файла берется из аргументов командной строки.
    in = fopen(argv[1], "r");

    Task* tasks_array[MAX_TASKS]; // Массив указателей на задачи.
    int tasks_count = 0; // Счетчик задач.
    struct node* temp_list; // Указатель на временный связный список.

    // Чтение задач из файла построчно.
    while (fgets(task, SIZE, in) != NULL) {
        temp = strdup(task); // Создаем дублированную строку для обработки.
        name = strsep(&temp, ","); // Извлекаем имя задачи.
        priority = atoi(strsep(&temp, ",")); // Извлекаем приоритет задачи и конвертируем строку в целое число.
        burst = atoi(strsep(&temp, ",")); // Извлекаем время выполнения задачи и конвертируем строку в целое число.

        // Добавляем задачу в список задач планировщика.
        add_task(&temp_list, name, priority, burst);
        tasks_array[tasks_count] = temp_list->task; // Сохраняем задачу в массиве.
        tasks_count++; // Увеличиваем счетчик задач.

        free(temp); // Освобождаем память, выделенную под временную строку.
    }

    // Очистка списка задач и добавление их в глобальную структуру планировщика.
    while (temp_list != NULL) {
        add(temp_list->task); // Добавляем задачу в планировщик.
        delete(&temp_list, temp_list->task); // Удаляем задачу из временного списка.
    }

    fclose(in); // Закрываем файл после завершения чтения.

    // Вызываем функцию планировщика.
    schedule();

    // Переменные для расчета средних значений.
    float average_turnaround_time = 0;
    float average_wait_time = 0;
    float average_response_time = 0;

    // Подсчет среднего времени завершения, ожидания и отклика для всех задач.
    for (int x = 0; x < tasks_count; x++) {
        average_turnaround_time += tasks_array[x]->time_end - tasks_array[x]->time_start; // Вычисляем время оборота.
        average_wait_time += tasks_array[x]->wait_time; // Суммируем время ожидания.
        average_response_time += tasks_array[x]->time_start; // Суммируем время отклика.
    }

    // Вычисляем средние значения.
    average_turnaround_time /= tasks_count;
    average_wait_time /= tasks_count;
    average_response_time /= tasks_count;
        
    printf("\nAverage turnaround time: %f", average_turnaround_time);
    printf("\nAverage wait time: %f", average_wait_time);
    printf("\nAverage response time: %f\n", average_response_time);

    return 0;
}
