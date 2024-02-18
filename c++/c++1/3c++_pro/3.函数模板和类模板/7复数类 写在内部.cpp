#include <iostream>
using namespace std;

template <typename T>
class Complex
{
	friend ostream & operator << (ostream &out, Complex &c3)
	{
		out << "a:" << c3.a << "b:" << c3.b << endl;
		return out;
	}
	
public:
	Complex(T a , T b )
	{
		this->a = a;
		this->b = b;
	}
	void printCom()
	{
		cout << "a:" << a << "b:" << endl;
	}
	Complex operator+ (Complex &c2)//������������������������������������������д���棻����ϰ�᣻LKJHGFMN   
	{
		Complex tmp(a + c2.a , b + c2.b);
		return tmp;
	}
	
protected:
private:
	T a;
	T b;

};


/*
ostream & operator << (ostream &out, Complex &c3)//���к�����д���ڲ�

{
	out << "a:" << c3.a << "b:" << c3.b << endl;
	return out;
}
*/
void main()
{
	Complex<int> c1(1, 2);//��Ҫ��ģ������廯�Ժ���ܶ������
	Complex <int>c2(3, 4);
	Complex<int> c3 = c1 + c2;//c1���������ͨ��thisָ������
	//Complex operator+(complex &c2);
	//c3.printCom();

	cout << c3 << endl;//<<>>ֻ������Ԫ���� �����������ض�д�ɳ�Ա���� ��Ҫ������Ԫ������������������������������������������
	//ostream & operator << (ostream &out, Complex &c3)//����һ��������֧����ʽ���
	//cout << c3 << endl;

	system("pause");
	return;
}

















/*
#include <iostream>
using namespace std;




class Complex
{
	friend ostream & operator << (ostream &out, Complex &c3);
public:
	Complex(int a = 0, int b = 0)
	{
		this->a = a;
		this->b = b;
	}
	void printCom()
	{
		cout << "a:" << a << "b:" << endl;
	}
	Complex operator+(Complex &c2)//������������������������������������������д���棻����ϰ�᣻LKJHGFMN   
	{
		Complex tmp(a + c2.a + b + c2.b);
		return tmp;
	}
protected:
private:
	int a;
	int b; 

};



ostream & operator << (ostream &out, Complex &c3)
{
	out << "a:" << c3.a << "b:" << c3.b << endl;
	return out;
}
void main()
{
	Complex c1(1, 2);
	Complex c2(3, 4);
	Complex c3 = c1 + c2;//c1���������ͨ��thisָ������
	//Complex operator+(complex &c2);
	c3.printCom();
	
	//cout << c3 << endl;
	//ostream & operator << (ostream &out, Complex &c3)//����һ��������֧����ʽ���
	cout << c3 << endl;

	system("pause");
	return;
}
*/