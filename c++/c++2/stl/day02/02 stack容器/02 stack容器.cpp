#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<stack>
using namespace std;

//1. stack����
void test01(){

	//����ջ���� ջ������������Ƚ����
	stack<int> s;
	stack<int> s2;
	s2 = s;

	//��ջ�����Ԫ�أ����� ѹջ ��ջ
	s.push(10);
	s.push(20);
	s.push(30);

	while (s.size() > 0){
		//���ջ��Ԫ��
		cout << s.top() << " ";
		//����ջ��Ԫ��
		s.pop();
	}
	cout << "size:" << s.size() << endl;
}

int main(){

	test01();


	system("pause");
	return EXIT_SUCCESS;
}