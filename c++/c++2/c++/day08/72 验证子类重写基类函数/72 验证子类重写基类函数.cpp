#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Animal{
public:
	virtual void func1(){
		cout << "���ǻ����func1!" << endl;
	}
	virtual void func2(){
		cout << "���ǻ����func2!" << endl;
	}
};

class Dog : public Animal{
public:
	virtual void func1(){
		cout << "���������func1!" << endl;
	}
	virtual void func3(){
		cout << "���������func3!" << endl;
	}
};


int main(){

	Animal* animal = new Dog;
	animal->func1();

	system("pause");
	return EXIT_SUCCESS;
}