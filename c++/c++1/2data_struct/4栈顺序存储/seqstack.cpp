#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "seqstack.h"
#include "seqlist.h"
//����ջ �൱�� �������Ա�

SeqStack * SeqStack_Create(int capacity)
{
	return SeqList_Create(capacity);
	
}

void SeqStack_Destory(SeqStack * stack)
{
	 SeqList_Destory(stack);
}
void SeqStack_Clear(SeqStack * stack)
{
	 SeqList_Clear(stack);
}
//�൱����  ��β������Ԫ��
int SeqStack_Push(SeqStack * stack, void * item)
{
	return SeqList_Insert(stack, item, SeqList_Length(stack));//β�巨
	
}
//�൱��β���ó�Ԫ��
void * SeqStack_Pop(SeqStack * stack)
{

	return SeqList_Delete(stack, SeqList_Length(stack) - 1);
	
}
//�൱��ȡ����β��Ԫ��
void * SeqStack_Top(SeqStack * stack)
{
	return SeqList_Get(stack, SeqList_Length(stack) - 1);
}

int SeqStack_Size(SeqStack * stack)
{
	return SeqList_Length(stack);
}

int SeqStack_Capacity(SeqStack * stack)
{
	return SeqList_Capacity(stack);
}