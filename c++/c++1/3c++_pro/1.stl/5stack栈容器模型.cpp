#include <iostream>
using namespace std;
#include "stack"

void main51()
{
	stack<int> s;
	for (int i = 0; i < 10; i++)//��ջ
	{
		s.push(i + 1);
	}
	cout << "ջ�Ĵ�С" << s.size() << endl;

	while (!s.empty())//��ջ
	{
		int tmp = s.top();//��ȡջ��Ԫ��
		cout << tmp << endl;

		s.pop();//����ջ��Ԫ��
	}
	

}

class Teacher
{
public:
	int age;
	char name[32];
public:
	void printT()
	{
		cout << "age" << age << endl;
	}
};

void main52()
{
	Teacher t1, t2, t3;
	t1.age = 31;
	t2.age = 32;
	t3.age = 33;

	stack<Teacher> s;
	s.push(t1);
	s.push(t2);
	s.push(t3);

	while (!s.empty())
	{
		Teacher tmp = s.top();
		tmp.printT();
		s.pop();
	}
}

void main53()
{
	Teacher t1, t2, t3;
	t1.age = 31;
	t2.age = 32;
	t3.age = 33;

	stack<Teacher *> s;
	s.push(&t1);
	s.push(&t2);
	s.push(&t3);

	while (!s.empty())
	{
		Teacher *p = s.top();
		p->printT();
		s.pop();
	}

}
void main()
{
	main53();
	system("pause");
	return;
}