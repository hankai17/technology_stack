#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Animal{
public:
	virtual void ShowAnimal() {};
	virtual ~Animal() = 0; //������������
};

Animal::~Animal(){}

class Dog : public Animal{
public:
	Dog(){
		pName = new char[strlen("Ahuang") + 1];
		strcpy(pName, "John");
	}
	virtual void ShowAnimal(){
		cout << "Name:" << pName << endl;
	}
	~Dog(){
		cout << "Dog ��������!" << endl;
		if (this->pName != NULL){
			delete[] this->pName;
			this->pName = NULL;
		}
	}
private:
	char* pName;
};

void test(){
	
	//1. �̳�
	//2. ��д������麯��
	//3. �����ָ���������ָ���������

	Animal* animal = new Dog;
	animal->ShowAnimal();
	delete animal;
}

int main(){

	test();

	system("pause");
	return EXIT_SUCCESS;
}