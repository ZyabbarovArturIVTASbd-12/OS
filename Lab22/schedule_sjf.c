#include "schedulers.h"
#include "list.h"
#include <stdlib.h>
#include "cpu.h"

struct node* tasks = NULL;  // Указатель на список задач, который изначально пуст (NULL)

// Функция добавляет задачу в список
void add(Task* task) {
    insert(&tasks, task);  // Вставляем новую задачу в список
}

Task* selected_task = NULL;  // Указатель на текущую выбранную задачу, поначалу равен NULL

// Функция выбирает следующую задачу для выполнения, основываясь на наименьшем времени burst (Shortest Job First)
Task* pick_next_task() {
    if (selected_task != NULL) {  // Если есть текущая выбранная задача
        delete(&tasks, selected_task);  // Удаляем её из списка задач
    }

    if (tasks == NULL) {  // Если список задач пуст
        return NULL;  // Возвращаем NULL, так как больше задач нет
    }

    struct node* pointer = tasks->next;  // Начинаем проверку со второй задачи, первая уже выбрана как минимальная
    Task* min_task_burst = tasks->task;  // Первая задача в списке считается минимальной по burst

    // Идем по всему списку и ищем задачу с минимальным burst
    while (pointer != NULL) {
        if (pointer->task->burst < min_task_burst->burst) {  // Если нашли задачу с меньшим burst, чем текущая минимальная
            min_task_burst = pointer->task;  // Обновляем минимальную задачу
        }
        pointer = pointer->next;  // Переходим к следующему элементу списка
    }

    return min_task_burst;  // Возвращаем задачу с наименьшим burst для выполнения
}

// Основная функция планировщика, которая управляет выполнением задач
void schedule() {
    selected_task = pick_next_task();  // Выбираем следующую задачу с наименьшим burst
    while (selected_task != NULL) {  // Пока есть задачи для выполнения
        run(selected_task, selected_task->burst);  // Выполняем задачу с её временем burst
        selected_task = pick_next_task();  // Выбираем следующую задачу
    }
}


// выполнение на основе предпалагаемого времени выполнения, от меньшего к большему