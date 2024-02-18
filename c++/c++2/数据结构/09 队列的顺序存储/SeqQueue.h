#ifndef SEQQUEUE_H
#define SEQQUEUE_H

#include<stdlib.h>

#define MAXSIZE 1024

typedef struct _SEQQUEUE{
	void* data[MAXSIZE];
	int size;
}SQueue;

typedef void* SeqQueue;

//��ʼ��
SeqQueue Init_SeqQueue();
//���
void Push_SeqQueue(SeqQueue queue,void* data);
//����
void Pop_SeqQueue(SeqQueue queue);
//��ö�ͷԪ��
void* Front_SeqQueue(SeqQueue queue);
//��ö�βԪ��
void* Back_SeqQueue(SeqQueue queue);
//��ö��еĳ���
int Size_SeqQueue(SeqQueue queue);
//���ٶ���
void Destroy_SeqQueue(SeqQueue queue);



#endif