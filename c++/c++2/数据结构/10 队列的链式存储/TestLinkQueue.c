#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"LinkQueue.h"

typedef struct _PERSON{
	QueueNode node;
	char name[64];
	int age;
}Person;

int main(){


	//��ʼ������
	LinkQueue queue = Init_LinkQueue();
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
	//���
	Push_LinkQueue(queue, (void*)&p1);
	Push_LinkQueue(queue, (void*)&p2);
	Push_LinkQueue(queue, (void*)&p3);
	Push_LinkQueue(queue, (void*)&p4);
	Push_LinkQueue(queue, (void*)&p5);
	Push_LinkQueue(queue, (void*)&p6);
	//��ӡ��ͷ ��βԪ��
	Person* front = (Person*)Front_LinkQueue(queue);
	printf("Name:%s Age:%d\n", front->name, front->age);
	Person* back = (Person*)Back_LinkQueue(queue);
	printf("Name:%s Age:%d\n", back->name, back->age);
	printf("----------------------------\n");
	//���
	while (Size_LinkQueue(queue) > 0){
		//��ö�ͷԪ��
		front = (Person*)Front_LinkQueue(queue);
		printf("Name:%s Age:%d\n", front->name, front->age);
		//���Ӳ���
		Pop_LinkQueue(queue);
	}

	//���ٶ���
	Destroy_LinkQueue(queue);


	system("pause");
	return EXIT_SUCCESS;
}