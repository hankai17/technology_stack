#include <iostream>
using namespace std;


template <typename T>//���߱�����Ҫ���ͱ���� ����T��Ҫ����
class A
{
public:
	A(T a = 0)//A(int a = 0)
	{
		this->a = a;
	}
	void printA()
	{
		cout << "a:" << a << endl;
	}
protected:
	T a;
private:

};

class B :public A<int>//ģ�������� Ҳ��Ҫ���廯ģ����  Ҫ֪������ռ���ڴ��С�Ƕ��� ֻ���������͹̶��������ܷ����ڴ�
{
public:
	B(int a = 10, int b = 20) :A<int>(a)
	{
		this->b = b;
	}
	void printB()
	{
		cout << "a:" << a << " b:" << b << endl;
	}

private:
	int b;

};
template<typename T>
class C :public A<T>
{
public:
	C(T c, T a) :A<T>(a)
	{
		this->c = c;
	}
	void printC()
	{
		cout << "c:" << c << endl;
	}

protected:
	T c;

};


void main()//��ģ��������ģ����
{
	C<int> c1(1, 2);
	c1.printC();
	system("pause");


}


void main62()
{
	B b1(1,2);
	b1.printB();
	system("pause");
}
//��ģ����������ͨ��
void UseA(A<int> &a)//�β�Ҳ�����Ǿ������ ���������ܷ����ڴ�
{
	a.printA();
}

void main61()
{
	
	//ģ�����ǳ������Ҫ�������;��廯 ���ɾ�����ඨ�����Ķ���//A a1(11);
	A<int>a1(11), a2(20), a3(30);
	a1.printA();
	UseA(a1);//��ģ������������

	system("pause");
	return;
}
