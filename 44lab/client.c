#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

// ��������� ������ ��� �������� ���������� � ������
struct data
{
    int a; // ������ �����
    int b; // ������ �����
};

// ������� ��� ���������� ��������
void add(void* param)
{
    struct data* temp;
    temp = (struct data*)param; // �������������� ��������� � ��������� �� struct data
    printf("I add two values %d and %d result = %d\n", temp->a, temp->b, temp->a + temp->b); // ����� ���������� ��������
}

// ������� ��� ���������� ���������
void sub(void* param)
{
    struct data* temp;
    temp = (struct data*)param; // �������������� ��������� � ��������� �� struct data
    printf("I sub from value %d value %d result = %d\n", temp->a, temp->b, temp->a - temp->b); // ����� ���������� ���������
}

// ������� ��� ���������� ���������
void mul(void* param)
{
    struct data* temp;
    temp = (struct data*)param; // �������������� ��������� � ��������� �� struct data
    printf("I mul two values %d and %d result = %d\n", temp->a, temp->b, temp->a * temp->b); // ����� ���������� ���������
}

// ������� ��� ���������� �������
void div(void* param)
{
    struct data* temp;
    temp = (struct data*)param; // �������������� ��������� � ��������� �� struct data
    printf("I div value %d by value %d result = %d\n", temp->a, temp->b, temp->a / temp->b); // ����� ���������� �������
}

// ������� ��� ������������� �������� �� 1000000
void inc(void* param)
{
    struct data* temp;
    temp = (struct data*)param; // �������������� ��������� � ��������� �� struct data
    for (int i = 0; temp->a < 1000000; i++) {
        printf("Inc value %d inc value %d\n", temp->a++, temp->b++); // ���������� �������� a � b � ����� ����������
    }
}

int main(void)
{
    // �������� ���������� ��� �������� ������ � ������
    struct data work, work1, work2, work3;
    work.a = 0;   // ������������� �������� ������ ���������
    work.b = 0;
    work1.a = 55; // ������������� �������� ������ ���������
    work1.b = 100;
    work2.a = 33; // ������������� �������� ������� ���������
    work2.b = 22;
    work3.a = 99; // ������������� �������� ��������� ���������
    work3.b = 77;

    // ������������� ���� �������
    pool_init();

    // �������� ����� � ��� �������
    pool_submit(&add, &work);  // ���������� ������ �������� ��� work
    pool_submit(&sub, &work);  // ���������� ������ ��������� ��� work
    // pool_submit(&mul, &work); 
    // pool_submit(&div, &work); 

    pool_submit(&add, &work1); // ���������� ������ �������� ��� work1
    pool_submit(&sub, &work1); // ���������� ������ ��������� ��� work1
    // pool_submit(&mul, &work1); 
    // pool_submit(&div, &work1); 

    // pool_submit(&add, &work2); 
    pool_submit(&sub, &work2);  // ���������� ������ ��������� ��� work2
    // pool_submit(&mul, &work2); 
    // pool_submit(&div, &work2); 

    pool_submit(&add, &work3);  // ���������� ������ �������� ��� work3
    pool_submit(&sub, &work3);  // ���������� ������ ��������� ��� work3
    // pool_submit(&mul, &work3); 
    // pool_submit(&div, &work3); 
    // // may be helpful
    // sleep(3); 

    // ���������� ������ ���� �������
    pool_shutdown();
    return 0;
}
