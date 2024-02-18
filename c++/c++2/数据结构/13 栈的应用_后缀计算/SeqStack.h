#ifndef SEQSTACK_H
#define SEQSTACK_H

#include<stdlib.h>

#define MAXSIZE 1024

typedef struct _SEQSTACK{
	void* data[MAXSIZE];
	int size;
}SStack;

typedef void* SeqStack;

//��ʼ��
SeqStack Init_SeqStack();
//ѹջ
void Push_SeqStack(SeqStack stack, void* data);
//��ջ
void Pop_SeqStack(SeqStack stack);
//���ջ��Ԫ��
void* Top_SeqStack(SeqStack stack);
//��С
int Size_SeqStack(SeqStack stack);
//����ջ
void Destroy_SeqStack(SeqStack stack);


#endif