#include <stdio.h>
#include <stdlib.h>

#define FRAME_SIZE 256           // ������ ����� ����� (� ������)
#define NUM_FRAMES 128           // ���������� ����� � ���������� ������
#define TLB_SIZE 16              // ���������� ������� � TLB (������� �����������)
#define PAGE_TABLE_SIZE 256      // ���������� ������� � ������� �������

// ��������� ��� ������ � TLB
typedef struct
{
    int pageNumber;   // ����� ��������
    int frameNumber;  // ����� �����
} TLBEntry;

// ������ ��� ���������� ������
char physicalMemory[NUM_FRAMES][FRAME_SIZE];
// ������ ��� �������� ������� TLB
TLBEntry tlb[TLB_SIZE];
// ������� ������� (���������� ������ ������� �� ������ �����)
int pageTable[PAGE_TABLE_SIZE];
// ������ ��� ������������ ��������� ����� (0 - ��������, 1 - ������)
int freeFrames[NUM_FRAMES];
// ��������� �� ������� ������� TLB
int tlbPointer = 0;
// ������� ��������� � TLB
int tlbHitCount = 0;
// ��������� ��� FIFO ��������� ���������
int fifoPointer = 0;

// ������� ��� ������ ������ �� ����� ��������
void readFromBackingStore(int pageNumber)
{
    FILE* backingStore = fopen("BACKING_STORE.bin", "rb"); // ��������� ���� �������� ��� ������
    fseek(backingStore, pageNumber * FRAME_SIZE, SEEK_SET); // ���������� ��������� �� ������ ��������
    fread(physicalMemory[fifoPointer], sizeof(char), FRAME_SIZE, backingStore); // ������ ������ � ���������� ������
    fclose(backingStore); // ��������� ���� ��������
}

// ������� ��� ���������� ������ � TLB
void updateTlb(int pageNumber, int frameNumber)
{
    tlb[tlbPointer].pageNumber = pageNumber; // ������������� ����� ��������
    tlb[tlbPointer].frameNumber = frameNumber; // ������������� ����� �����
    tlbPointer = (++tlbPointer) % TLB_SIZE; // ��������� ��������� TLB � ������
}

// ������� ��� ��������� ��������� �����
int getFreeFrame()
{
    int freeFrame = -1; // ���������� ������������, ��� ��������� ����� ���
    for (int i = 0; i < NUM_FRAMES; i++)
    {
        if (freeFrames[i] == 0) // ���� ����� ��������
        {
            freeFrame = i; // ���������� ����� ��������� �����
            freeFrames[i] = 1; // �������� �� ��� �������
            break; // ��������� ����, ��������� ��������� ����� �������
        }
    }
    return freeFrame; // ���������� ����� ��������� ����� ��� -1, ���� ����� ���
}

// ������� ��� �������������� ������������ ������ � ����������
int translateAddress(int virtualAddress)
{
    int pageNumber = (virtualAddress >> 8) & 0xFF; // ��������� ����� �������� �� ������������ ������
    int offset = virtualAddress & 0xFF; // ��������� �������� �� ������������ ������
    int frameNumber;

    // ��������� TLB �� ������� ������������
    for (int i = 0; i < TLB_SIZE; i++)
    {
        if (tlb[i].pageNumber == pageNumber) // ���� �� ����� �������� � TLB
        {
            frameNumber = tlb[i].frameNumber; // �������� ����� ����� �� TLB
            tlbHitCount++; // ����������� ������� ��������� � TLB
            return frameNumber * FRAME_SIZE + offset; // ���������� ���������� �����
        }
    }

    // ���� �������� ��� � TLB, ��������� ������� �������
    if (pageTable[pageNumber] == -1)
    {
        int frameNumber = getFreeFrame(); // ���� ��������� �����
        if (frameNumber == -1) // ���� ��������� ����� ���
        {
            frameNumber = fifoPointer; // ���������� ����� �� FIFO
        }
        pageTable[pageNumber] = frameNumber; // ��������� ������ � ������� �������
        readFromBackingStore(pageNumber); // ������ �������� �� ����� ��������
        fifoPointer = (++fifoPointer) % NUM_FRAMES; // ��������� ��������� FIFO
    }

    frameNumber = pageTable[pageNumber]; // �������� ����� ����� �� ������� �������
    updateTlb(pageNumber, frameNumber); // ��������� TLB � ����� �������

    return frameNumber * FRAME_SIZE + offset; // ���������� ���������� �����
}

int main()
{
    FILE* addressesFile = fopen("addresses.txt", "r"); // ��������� ���� � ������������ ��������
    FILE* outputFile = fopen("output.txt", "w+"); // ��������� ���� ��� ������ �����������

    // ���������, ������� �� ������� �����
    if (addressesFile == NULL || outputFile == NULL)
    {
        perror("Error opening file"); // ���� ���� �� ������ �� ������� �������, ������� ��������� �� ������
        return -1;
    }

    // �������������� TLB
    for (int i = 0; i < TLB_SIZE; i++)
    {
        tlb[i].pageNumber = -1; // ������������� ����� �������� ��� -1 (�������� �������)
        tlb[i].frameNumber = -1; // ������������� ����� ����� ��� -1 (�������� �������)
    }

    // �������������� ������� �������
    for (int i = 0; i < PAGE_TABLE_SIZE; i++)
    {
        pageTable[i] = -1; // ��� �������� ���������� ����������� (�������� -1)
    }

    // �������������� ������ ��������� �����
    for (int i = 0; i < NUM_FRAMES; i++)
    {
        freeFrames[i] = 0; // ������� ��� ����� �������� ��� ���������
    }

    int total = 0; // ����� ������� ����������� �������
    int virtualAddress; // ���������� ��� �������� �������� ������������ ������
    // ������ ����������� ������ �� �����
    while (fscanf(addressesFile, "%d", &virtualAddress) != EOF)
    {
        int physicalAddress = translateAddress(virtualAddress); // ����������� ����������� ����� � ����������
        char value = physicalMemory[physicalAddress / FRAME_SIZE][physicalAddress % FRAME_SIZE] & 0xFF; // �������� �������� �� ����������� ������
        fprintf(outputFile, "Virtual address: %d Physical address: %d Value: %d", virtualAddress, physicalAddress, value); // ���������� ��������� � �������� ����
        total++; // ����������� ����� �������
    }

    // ��������� ������� ������ ������� � ������� ��������� � TLB
    double pageFault_rate = (double)fifoPointer / total; // ����� ������ ������� ����� �� ����� ���������� �������
    double tlbHit_rate = (double)tlbHitCount / total; // ����� ��������� � TLB ����� �� ����� ���������� �������

    // ������� ���������� �� �����
    printf("Page fault rate: %.2f%%\n", pageFault_rate * 100);
    printf("TLB hit rate: %.2f%%\n", tlbHit_rate * 100);

    fclose(addressesFile); // ��������� ���� � ��������
    fclose(outputFile); // ��������� �������� ����
    return 0; // ��������� ���������
}