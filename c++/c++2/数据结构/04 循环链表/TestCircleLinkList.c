#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"CircleLinkList.h"
typedef struct _PERSON{
	CircleLinkNode node;
	char name[64];
	int age;
}Person;


void MyPrint(CircleLinkNode* data){
	Person* p = (Person*)data;
	printf("Name:%s Age:%d\n", p->name, p->age);
}

int MyCompare(CircleLinkNode* data1, CircleLinkNode* data2){
	Person* p1 = (Person*)data1;
	Person* p2 = (Person*)data2;
	return strcmp(p1->name, p2->name) == 0 && p1->age == p2->age;
}

int main(){

	//��ʼ������
	CircleLinkList list = Init_CircleLinkList();
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

	//ͷ��
	PushFront_CircleLinkList(list, (CircleLinkNode*)&p1);
	PushFront_CircleLinkList(list, (CircleLinkNode*)&p2);
	//β��
	PushBack_CircleLinkList(list, (CircleLinkNode*)&p3);
	PushBack_CircleLinkList(list, (CircleLinkNode*)&p4);
	//ָ��λ�ò���
	Insert_CircleLinkList(list, 1, (CircleLinkNode*)&p5);
	Insert_CircleLinkList(list, 1, (CircleLinkNode*)&p6);
	//��ӡ
	Print_CircleLinkList(list, MyPrint);
	//ͷɾ
	PopFront_CircleLinkList(list);
	printf("ͷɾ���:\n");
	Print_CircleLinkList(list, MyPrint);
	printf("---------------------\n");
	//βɾ
	PopBack_CircleLinkList(list);
	printf("βɾ���:\n");
	Print_CircleLinkList(list, MyPrint);
	printf("---------------------\n");
	//ָ��λ��ɾ��
	RemoveByPos_CircleLinkList(list, 1);
	printf("ָ��λ��1ɾ���:\n");
	Print_CircleLinkList(list, MyPrint);
	printf("---------------------\n");
	printf("������:%d\n", Size_CircleLinkList(list));
	//����ֵɾ��
	RemoveByVal_CircleLinkList(list, (CircleLinkNode*)&p3, MyCompare);
	printf("����ֵɾ���:\n");
	Print_CircleLinkList(list, MyPrint);
	printf("---------------------\n");
	//��������
	Destroy_CircleLinkList(list);


	system("pause");
	return EXIT_SUCCESS;
}