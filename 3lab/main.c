#include <stdio.h>
#include <stdlib.h>

#define FRAME_SIZE 256           // Размер одной рамки (в байтах)
#define NUM_FRAMES 128           // Количество рамок в физической памяти
#define TLB_SIZE 16              // Количество записей в TLB (таблице локальности)
#define PAGE_TABLE_SIZE 256      // Количество страниц в таблице страниц

// Структура для записи в TLB
typedef struct
{
    int pageNumber;   // Номер страницы
    int frameNumber;  // Номер рамки
} TLBEntry;

// Массив для физической памяти
char physicalMemory[NUM_FRAMES][FRAME_SIZE];
// Массив для хранения записей TLB
TLBEntry tlb[TLB_SIZE];
// Таблица страниц (отображает номера страниц на номера рамок)
int pageTable[PAGE_TABLE_SIZE];
// Массив для отслеживания занятости рамок (0 - свободно, 1 - занято)
int freeFrames[NUM_FRAMES];
// Указатель на текущую позицию TLB
int tlbPointer = 0;
// Счетчик попаданий в TLB
int tlbHitCount = 0;
// Указатель для FIFO замещения страничек
int fifoPointer = 0;

// Функция для чтения данных из файла подкачки
void readFromBackingStore(int pageNumber)
{
    FILE* backingStore = fopen("BACKING_STORE.bin", "rb"); // Открываем файл подкачки для чтения
    fseek(backingStore, pageNumber * FRAME_SIZE, SEEK_SET); // Перемещаем указатель на нужную страницу
    fread(physicalMemory[fifoPointer], sizeof(char), FRAME_SIZE, backingStore); // Читаем данные в физическую память
    fclose(backingStore); // Закрываем файл подкачки
}

// Функция для обновления записи в TLB
void updateTlb(int pageNumber, int frameNumber)
{
    tlb[tlbPointer].pageNumber = pageNumber; // Устанавливаем номер страницы
    tlb[tlbPointer].frameNumber = frameNumber; // Устанавливаем номер рамки
    tlbPointer = (++tlbPointer) % TLB_SIZE; // Обновляем указатель TLB с циклом
}

// Функция для получения свободной рамки
int getFreeFrame()
{
    int freeFrame = -1; // Изначально предполагаем, что свободной рамки нет
    for (int i = 0; i < NUM_FRAMES; i++)
    {
        if (freeFrames[i] == 0) // Если рамка свободна
        {
            freeFrame = i; // Запоминаем номер свободной рамки
            freeFrames[i] = 1; // Отмечаем ее как занятую
            break; // Прерываем цикл, поскольку свободная рамка найдена
        }
    }
    return freeFrame; // Возвращаем номер свободной рамки или -1, если таких нет
}

// Функция для преобразования виртуального адреса в физический
int translateAddress(int virtualAddress)
{
    int pageNumber = (virtualAddress >> 8) & 0xFF; // Извлекаем номер страницы из виртуального адреса
    int offset = virtualAddress & 0xFF; // Извлекаем смещение из виртуального адреса
    int frameNumber;

    // Проверяем TLB на наличие соответствия
    for (int i = 0; i < TLB_SIZE; i++)
    {
        if (tlb[i].pageNumber == pageNumber) // Если мы нашли страницу в TLB
        {
            frameNumber = tlb[i].frameNumber; // Получаем номер рамки из TLB
            tlbHitCount++; // Увеличиваем счетчик попаданий в TLB
            return frameNumber * FRAME_SIZE + offset; // Возвращаем физический адрес
        }
    }

    // Если страницы нет в TLB, проверяем таблицу страниц
    if (pageTable[pageNumber] == -1)
    {
        int frameNumber = getFreeFrame(); // Ищем свободную рамку
        if (frameNumber == -1) // Если свободных рамок нет
        {
            frameNumber = fifoPointer; // Используем рамку по FIFO
        }
        pageTable[pageNumber] = frameNumber; // Добавляем запись в таблицу страниц
        readFromBackingStore(pageNumber); // Читаем страницу из файла подкачки
        fifoPointer = (++fifoPointer) % NUM_FRAMES; // Обновляем указатель FIFO
    }

    frameNumber = pageTable[pageNumber]; // Получаем номер рамки из таблицы страниц
    updateTlb(pageNumber, frameNumber); // Обновляем TLB с новой записью

    return frameNumber * FRAME_SIZE + offset; // Возвращаем физический адрес
}

int main()
{
    FILE* addressesFile = fopen("addresses.txt", "r"); // Открываем файл с виртуальными адресами
    FILE* outputFile = fopen("output.txt", "w+"); // Открываем файл для вывода результатов

    // Проверяем, успешно ли открыты файлы
    if (addressesFile == NULL || outputFile == NULL)
    {
        perror("Error opening file"); // Если один из файлов не удалось открыть, выводим сообщение об ошибке
        return -1;
    }

    // Инициализируем TLB
    for (int i = 0; i < TLB_SIZE; i++)
    {
        tlb[i].pageNumber = -1; // Устанавливаем номер страницы как -1 (помечаем пустыми)
        tlb[i].frameNumber = -1; // Устанавливаем номер рамки как -1 (помечаем пустыми)
    }

    // Инициализируем таблицу страниц
    for (int i = 0; i < PAGE_TABLE_SIZE; i++)
    {
        pageTable[i] = -1; // Все страницы изначально отсутствуют (помечены -1)
    }

    // Инициализируем массив свободных рамок
    for (int i = 0; i < NUM_FRAMES; i++)
    {
        freeFrames[i] = 0; // Сначала все рамки помечены как свободные
    }

    int total = 0; // Общий счетчик виртуальных адресов
    int virtualAddress; // Переменная для хранения текущего виртуального адреса
    // Читаем виртуальные адреса из файла
    while (fscanf(addressesFile, "%d", &virtualAddress) != EOF)
    {
        int physicalAddress = translateAddress(virtualAddress); // Транслируем виртуальный адрес в физический
        char value = physicalMemory[physicalAddress / FRAME_SIZE][physicalAddress % FRAME_SIZE] & 0xFF; // Получаем значение по физическому адресу
        fprintf(outputFile, "Virtual address: %d Physical address: %d Value: %d", virtualAddress, physicalAddress, value); // Записываем результат в выходной файл
        total++; // Увеличиваем общий счетчик
    }

    // Вычисляем частоту ошибок страниц и частоту попаданий в TLB
    double pageFault_rate = (double)fifoPointer / total; // Число ошибок страниц делим на общее количество адресов
    double tlbHit_rate = (double)tlbHitCount / total; // Число попаданий в TLB делим на общее количество адресов

    // Выводим результаты на экран
    printf("Page fault rate: %.2f%%\n", pageFault_rate * 100);
    printf("TLB hit rate: %.2f%%\n", tlbHit_rate * 100);

    fclose(addressesFile); // Закрываем файл с адресами
    fclose(outputFile); // Закрываем выходной файл
    return 0; // Завершаем программу
}