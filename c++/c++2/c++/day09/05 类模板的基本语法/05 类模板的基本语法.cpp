#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

template<class T>
class MyClass{
private:
	T* pAddress; //ָ��һ������ռ�
};

int main(){

	//ʹ����ģ��
	MyClass<int> mclass; //������ʾָ�����ͣ������Զ������Ƶ�

	system("pause");
	return EXIT_SUCCESS;
}