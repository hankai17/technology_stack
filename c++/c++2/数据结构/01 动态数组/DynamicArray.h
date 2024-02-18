#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include<stdlib.h>
#include<string.h>

//��̬����ṹ��
typedef struct _DYNAMICARRAY{
	void** address;
	int capacity;
	int size;
}DArray;

typedef void* DynamicArray;
//�ȽϺ���ָ��
typedef int(DATACOMPARE)(void*,void*);
//��ӡ����ָ��
typedef void(DATAPRINT)(void*);

//��ʼ������
DynamicArray Init_DynamicArray();
//ָ��λ�ò���
void Insert_DynamicArray(DynamicArray darray,int pos, void* data);
//β���������
void PushBack_DynamicArray(DynamicArray darray, void* data);
//ͷ���������
void PushFront_DynamicArray(DynamicArray darray, void* data);
//ָ��λ��ɾ��
void RemoveByPos_DynamicArray(DynamicArray darray, int pos);
//β��ɾ������
void PopBack_DynamicArray(DynamicArray darray);
//ͷ��ɾ������
void PopFront_DynamicArray(DynamicArray darray);
//ֵɾ��
void RemoveByVal_DynamicArray(DynamicArray darray, void* data, DATACOMPARE compare);
//���ָ��λ��Ԫ��
void* Get_DynamicArray(DynamicArray darray,int pos);
//�����С
int Size_DynamicArray(DynamicArray darray);
//��������
int Capacity_DynamicArray(DynamicArray darray);
//����
void Print_DynamicArray(DynamicArray darray,DATAPRINT print);
//��������
void Destroy_DynamicArray(DynamicArray darray);


#endif