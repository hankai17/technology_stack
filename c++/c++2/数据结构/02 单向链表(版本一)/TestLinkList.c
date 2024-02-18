#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"LinkList.h"

typedef struct _PERSON{
	char name[64];
	int age;
}Person;


void MyPrint(void* data){
	Person* p = (Person*)data;
	printf("Name:%s Age:%d\n",p->name,p->age);
}

int MyCompare(void* data1,void* data2){
	Person* p1 = (Person*)data1;
	Person* p2 = (Person*)data2;
	return strcmp(p1->name, p2->name) == 0 && p1->age == p2->age;
}

int main(){

	//��ʼ������
	LinkList list = Init_LinkList();
	//��������
	Person p1 = { "aaa", 12 };
	Person p2 = { "bbb", 13 };
	Person p3 = { "ccc", 14 };
	Person p4 = { "ddd", 15 };
	Person p5 = { "eee", 16 };
	Person p6 = { "fff", 17 };
	//ͷ��
	PushFront_LinkList(list, (void*)&p1);
	PushFront_LinkList(list, (void*)&p2);
	//β��
	PushBack_LinkList(list, (void*)&p3);
	PushBack_LinkList(list, (void*)&p4);
	//ָ��λ�ò���
	Insert_LinkList(list, 2, (void*)&p5);
	//Insert_LinkList(list, 1, (void*)&p6);
	//��ӡ
	Print_LinkList(list, MyPrint);
	//ͷɾ
	PopFront_LinkList(list);
	printf("ͷɾ���:\n");
	Print_LinkList(list, MyPrint);
	printf("---------------------\n");
	//βɾ
	PopBack_LinkList(list);
	printf("βɾ���:\n");
	Print_LinkList(list, MyPrint);
	printf("---------------------\n");
	//ָ��λ��ɾ��
	RemoveByPos_LinkList(list,1);
	printf("ָ��λ��1ɾ���:\n");
	Print_LinkList(list, MyPrint);
	printf("---------------------\n");
	printf("������:%d\n",Size_LinkList(list));
	//����ֵɾ��
	RemoveByVal_LinkList(list, (void*)&p1, MyCompare);
	printf("����ֵɾ���:\n");
	Print_LinkList(list, MyPrint);
	printf("---------------------\n");
	//��������
	Destroy_LinkList(list);

	system("pause");
	return EXIT_SUCCESS;
}