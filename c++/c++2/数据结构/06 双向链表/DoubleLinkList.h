#ifndef DOUBLELINKLIST_H
#define DOUBLELINKLIST_H

#include<stdlib.h>

//����ڵ�
typedef struct _DOUBLELINKNODE{
	struct _DOUBLELINKNODE* prev;
	struct _DOUBLELINKNODE* next;
}DoubleLinkNode;

//����
typedef struct _DOUBLELINKLIST{
	DoubleLinkNode header;
	int size;
}DLinkList;

typedef void* DoubleLinkList;
//��ӡ
typedef void(*DATA_PRINT)(DoubleLinkNode*);

//��ʼ��
DoubleLinkList Init_DoubleLinkList();
//ָ��λ�ò���
void Insert_DoubleLinkList(DoubleLinkList dlist, int pos, DoubleLinkNode* data);
//ͷ���������
void PushFront_DoubleLinkList(DoubleLinkList dlist, DoubleLinkNode* data);
//β���������
void PushBack_DoubleLinkList(DoubleLinkList dlist, DoubleLinkNode* data);
//ָ��λ��ɾ��
void RemoveByPos_DoubleLinkList(DoubleLinkList dlist,int pos);
//ͷ��ɾ������
void PopFront_DoubleLinkList(DoubleLinkList dlist);
//β��ɾ������
void PopBack_DoubleLinkList(DoubleLinkList dlist);
//�����С
int Size_DoubleLinkList(DoubleLinkList dlist);
//�����ӡ����
void Print_DoubleLinkList(DoubleLinkList dlist, DATA_PRINT print);
//�����ӡ����
void RPrint_DoubleLinkList(DoubleLinkList dlist, DATA_PRINT print);
//��������
void Destroy_DoubleLinkList(DoubleLinkList dlist);


#endif