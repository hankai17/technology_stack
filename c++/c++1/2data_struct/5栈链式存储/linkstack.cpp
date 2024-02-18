#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "linkstack.h"
#include "linklist.h"
/*
typedef struct Teacher//�����ҵ��ڵ�
{
	LinkListNode node;
	void * item;//ջ��ҵ��ڵ�
}Teacher;
*/
typedef struct _tag_LinkStackNode//�����ҵ��ڵ�
{
	LinkListNode node;
	void * item;//ջ��ҵ��ڵ�
}TLinkStackNode;


//����ջ �൱�� ����һ�����Ա�
LinkStack * LinkStack_Create()
{

	return LinkStack_Create();
}

void LinkStack_Destory(LinkStack * stack)
{
	LinkList_Clear(stack);
	LinkList_Destroy(stack);
	
}
//���ջ �漰��ջԪ���������ڹ���
void LinkStack_Clear(LinkStack * stack)
{
	if (stack == NULL)
	{
		return;
	}
	while (LinkList_Length(stack)>0)
	{
		LinkStack_Pop(stack);//������������ͷŽڵ��ڴ�

	}


	return;
}
//��ջ�����Ԫ�� �൱ �����Ա��ͷ������Ԫ��
//void * item ջ��ҵ��ڵ�----��ת���� �����ҵ��ڵ�
int LinkStack_Push(LinkStack * stack, void * item)
{
	TLinkStackNode * tmp = NULL;
	int ret = 0;
	tmp = (TLinkStackNode *)malloc(sizeof(TLinkStackNode));
	if (tmp == NULL)
	{
		return -1;
	}
	memset(tmp, 0, sizeof(TLinkStackNode));
	tmp->item = item;

	ret=LinkList_Insert(stack,(LinkListNode *)tmp,0);
	if (ret != 0)
	{
		printf("err\n");
		if (tmp != NULL)
		{
			free(tmp);
		}
	}
	return ret;
}
//�����Ա��ͷ��ɾ��Ԫ��
void * LinkStack_Pop(LinkStack * stack)
{
	void * item = NULL;//ջ��ҵ��ڵ�
	TLinkStackNode *tmp = NULL;
	tmp=(TLinkStackNode *)LinkList_Delete(stack, 0);
	if (tmp == NULL)
	{
		return NULL;
	}
	item=tmp->item;
	//��Ϊinsertʱ��������ڴ� ����Ҫ�ͷ��ڴ�
	free(tmp);
	return item;
}

void * LinkStack_Top(LinkStack * stack)
{
	TLinkStackNode * tmp = NULL;
	tmp=(TLinkStackNode *)LinkList_Get(stack, 0);
	if (tmp == NULL)
	{
		return NULL;
	}
	return tmp->item;
}

int LinkStack_Size(LinkStack * stack)
{
	
	return LinkList_Length(stack);
}