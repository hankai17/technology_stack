#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"LinkList.h"


typedef struct _PERSON{
	LinkNode node;
	char name[64];
	int age;
}Person;

void MyPrint(LinkNode* data){
	Person* person = (Person*)data;
	printf("Name:%s Age:%d\n",person->name,person->age);
}

int MyCompare(LinkNode* data1,LinkNode* data2){
	Person* person1 = (Person*)data1;
	Person* person2 = (Person*)data2;
	return strcmp(person1->name, person2->name) == 0 && person1->age == person2->age;
}

int main(){

	//��ʼ��
	LinkList list = Init_LinkList();
	//��������
	Person p1,p2,p3,p4,p5,p6;
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
	PushFront_LinkList(list, (LinkNode*)&p1);
	PushFront_LinkList(list, (LinkNode*)&p2);
	//β��
	PushBack_LinkList(list, (LinkNode*)&p3);
	PushBack_LinkList(list, (LinkNode*)&p4);
	//ָ��λ�ò���
	Insert_LinkList(list, 2, (LinkNode*)&p5); 
	Insert_LinkList(list, 3, (LinkNode*)&p6);
	//Insert_LinkList(list, 3, (LinkNode*)&p6);

	//��ӡb a e f c d
	Print_LinkList(list, MyPrint);
	//βɾ
	PopBack_LinkList(list);
	printf("βɾ���:\n");
	Print_LinkList(list, MyPrint);
	printf("----------------\n");
	//ͷɾ
	PopFront_LinkList(list);
	printf("ͷɾ���:\n");
	Print_LinkList(list, MyPrint);
	printf("----------------\n");
	//ָ��λ��ɾ��
	RemoveByPos_LinkList(list,2);
	printf("ָ��λ��2ɾ�����:\n");
	Print_LinkList(list, MyPrint);
	printf("----------------\n");
	//ֵɾ��
	RemoveByVal_LinkList(list, (LinkNode*)&p3, MyCompare);
	printf("ɾ��cccɾ�����:\n");
	Print_LinkList(list, MyPrint);
	printf("----------------\n");


	//��������
	Destroy_LinkList(list);

	system("pause");
	return EXIT_SUCCESS;
}