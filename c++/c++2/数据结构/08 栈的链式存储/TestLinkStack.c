#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"LinkStack.h"

typedef struct _PERSON{
	StackNode node;
	char name[64];
	int age;
}Person;

int main(){

	//��ʼ��ջ
	LinkStack stack = Init_LinkStack();
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
	//��ջ
	Push_LinkStack(stack, (StackNode*)&p1);
	Push_LinkStack(stack, (StackNode*)&p2);
	Push_LinkStack(stack, (StackNode*)&p3);
	Push_LinkStack(stack, (StackNode*)&p4);
	Push_LinkStack(stack, (StackNode*)&p5);
	Push_LinkStack(stack, (StackNode*)&p6);

	//���ջ��Ԫ��
	while (Size_LinkStack(stack) > 0){
		//���ջ��Ԫ��
		Person* person = (Person*)Top_LinkStack(stack);
		printf("Name:%s Age:%d\n", person->name, person->age);
		//��ջ
		Pop_LinkStack(stack);
	}
	//����ջ
	Destroy_LinkStack(stack);


	system("pause");
	return EXIT_SUCCESS;
}