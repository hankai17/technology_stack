#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;


template<typename T>
void MyPlus(T a, T b){
	cout << a + b << endl;
}

template<typename T>
void MyPlus(T a, T b, T c){
	cout << a + b + c<< endl;
}

void MyPlus(int a, int b){
	cout << a + b << endl;
}


int main(){

	int a = 10;
	int b = 20;
	//1.�������ģ�����ͨ��������ƥ����ã���ô���������ȿ�����ͨ����
	MyPlus(a, b);
	//2.
	MyPlus<>(a, b); //ǿ�Ʊ��������ú���ģ��

	//3. �������ģ���ܲ������õ�ƥ�䣬��ô�ͻ���ú���ģ��
	char aa = 'a';
	char bb = 'b';
	MyPlus(aa, bb);

	//4. ����ģ��Ҳ��������ͨ������������
	MyPlus(10, 20, 30);


	system("pause");
	return EXIT_SUCCESS;
}