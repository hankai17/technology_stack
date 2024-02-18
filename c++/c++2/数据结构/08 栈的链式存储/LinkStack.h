#ifndef LINKSTACK_H
#define LINKSTACK_H

#include<stdlib.h>

//ջ�ڵ�
typedef struct _STACKNODE{
	struct _STACKNODE* next;
}StackNode;

//ջ
typedef struct _LINKSTACK{
	StackNode header;
	int size;
}LStack;

typedef void* LinkStack;

//��ʼ��
LinkStack Init_LinkStack();
//ѹջ
void Push_LinkStack(LinkStack stack, StackNode* data);
//��ջ
void Pop_LinkStack(LinkStack stack);
//���ջ��Ԫ��
void* Top_LinkStack(LinkStack stack);
//��С
int Size_LinkStack(LinkStack stack);
//����ջ
void Destroy_LinkStack(LinkStack stack);


#endif