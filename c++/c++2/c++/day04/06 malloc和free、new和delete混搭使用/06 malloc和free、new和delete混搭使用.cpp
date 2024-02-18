#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Person{
public:
	Person(char* name, int age){
		cout << "���캯��!" << endl;
		pName = (char*)malloc(sizeof(name));
		strcpy(pName, name);
		mAge = age;
	}
	~Person(){
		cout << "��������!" << endl;
		if (pName != NULL){
			delete pName;
		}
	}
public:
	char* pName;
	int mAge;
};

void test01(){
	Person* person = new Person("John",30);
	free(person); //û�е�����������
}

void test02(){
	Person* person = (Person*)malloc(sizeof(Person));
	delete person;
}

int main(){

	//test01();
	test02();

	system("pause");
	return EXIT_SUCCESS;
}