#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


typedef struct _Person{
	char name[64];
	int age;
}Person;

typedef struct _Aninal{
	char name[64];
	int age;
	int type; //��������
}Ainmal;

void PersonEat(Person* person){
	printf("%s�ڳ��˳Եķ�!\n", person->name);
}
void AnimalEat(Ainmal* animal){
	printf("%s�ڳԶ���Եķ�!\n", animal->name);
}

int main(){

	Person person;
	strcpy(person.name, "С��");
	person.age = 30;
	AnimalEat(&person);

	system("pause");
	return EXIT_SUCCESS;
}