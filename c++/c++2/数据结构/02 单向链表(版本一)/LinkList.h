#ifndef LINKLIST_H
#define LINKLIST_H

#include<stdlib.h>

//����ڵ�
typedef struct _LINKNODE{
	void* data; //������
	struct _LINKNODE* next; //ָ����
}LinkNode;

//����
typedef struct _LINKLIST{
	LinkNode header; //ͷ���
	int size;//�����С
}LList;

typedef void* LinkList;
//�ȽϺ���ָ��
typedef int(DATA_COMPARE)(void*, void*);
//��ӡ����ָ��
typedef void(DATA_PRINT)(void*);

//��ʼ��
LinkList Init_LinkList();
//ָ��λ�ò���
void Insert_LinkList(LinkList list,int pos,void* data);
//ͷ���������
void PushFront_LinkList(LinkList list, void* data);
//β���������
void PushBack_LinkList(LinkList list, void* data);
//ָ��λ��ɾ��
void RemoveByPos_LinkList(LinkList list,int pos);
//ͷ��ɾ������
void PopFront_LinkList(LinkList list);
//β��ɾ������
void PopBack_LinkList(LinkList list);
//ֵɾ��
void RemoveByVal_LinkList(LinkList list, void* data, DATA_COMPARE compare);
//���ָ��λ��Ԫ��
void* Get_LinkList(LinkList list,int pos);
//�����С
int Size_LinkList(LinkList list);
//��������
void Print_LinkList(LinkList list, DATA_PRINT print);
//��������
void Destroy_LinkList(LinkList list);


#endif