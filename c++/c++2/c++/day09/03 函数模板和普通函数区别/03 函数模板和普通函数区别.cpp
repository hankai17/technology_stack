#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

template<typename T>
void MyPlus1(T a,T b){
	cout << a + b << endl;
}

void MyPlus2(int a,int b){
	cout << a + b << endl;
}


int main(){

	int a = 10;
	char b = 'a';

	MyPlus1(a, a);
	MyPlus2(a, a);

	//����ģ�岻�ܽ����Զ�����ת��������Ҫ�ϸ�ƥ������
	//MyPlus1(a, b);
	//�Զ�����ת��
	MyPlus2(a, b);


	system("pause");
	return EXIT_SUCCESS;
}