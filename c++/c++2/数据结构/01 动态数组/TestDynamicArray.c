#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"DynamicArray.h"

typedef struct _PERSON{
	char name[64];
	int age;
}Person;

//��ӡ����
void MyPrint(void* data){
	Person* p = (Person*)data;
	printf("Name:%s Age:%d\n",p->name,p->age);
}

//�ȽϺ���
int MyCompare(void* data1,void* data2){
	Person* p1 = (Person*)data1;
	Person* p2 = (Person*)data2;
	return strcmp(p1->name, p2->name) == 0 && p1->age == p2->age;
}

//���Զ�̬����
void test01(){

	//��ʼ������
	DynamicArray darray = Init_DynamicArray();
	Person p1 = { "aaa", 12 };
	Person p2 = { "bbb", 13 };
	Person p3 = { "ccc", 14 };
	Person p4 = { "ddd", 15 };
	Person p5 = { "eee", 16 };
	Person p6 = { "fff", 17 };

	//β��
	PushBack_DynamicArray(darray, &p1);
	PushBack_DynamicArray(darray, &p2);
	//ͷ��
	PushFront_DynamicArray(darray, &p3);
	PushFront_DynamicArray(darray, &p4);
	//ָ��λ�ò���
	Insert_DynamicArray(darray, 3, &p5);
	Insert_DynamicArray(darray, 3, &p6);
	//��ӡ
	Print_DynamicArray(darray, MyPrint);
	//���ָ��λ��Ԫ��
	Person* person = (Person*)Get_DynamicArray(darray, 3);
	printf("ָ��λ��Ԫ��: Name:%s Age:%d\n",person->name,person->age);
	//��ӡ�����ʹ�С
	printf("��С:%d\n", Size_DynamicArray(darray));
	printf("����:%d\n", Capacity_DynamicArray(darray));
	//ͷɾ
	PopFront_DynamicArray(darray);
	printf("ͷɾ����:\n");
	Print_DynamicArray(darray, MyPrint);
	printf("-----------------\n");
	//βɾ
	PopBack_DynamicArray(darray);
	printf("βɾ����:\n");
	Print_DynamicArray(darray, MyPrint);
	printf("-----------------\n");
	//ָ��λ��ɾ
	RemoveByPos_DynamicArray(darray,2);
	printf("ɾ��2λ�ý��:\n");
	Print_DynamicArray(darray, MyPrint);
	printf("-----------------\n");
	//����ֵɾ
	RemoveByVal_DynamicArray(darray, (void*)&p1, MyCompare);
	printf("����ֵɾ�����:\n");
	Print_DynamicArray(darray, MyPrint);
	printf("-----------------\n");
	//��������
	Destroy_DynamicArray(darray);
	darray = NULL;
	

}

int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}