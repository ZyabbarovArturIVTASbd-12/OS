#include "schedulers.h"
#include "list.h"
#include "stdlib.h"
#include "cpu.h"

struct node* tasks = NULL;  // Указатель на список задач, который изначально пуст

// Функция добавляет новую задачу в список
void add(Task* task) {
    insert(&tasks, task);  // Вставляем задачу в связный список через функцию insert
}

Task* selected_task = NULL;  // Указатель на текущую выбранную задачу, по умолчанию равен NULL

// Функция выбирает следующую задачу для выполнения, основываясь на максимальном приоритете
Task* pick_next_task() {
    if (selected_task != NULL) {  // Если уже есть выбранная задача
        delete(&tasks, selected_task);  // Удаляем её из списка задач, чтобы не выполнять её повторно
    }
    if (tasks == NULL) {  // Если список задач пуст
        return NULL;  // Возвращаем NULL, так как больше задач нет
    }

    struct node* pointer = tasks->next;  // Начинаем проверку со второй задачи
    Task* max_task_priority = tasks->task;  // Инициализируем первую задачу как задачу с максимальным приоритетом

    // Идем по всему списку задач и ищем ту, у которой приоритет больше
    while (pointer != NULL) {
        if (pointer->task->priority > max_task_priority->priority) {  // Если нашли задачу с приоритетом больше текущего
            max_task_priority = pointer->task;  // Обновляем задачу с максимальным приоритетом
        }
        pointer = pointer->next;  // Переходим к следующему элементу списка
    }

    return max_task_priority;  // Возвращаем задачу с наибольшим приоритетом для выполнения
}

// Основная функция планировщика, которая управляет выполнением задач
void schedule() {
    selected_task = pick_next_task();  // Выбираем задачу с наибольшим приоритетом для выполнения
    while (selected_task != NULL) {  // Пока есть задачи для выполнения
        run(selected_task, selected_task->burst);  // Выполняем выбранную задачу с её временем burst
        selected_task = pick_next_task();  // Выбираем следующую задачу после завершения текущей
    }
}

// по прио, от высшего к меньшему