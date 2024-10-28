#include "threadpool.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <assert.h>

#define QUEUE_SIZE 20             // Максимальный размер очереди задач
#define NUMBER_OF_THREADS 8        // Количество потоков в пуле

// Структура, представляющая задачу в пуле потоков
typedef struct {
    void (*function)(void* p);    // Указатель на функцию, которую нужно выполнить
    void* data;                   // Данные, передаваемые в функцию
} task;

// Очередь задач и её свойства
task work_queue[QUEUE_SIZE];       // Массив для хранения задач
int queue_front = 0;               // Индекс начала очереди
int queue_rear = -1;               // Индекс конца очереди
int queue_count = 0;               // Счетчик текущего числа задач в очереди

// Идентификаторы рабочих потоков и мьютекс для защиты доступа к очереди
pthread_t worker_threads[NUMBER_OF_THREADS];   // Массив потоков пула
pthread_mutex_t queue_mutex;                   // Мьютекс для синхронизации доступа к очереди

// Семафор для сигнализации о наличии задач в очереди
sem_t task_semaphore;

// Обертка для блокировки мьютекса с проверкой ошибок
void Pthread_mutex_lock(pthread_mutex_t* mutex) {
    int rc = pthread_mutex_lock(mutex);
    assert(rc == 0);   // Проверяет успешное выполнение
}

// Обертка для разблокировки мьютекса с проверкой ошибок
void Pthread_mutex_unlock(pthread_mutex_t* mutex) {
    int rc = pthread_mutex_unlock(mutex);
    assert(rc == 0);   // Проверяет успешное выполнение
}

// Обертка для инициализации мьютекса с проверкой ошибок
void Pthread_mutex_init(pthread_mutex_t* mutex) {
    int rc = pthread_mutex_init(mutex, NULL);
    assert(rc == 0);   // Проверяет успешное выполнение
}

// Добавление задачи в очередь
void enqueue(task t) {
    Pthread_mutex_lock(&queue_mutex);           // Блокируем мьютекс для безопасного доступа
    if (queue_count < QUEUE_SIZE) {             // Проверка на наличие свободного места в очереди
        queue_rear = (++queue_rear) % QUEUE_SIZE; // Устанавливаем новый индекс конца очереди
        work_queue[queue_rear] = t;             // Добавляем задачу в очередь
        queue_count++;                          // Увеличиваем счетчик задач
        sem_post(&task_semaphore);              // Уведомляем о наличии новой задачи
    }
    Pthread_mutex_unlock(&queue_mutex);         // Разблокируем мьютекс
}

// Извлечение задачи из очереди
task dequeue() {
    task worktodo;
    sem_wait(&task_semaphore);                 // Ожидание наличия задачи (блокировка при отсутствии задач)
    Pthread_mutex_lock(&queue_mutex);          // Блокируем мьютекс для безопасного доступа
    worktodo = work_queue[queue_front];        // Извлекаем задачу из начала очереди
    queue_front = (++queue_front) % QUEUE_SIZE; // Перемещаем начало очереди
    queue_count--;                             // Уменьшаем счетчик задач
    Pthread_mutex_unlock(&queue_mutex);        // Разблокируем мьютекс
    return worktodo;                           // Возвращаем извлеченную задачу
}

// Выполнение задачи
void execute(void (*somefunction)(void* p), void* p) {
    (*somefunction)(p);                        // Вызов переданной функции с параметром
}

// Функция рабочего потока
void* worker(void* param) {
    while (1) {
        task worktodo = dequeue();             // Извлекаем задачу из очереди
        if (worktodo.function == NULL)         // Если функция равна NULL, завершить поток
            pthread_exit(NULL);
        execute(worktodo.function, worktodo.data); // Выполняем задачу
    }
}

// Подача задачи в пул потоков
int pool_submit(void (*somefunction)(void* p), void* p) {
    task new_task;
    new_task.function = somefunction;          // Устанавливаем указатель на функцию
    new_task.data = p;                         // Устанавливаем данные
    enqueue(new_task);                         // Добавляем задачу в очередь
    return 0;
}

// Инициализация пула потоков
void pool_init(void) {
    Pthread_mutex_init(&queue_mutex);          // Инициализация мьютекса
    sem_init(&task_semaphore, 0, 0);           // Инициализация семафора (начальное значение - 0)
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_create(&worker_threads[i], NULL, worker, NULL); // Создаем рабочие потоки
    }
}

// Завершение работы пула потоков
void pool_shutdown(void) {
    // Отправляем в очередь NULL-задачи для завершения каждого потока
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pool_submit(NULL, NULL);
    }
    // Ожидание завершения всех потоков
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_join(worker_threads[i], NULL);
    }
    // Уничтожение мьютекса и семафора после завершения работы всех потоков
    pthread_mutex_destroy(&queue_mutex);
    sem_destroy(&task_semaphore);
}
