#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

namespace A{
	int paramA = 10;
	int paramB = 20;
	void func(){
		cout << "hello world!" << endl;
	}
}

//using����
void test01(){

	A::paramA;
	A::func();

	//2. ͵�� ��д�����÷�
	using A::func;
	//ע����ͬһ�������ڷ���������ͻ
	using A::paramA;
	func();
	//3. ������ͻ
	//int paramA = 20;
}

namespace B{
	int paramA = 10;
	int paramC = 20;
	void func1(){
		cout << "hello world!" << endl;
	}
}


//using����ָ��
void test02(){

	using namespace A;
	using namespace B;
	
	//func();

	//���ʹ��using����ָ���ô������ͻ�����������ռ�A�еı���
	//int paramA = 20;
	//cout << paramA << endl;

}

namespace VeryLongName{
	int param = 10;
}

//�������ռ�ȡ������  namespace ���� = �����ռ�����
void test03(){

	namespace shorName = VeryLongName;
	cout << shorName::param << endl;
	cout << VeryLongName::param << endl;
}
int main(){

	//test02();
	test03();

	system("pause");
	return EXIT_SUCCESS;
}