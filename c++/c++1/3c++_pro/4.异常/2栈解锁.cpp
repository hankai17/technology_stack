#include <iostream>
using namespace std;

class Test3
{
public:
	Test3(int a = 0, int b = 0)
	{
		this->a = a;
		this->b = b;
	}
	~Test3()
	{
		cout << "��������\n" << endl;
	}
private:
	int a;
	int b;
};

void myDivide()//void myDivide() throw(int,char,char *)//�����ӳ�int char char *���͵��쳣
               //void myDivide() //�����׳��κ������쳣
			   //void myDivide() throw()//���׳��쳣     ������������������������������쳣�ӿ�����
{
	Test3 t1(1, 2), t2(3, 4);//��������ջ���� �������쳣ʱ �����
	cout << "mydivide..Ҫ�����쳣\n";
	throw 1;
}


void main()
{
	try
	{
		myDivide();
	}
	catch (int a)
	{
		cout << "int �����쳣\n";
	}
	catch (...)
	{
		cout << "δ֪�쳣\n";
	}

	system("pause");
	return;
}

