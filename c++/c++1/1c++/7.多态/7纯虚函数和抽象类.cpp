#include <iostream>
using namespace std;

class Figure//������  ���ܱ�ʵ����
{
public:
	//Լ��һ��ͳһ�ӿڣ����棩 ���������ʵ��
	virtual void getArea() = 0;//���麯�� Ŀ�������Ӻ����̳�
protected:
private:

};


class Circle :public Figure
{
public:
	Circle(int a, int b)
	{
		this->a = a;
		this->b = b;
	}
	virtual void getArea()
	{
		cout << "Բ�������" << 3.14*a*a << endl;
	}

private:
	int a;
	int b;
};

class Tri :public Figure
{
public:
	Tri(int a, int b)
	{
		this->a = a;
		this->b = b;
	}
	virtual void getArea()
	{
		cout << "�����ε������" << 0.5*a*b << endl;
	}

private:
	int a;
	int b;
};

class Square :public Figure
{
public:
	Square(int a, int b)
	{
		this->a = a;
		this->b = b;
	}
	virtual void getArea()
	{
		cout << "�����ε������" << a*b << endl;
	}

private:
	int a;
	int b;
};

void objplay(Figure * base)
{
	base->getArea();

}
void main()
{
	//Figer f;���ܱ�ʵ����
	//Figure * base = NULL;//�ɶ���ָ��
	Circle c1(10,20);//c1.getArea();//û���ö�̬
	Tri t1(20, 30);
	Square s1(3, 4);

	//����������� ����һ��Ԥ�ȶ���õĽӿڱ��  �����  ģ��Ļ���

	objplay(&c1);
	objplay(&t1);
	objplay(&s1);
	system("pause");
	return;
}