#include "schedulers.h"
#include "list.h"
#include "stdlib.h"
#include "cpu.h"
#include "sys/param.h"
#include "string.h"

struct node* tasks = NULL;  // Указатель на список задач, инициализирован как NULL
struct node* last_node = NULL;  // Указатель на последний узел в списке задач

// Функция добавления задачи в список
void add(Task* task) {
    insert(&tasks, task);  // Вставляем новую задачу в список через функцию insert
    if (last_node == NULL) {  // Если last_node еще не инициализирован (список был пуст)
        last_node = tasks;  // Указываем на первую задачу в списке как на last_node
    }
}

// Функция выбора следующей задачи для выполнения
Task* pick_next() {
    struct node* pointer = tasks->prev;  // Начинаем проверку с предыдущей задачи относительно текущей
    struct node* max_task_priority_node = tasks->task->burst ? tasks : tasks->next;  // Инициализируем задачу с наибольшим приоритетом
    while (pointer && pointer != tasks) {  // Проходим по кругу до тех пор, пока не вернемся к исходной задаче
        // Проверяем, не завершена ли задача (burst != 0) и имеет ли она более высокий приоритет
        if (pointer->task->burst != 0 && pointer->task->priority >= max_task_priority_node->task->priority) {
            max_task_priority_node = pointer;  // Обновляем задачу с наибольшим приоритетом
        }
        pointer = pointer->prev;  // Переходим к предыдущей задаче в списке
    }

    if (tasks != tasks->next) {  // Проверяем, что список содержит более одной задачи
        // Если текущая задача завершена (burst == 0), удаляем её из списка
        if (tasks->task->burst == 0) {
            tasks->prev->next = tasks->next;  // Обновляем связи: предыдущая задача ссылается на следующую
            tasks->next->prev = tasks->prev;  // Следующая задача ссылается на предыдущую
        }
        tasks = max_task_priority_node;  // Перемещаемся на задачу с наибольшим приоритетом
        return tasks->task;  // Возвращаем задачу для выполнения
    }
    return NULL;  // Если в списке осталась только одна задача, возвращаем NULL
}

// Основная функция планировщика, которая управляет выполнением задач
void schedule() {
    last_node->next = tasks;  // Замыкаем список, делая его циклическим (последняя задача указывает на первую)
    tasks->prev = last_node;  // Первая задача указывает на последнюю

    // Пока есть задачи для выполнения
    while (pick_next() != NULL) {
        // Выполняем задачу с ограничением по времени (TIME_QUANTUM)
        run(tasks->task, MIN(tasks->task->burst, TIME_QUANTUM));

        // Уменьшаем оставшееся время выполнения задачи на величину кванта
        tasks->task->burst -= MIN(tasks->task->burst, TIME_QUANTUM);
    }
}

// сначала прио, потом циклическое, смысл - кванты времени