#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
#include"LinkStack.hpp"
#include<string>

class Person{
public:
	Person(){}
	Person(string name, int age){
		this->mName = name;
		this->mAge = age;
	}
public:
	string mName;
	int mAge;
};

void test(){
	
	//��ʼ��ջ
	LinkStack<Person> stack;
	//��������
	Person p1("aaa", 10);
	Person p2("bbb", 20);
	Person p3("ccc", 30);
	Person p4("ddd", 40);
	Person p5("eee", 50);
	Person p6("fff", 60);
	//ѹջ
	stack.Push(p1);
	stack.Push(p2);
	stack.Push(p3);
	stack.Push(p4);
	stack.Push(p5);
	stack.Push(p6);
	//���
	while (!stack.Empty()){
		//���ջ��Ԫ��
		cout << "Name:" << stack.Top().mName << " Age:" << stack.Top().mAge << endl;
		stack.Pop();
	}

}

int main(){

	test();

	system("pause");
	return EXIT_SUCCESS;
}