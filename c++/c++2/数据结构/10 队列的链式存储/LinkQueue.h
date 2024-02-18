#ifndef LINKQUEUE_H
#define LINKQUEUE_H

#include<stdlib.h>

//��ʽ���нڵ�
typedef struct _QUEUENODE{
	struct _QUEUENODE* next;
}QueueNode;


//����
typedef struct _LINKQUEUE{
	QueueNode header;
	int size;
}LQueue;

typedef void* LinkQueue;

//��ʼ��
LinkQueue Init_LinkQueue();
//���
void Push_LinkQueue(LinkQueue queue, QueueNode* data);
//����
void Pop_LinkQueue(LinkQueue queue);
//��ö�ͷԪ��
void* Front_LinkQueue(LinkQueue queue);
//��ö�βԪ��
void* Back_LinkQueue(LinkQueue queue);
//���д�С
int Size_LinkQueue(LinkQueue queue);
//���ٶ���
void Destroy_LinkQueue(LinkQueue queue);


#endif