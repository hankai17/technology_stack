#include <iostream>
using namespace std;

//public���εĳ�Ա���� ������������ⶼ��ʹ�á�
//private:private�̳к�ֻ�������ڲ�ʹ��
//protected��protect�̳��Ժ�ֻ������ڲ�ʹ�� �ڼ̳е������п���
class Parent
{
public:
	int a;//����������
protected:
	int b;//����������
private:
	int c;//����������
public:
	void printT()
	{
		cout << "printT" << endl;
	}

};

class Child :public Parent//public�̳�
{
public:
	void useVar()
	{
		a = 0;
		b = 0;
		//c = 0;
	}
protected:
private:

};

class Child2 :private Parent//˽�м̳�
{
public:
	void useVar()
	{
		a = 0;
		b = 0;
		//c = 0;
	}
protected:
private:

};

class Child3 :protected Parent//˽�м̳�
{
public:
	void useVar()
	{
		a = 0;//protected
		b = 0;//protected
		//c = 0;
	}
protected:
private:

};

void main1()
{
	Parent t1, t2;
	t1.a = 10;
	//t1.b = 9; t1.c=0; ���Ǵ�� ����������ʹ��

	system("pause");
	return;
}
//����ԭ�򣺿�������� ��д��������ڲ������ⲿ  ����ʲô�̳�  �������� ���ʼ���

void main2()
{
	Child2 t1, t2;
	//t1.a = 10;
	
	//t1.b = 9; t1.c=0; ���Ǵ�� ����������ʹ��

	system("pause");
	return;
}

void main()
{
	Child3 t1, t2;
	//t1.a = 10;

	//t1.b = 9; t1.c=0; ���Ǵ�� ����������ʹ��

	system("pause");
	return;
}

//