#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include"MyArray.h"
using namespace std;

//1. []���������
void test01(){

	//��������
	MyArray marray;
	
	for (int i = 0; i < 10; i++){
		//��������[]���������
		marray[i] = i + 1;
	}

	for (int i = 0; i < 10;i++){
		cout << marray[i] << " ";
	}

}
//2. +���������
class OtherClass{
public:
	OtherClass(int param){
		mParam = param;
	}
public:
	int mParam;
};
class MyClass{
	
public:
	MyClass(int param1,int param2){
		mParam1 = param1;
		mParam2 = param2;
	}
	void ShowMyClass(){
		cout << "mParam1:" << mParam1 << " mParam2:" << mParam2 << endl;
	}
	//��Ա��������������
	MyClass operator+(const MyClass& mclass){
		int newParam1 = this->mParam1 + mclass.mParam1;
		int newParam2 = this->mParam2 + mclass.mParam2;
		cout << "��Ա��" << endl;
		MyClass temp(newParam1, newParam2);
		return temp;
	}
	//��Ա��������������
	MyClass operator+(const OtherClass& mclass){
		int newParam1 = this->mParam1 + mclass.mParam;
		int newParam2 = this->mParam2 + mclass.mParam;
		cout << "��Ա��" << endl;
		MyClass temp(newParam1, newParam2);
		return temp;
	}
public:
	int mParam1;
	int mParam2;
};

//ȫ�����������
MyClass operator+(const MyClass& class1, const MyClass& class2){
	int newParam1 = class1.mParam1 + class2.mParam1;
	int newParam2 = class2.mParam2 + class2.mParam2;
	cout << "ȫ�֣�" << endl;
	MyClass temp(newParam1, newParam2);
	return temp;
}

void test02(){
	
	MyClass class1(10, 10);
	MyClass class2(20, 20);
	MyClass mclass = class1 + class2; //class1.operator+(class2);
	

	//����2. ��ͬ����������֮�����
	OtherClass oclass(10);
	MyClass classret = class1 + oclass; // class1.operator+(oclass)
	classret.ShowMyClass();
#if 0
	//����: �����������OtherClass��ҲҪ����+�Ų�����
	MyClass classret = oclass + class1; //oclass.operator+(class1)
	classret.ShowMyClass();
#endif

}


int main(){

	//test01();
	test02();

	system("pause");
	return EXIT_SUCCESS;
}