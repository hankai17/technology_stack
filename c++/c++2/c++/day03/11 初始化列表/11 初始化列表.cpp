#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;

class Car{
public:
	Car(string name){
		cout << "Car���캯��!" << endl;
		mName = name;
	}
	void run(){
		cout << "��������..." << endl;
	}
	~Car(){
		cout << "Car��������!" << endl;
	}
public:
	string mName;
};

class Tuolaji{
public:
	Tuolaji(string name){
		cout << "Tuolaji���캯��!" << endl;
		mName = name;
	}
	void run(){
		cout << "����������..." << endl;
	}
	~Tuolaji(){
		cout << "Tuolaji��������!" << endl;
	}
public:
	string mName;
};

class Person{
public:
	Person(string CarName, string TLJName, int a) : mTuolaji(TLJName), car(CarName), a(a){
		cout << "Person���캯��!" << endl;
	}
	void GoToWork(){
		car.run();
		cout << "��������" << car.mName << "ȥ�ϰ�..." << endl;
	}
	~Person(){
		cout << "Person��������!" << endl;
	}
public:
	Car car;
	Tuolaji mTuolaji;
	int a;
};

void test(){
	Person person("������","������",10);
	person.GoToWork();
}

int main(){

	test();

	system("pause");
	return EXIT_SUCCESS;
}