#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//new����������������
void test01(){
	int* pInt = new int[20];
	char* str = new char[20];

	delete[] pInt;
	delete[] str;
}

class Person{
public:
	Person(){
		cout << "�޲ι��캯����" << endl;
		mName = "undefined!";
		mAge = 0;
	}
	Person(string name, int age){
		cout << "2���ι��캯����" << endl;
		mName = name;
		mAge = age;
	}

	~Person(){
		cout << "����������" << endl;
	}
public:
	string mName;
	int mAge;
};

void test02(){

	//�����Զ���������飬�����ṩ�޲ι��캯��
	Person* personArr = new Person[10];
	delete personArr;
}




int main(){

	test02();



	system("pause");
	return EXIT_SUCCESS;
}