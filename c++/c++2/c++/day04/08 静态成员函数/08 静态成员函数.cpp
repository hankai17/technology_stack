#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class MyClass{
	//3. ��̬��ԱҲ���з���Ȩ�޵�
public:
	static void ShowMyClass(){
		//1. ��̬��Ա�������ܷ��ʷǾ�̬��Ա����
		//cout << mB << endl;
	}

	void PrintMyClass(){
		//2. �Ǿ�̬��Ա�������Է��ʾ�̬��Ա�����ͷǾ�̬��Ա����
		cout << a << endl;
		cout << mB << endl;
	}
public:
	//���������������ʼ��
	static int a; //��̬��Ա����
	int mB;
};


int main(){

	//1. ��������
	MyClass::ShowMyClass();

	//2. ������
	MyClass m1;
	m1.ShowMyClass();


	system("pause");
	return EXIT_SUCCESS;
}