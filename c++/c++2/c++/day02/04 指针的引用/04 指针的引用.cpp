#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//ָ�����û����﷨
void test01(){
	int* p = NULL;
	int*& pRef = p;
}

struct Person{
	int mAge;
};


//��ָ�����ռ䲢�ҳ�ʼ��
void AllocateAndInitByPointer(Person** person){

	//����ռ�
	*person = (Person*)malloc(sizeof(Person));
	//��ʼ��
	(*person)->mAge = 100;
}

//�ͷ��ڴ�
void FreeSpaceByPointer(Person** person){
	if (*person != NULL){
		free(*person);
		*person = NULL;
	}
}

//ָ������
void AllocateAndInitByReference(Person*& person){
	person = (Person*)malloc(sizeof(Person));
	person->mAge = 200;
}

//�ͷ��ڴ�
void FreeSpaceByReference(Person*& person){
	if (person != NULL){
		free(person);
		person = NULL;
	}
}

int main(){

	Person* person = NULL;
	AllocateAndInitByPointer(&person);
	cout << "Age:" << person->mAge << endl;
	FreeSpaceByPointer(&person);
	//2. ָ���Ӧ��
	AllocateAndInitByReference(person);
	cout << "Age:" << person->mAge << endl;
	FreeSpaceByReference(person);

	system("pause");
	return EXIT_SUCCESS;
}