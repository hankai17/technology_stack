#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"DoubleLinkList.h"

typedef struct _PERSON{
	DoubleLinkNode node;
	char name[64];
	int age;
}Person;

void MyPrint(DoubleLinkNode* data){
	Person* p = (Person*)data;
	printf("Name:%s Age:%d\n",p->name,p->age);
}

int main(){

	//����˫������
	DoubleLinkList dlist = Init_DoubleLinkList();
	//��������
	Person p1, p2, p3, p4, p5, p6;
	strcpy(p1.name, "aaa");
	strcpy(p2.name, "bbb");
	strcpy(p3.name, "ccc");
	strcpy(p4.name, "ddd");
	strcpy(p5.name, "eee");
	strcpy(p6.name, "fff");
	p1.age = 10;
	p2.age = 20;
	p3.age = 30;
	p4.age = 40;
	p5.age = 50;
	p6.age = 60;
	
	//β��
	PushBack_DoubleLinkList(dlist, (DoubleLinkNode*)&p1);
	PushBack_DoubleLinkList(dlist, (DoubleLinkNode*)&p2);
	//ͷ��
	PushFront_DoubleLinkList(dlist, (DoubleLinkNode*)&p3);
	PushFront_DoubleLinkList(dlist, (DoubleLinkNode*)&p4);
	//ָ��λ�ò���
	Insert_DoubleLinkList(dlist, 1, (DoubleLinkNode*)&p5);
	Insert_DoubleLinkList(dlist, 1, (DoubleLinkNode*)&p6);
	//��ӡ
	Print_DoubleLinkList(dlist, MyPrint);
	printf("�����ӡ...\n");
	RPrint_DoubleLinkList(dlist,MyPrint);
	//ͷɾ
	PopFront_DoubleLinkList(dlist);
	printf("ͷɾ����:\n");
	Print_DoubleLinkList(dlist, MyPrint);
	printf("----------------\n");
	//βɾ
	PopBack_DoubleLinkList(dlist);
	printf("βɾ����:\n");
	Print_DoubleLinkList(dlist, MyPrint);
	printf("----------------\n");
	//ָ��λ��ɾ��
	RemoveByPos_DoubleLinkList(dlist,3);
	printf("ָ��λ��ɾ����:\n");
	Print_DoubleLinkList(dlist, MyPrint);
	printf("----------------\n");
	printf("�����С:%d\n",Size_DoubleLinkList(dlist));
	//��������
	Destroy_DoubleLinkList(dlist);

	system("pause");
	return EXIT_SUCCESS;
}