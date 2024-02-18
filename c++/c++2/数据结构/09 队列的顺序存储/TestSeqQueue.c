#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"SeqQueue.h"

typedef struct _PERSON{
	char name[64];
	int age;
}Person;

int main(){


	//��ʼ������
	SeqQueue queue = Init_SeqQueue();
	//��������
	Person p1 = { "aaa", 10 };
	Person p2 = { "bbb", 20 };
	Person p3 = { "ccc", 30 };
	Person p4 = { "ddd", 40 };
	Person p5 = { "eee", 50 };
	Person p6 = { "fff", 60 };
	//���
	Push_SeqQueue(queue, (void*)&p1);
	Push_SeqQueue(queue, (void*)&p2);
	Push_SeqQueue(queue, (void*)&p3);
	Push_SeqQueue(queue, (void*)&p4);
	Push_SeqQueue(queue, (void*)&p5);
	Push_SeqQueue(queue, (void*)&p6);
	//��ӡ��ͷ ��βԪ��
	Person* front = (Person*)Front_SeqQueue(queue);
	printf("Name:%s Age:%d\n",front->name,front->age);
	Person* back = (Person*)Back_SeqQueue(queue);
	printf("Name:%s Age:%d\n", back->name, back->age);
	printf("----------------------------\n");
	//���
	while (Size_SeqQueue(queue) > 0){
		//��ö�ͷԪ��
		front = (Person*)Front_SeqQueue(queue);
		printf("Name:%s Age:%d\n", front->name, front->age);
		//���Ӳ���
		Pop_SeqQueue(queue);
	}
	
	//���ٶ���
	Destroy_SeqQueue(queue);

	system("pause");
	return EXIT_SUCCESS;
}