#include <iostream>
using namespace std;
#include "vector"
#include "algorithm"

void main11()
{
	vector<int> v1;//1��������v1 ʢ��int����
	v1.push_back(1);
	v1.push_back(3);
	v1.push_back(5);//Ԫ�ؿ�����������
	v1.push_back(-1);
	// 1  3  5
	//2���������൱��һ��ָ�� 
	for (vector<int>::iterator it = v1.begin(); it != v1.end(); it++)
	{
		cout << *it << " ";
	}
	//3�㷨 �㷨�͵����������޷�����
	//int num1 = count(v1.begin(), v1.end, 3);//��ͷ��β�ı��� ����3�ĸ��� 
	//cout << "mum1:" << num1 << endl;
}

class Teacher
{
public:
	int age;
	char name[64];
public:
	void printT()
	{
		cout << "age:" << age << endl;
	}
};

void main12()
{
	Teacher t1, t2, t3;
	t1.age = 31;
	t2.age = 32;
	t3.age = 33;


	vector<Teacher> v1;//1��������v1 ʢ��Teacher����//����ʵ���� �������ͺ��㷨����Ч����
	v1.push_back(t1);
	v1.push_back(t2);
	v1.push_back(t3);//Ԫ�ؿ�����������
	
	//2���������൱��һ��ָ�� 
	for (vector<Teacher>::iterator it = v1.begin(); it != v1.end(); it++)
	{
		cout << it->age << " ";
	}
	//3�㷨 �㷨�͵����������޷�����
	//int num1 = count(v1.begin(), v1.end, 3);//��ͷ��β�ı��� ����3�ĸ��� 
	//cout << "mum1:" << num1 << endl;
}

void main13()
{
	Teacher t1, t2, t3;
	t1.age = 31;
	t2.age = 32;
	t3.age = 33;
	Teacher * p1, *p2, *p3;
	p1 = &t1;
	p2 = &t2;
	p3 = &t3;


	vector<Teacher *> v1;//��t123���ڴ��׵�ַ�ŵ�������
	v1.push_back(p1);
	v1.push_back(p2);
	v1.push_back(p3);//Ԫ�ؿ�����������
	// 1  3  5
	//2���������൱��һ��ָ�� ָ��p1 p2 p3
	for (vector<Teacher *>::iterator it = v1.begin(); it != v1.end(); it++)
	{
		cout << (*it)->age<< " ";
	}
	//3�㷨 �㷨�͵����������޷�����
	//int num1 = count(v1.begin(), v1.end, 3);//��ͷ��β�ı��� ����3�ĸ��� 
	//cout << "mum1:" << num1 << endl;
}
void main()
{
	main11(); 
	//main12();
	//main13();
	system("pause");
	return;
}