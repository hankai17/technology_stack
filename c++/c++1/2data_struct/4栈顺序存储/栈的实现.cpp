#include <iostream>
using namespace std;
#include "seqstack.h"
//ջ��һ����������Ա�
void main()
{
	int i = 0;
	int a[10];
	SeqStack * stack = NULL;
	stack = SeqStack_Create(10);
	if (stack == NULL)
	{
		return;
	}
	for (i = 0; i < 5; i++)
	{
		a[i] = i + 1;
		SeqStack_Push(stack,&a[i]);
	}

	printf("������ %d\n", SeqStack_Capacity(stack));
	printf("���ȣ�%d\n", SeqStack_Size(stack));
	printf("ջ��Ԫ�أ�%d\n", *(int *)(SeqStack_Size(stack))  );

	while (SeqStack_Size(stack)>0)
	{
		int tmp=*((int *)SeqStack_Pop(stack));
		printf("tmp:%d\n", tmp);
	}


	system("pause");
	return;
}