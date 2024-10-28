#include "schedulers.h"
#include "list.h"
#include "stdlib.h"
#include "cpu.h"
#include "sys/param.h"

struct node* tasks = NULL;  // Указатель на список задач, который изначально пуст
struct node* last_node = NULL;  // Указатель на последний узел (задачу) в списке

// Функция добавления задачи в список
void add(Task* task) {
    insert(&tasks, task);  // Вставляем задачу в список через функцию insert
    if (last_node == NULL) {  // Если last_node еще не инициализирован
        last_node = tasks;  // Присваиваем last_node указатель на первую задачу (tasks)
    }
}

// Функция выбора следующей задачи для выполнения
Task* pick_next() {
    // Проверяем, не является ли текущая задача единственной в списке
    if (tasks != tasks->next) {
        // Если у текущей задачи burst (время на выполнение) равен 0, значит, задача завершена
        if (tasks->task->burst == 0) {
            // Удаляем текущую задачу из связного списка
            tasks->prev->next = tasks->next;  // Указываем, что предыдущая задача теперь указывает на следующую
            tasks->next->prev = tasks->prev;  // Указываем, что следующая задача ссылается на предыдущую
        }
        tasks = tasks->next;  // Переходим к следующей задаче в списке
        return tasks->task;  // Возвращаем задачу для выполнения
    }
    return NULL;  // Если осталась только одна задача, возвращаем NULL
}

// Основная функция планировщика
void schedule() {
    last_node->next = tasks;  // Замыкаем список, чтобы он стал циклическим (последняя задача ссылается на первую)
    tasks->prev = last_node;  // Связываем первую задачу с последней
    tasks = last_node;  // Начинаем выполнение с последней задачи

    // Пока есть задачи для выполнения
    while (pick_next() != NULL) {
        // Выполняем задачу, но не более чем за время TIME_QUANTUM
        run(tasks->task, MIN(tasks->task->burst, TIME_QUANTUM));

        // Уменьшаем оставшееся время burst у задачи на величину выполненного кванта
        tasks->task->burst -= MIN(tasks->task->burst, TIME_QUANTUM);
    }
}

// циклическое, смысл - кванты времени