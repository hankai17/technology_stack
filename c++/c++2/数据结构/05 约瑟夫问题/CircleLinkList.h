#ifndef CIRCLELINKLIST_H
#define CIRCLELINKLIST_H

#include<stdlib.h>

//����ڵ�
typedef struct _CIRCLELINKNODE{
	struct _CIRCLELINKNODE* next;
}CircleLinkNode;

//����
typedef struct _CIRCLELINKLIST{
	CircleLinkNode header;
	int size;
}CLinkList;

typedef void* CircleLinkList;
//�ȽϺ���ָ��
typedef int(DATA_COMPARE)(CircleLinkNode*, CircleLinkNode*);
//��ӡ����ָ��
typedef void(DATA_PRINT)(CircleLinkNode*);

//��ʼ��
CircleLinkList Init_CircleLinkList();
//ָ��λ�ò���
void Insert_CircleLinkList(CircleLinkList clist, int pos, CircleLinkNode* data);
//ͷ���������
void PushFront_CircleLinkList(CircleLinkList clist, CircleLinkNode* data);
//β���������
void PushBack_CircleLinkList(CircleLinkList clist, CircleLinkNode* data);
//ָ��λ��ɾ��
void RemoveByPos_CircleLinkList(CircleLinkList clist,int pos);
//ͷ��ɾ������
void PopFront_CircleLinkList(CircleLinkList clist);
//β��ɾ������
void PopBack_CircleLinkList(CircleLinkList clist);
//ֵɾ��
void RemoveByVal_CircleLinkList(CircleLinkList clist, CircleLinkNode* data, DATA_COMPARE compare);
//���ָ��λ��Ԫ��
void* Get_CircleLinkList(CircleLinkList clist,int pos);
//�����С
int Size_CircleLinkList(CircleLinkList clist);
//��ӡ����
void Print_CircleLinkList(CircleLinkList clist, DATA_PRINT print);
//��������
void Destroy_CircleLinkList(CircleLinkList clist);

#endif