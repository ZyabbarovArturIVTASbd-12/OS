#include "schedulers.h"
#include "list.h"
#include <stdlib.h>
#include "cpu.h"

struct node* tasks = NULL;  // Указатель на список задач, который изначально пуст (NULL)

// Функция добавляет задачу в список
void add(Task* task) {
    insert(&tasks, task);  // Используется функция insert (вставка) из библиотеки "list.h", чтобы добавить задачу в список
}

Task* selected_task = NULL;  // Указатель на текущую задачу, которая будет выполняться, поначалу равен NULL

// Функция выбирает следующую задачу для выполнения
Task* pick_next_task() {
    if (selected_task != NULL) {  // Если есть выбранная задача
        delete(&tasks, selected_task);  // Удаляем её из списка задач
    }
    if (tasks == NULL) {  // Если список задач пуст
        return NULL;  // Возвращаем NULL (нет задач для выполнения)
    }
    return tasks->task;  // Возвращаем первую задачу из списка (FIFO — "первый пришел, первый ушел")
}

// Основная функция планировщика, которая управляет выполнением задач
void schedule() {
    selected_task = pick_next_task();  // Выбираем следующую задачу для выполнения
    while (selected_task != NULL) {  // Пока есть задачи для выполнения
        run(selected_task, selected_task->burst);  // Выполняем задачу с её временем выполнения (burst)
        selected_task = pick_next_task();  // Выбираем следующую задачу после завершения предыдущей
    }
}

// fifo, кто первый пришел тот первый выполняется