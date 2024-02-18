#ifndef LINKLIST_H
#define LINKLIST_H

#include<stdlib.h>

//����С�ڵ�
typedef struct _LINKNODE{
	struct _LINKNODE* next;
}LinkNode;

//����
typedef struct _LINKLIST{
	LinkNode header;
	int size;
}LList;

typedef void* LinkList;
typedef int(*DATA_COMPARE)(LinkNode*, LinkNode*);
typedef void(*DATA_PRINT)(LinkNode*);

//��ʼ��
LinkList Init_LinkList();
//ָ��λ�ò���
void Insert_LinkList(LinkList list, int pos, LinkNode* data);
//ͷ���������
void PushFront_LinkList(LinkList list,LinkNode* data);
//β���������
void PushBack_LinkList(LinkList list, LinkNode* data);
//ָ��λ��ɾ��
void RemoveByPos_LinkList(LinkList list,int pos);
//ͷ��λ��ɾ��
void PopFront_LinkList(LinkList list);
//β��λ��ɾ��
void PopBack_LinkList(LinkList list);
//����ֵɾ��
void RemoveByVal_LinkList(LinkList list, LinkNode* data, DATA_COMPARE compare);
//�����С
int Size_LinkList(LinkList list);
//��ӡ����
void Print_LinkList(LinkList list, DATA_PRINT print);
//��������
void Destroy_LinkList(LinkList list);


#endif