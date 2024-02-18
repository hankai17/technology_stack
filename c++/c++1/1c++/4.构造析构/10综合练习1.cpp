#include <iostream>
using namespace std;

class ABCD
{
public:
	ABCD(int a,int b,int c)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		printf("ABCD���캯����a%d ,b%d ,c%d \n", this->a, this->b, this->c);
	}
	~ABCD()
	{
		printf("ABCD����������a%d ,b%d ,c%d \n", this->a, this->b, this->c);
	}
	int getA()
	{
		return this->a;
	}
private:
	int a;
	int b;
	int c;

};
class MyE
{
public:
	MyE() :abcd1(1, 2, 3), abcd2(4, 5, 6), m(100)
	{
		cout << "MyD()"<<endl;
	}
	~MyE()
	{
		cout << "~MyD" << endl;
	}
	MyE(const MyE & obj) :abcd1(7, 8, 9), abcd2(10, 11, 12), m(100)
	{
		printf("MyD(const MyD & obj)\n");

	}

public:
	ABCD abcd1;
	ABCD abcd2;
	const int m;
};

int doThing(MyE mye1)
{
	printf("doThing() mye1.abcd1.a:%d\n", mye1.abcd1.getA());
	return 0;
}

int run2()
{
	MyE myE;
	doThing(myE);
	return 0;
}
int run3()
{
	printf("run3 start..\n");
	printf("run3 end\n");
	return 0;
}
void main()
{
	run2();
	system("pause");
	return;
}
/*
����mye�Ĺ��캯�� �������abcd ������ִ��MyE() :abcd1(1, 2, 3), abcd2(4, 5, 6), m(100)
printf("ABCD���캯����a%d ,b%d ,c%d \n", this->a, this->b, this->c); 123
printf("ABCD���캯����a%d ,b%d ,c%d \n", this->a, this->b, this->c) 456
cout << "MyD()"<<endl;

ʵ�γ�ʼ���β� ���ÿ������캯��MyE(const MyE & obj) :abcd1(7, 8, 9), abcd2(10, 11, 12), m(100)
printf("ABCD���캯����a%d ,b%d ,c%d \n", this->a, this->b, this->c);789
printf("ABCD���캯����a%d ,b%d ,c%d \n", this->a, this->b, this->c);10 11 12
printf("MyD(const MyD & obj)\n");�������캯������
�βγ�ʼ�����

printf("doThing() mye1.abcd1.a:%d\n", mye1.abcd1.getA()); 7
��Ĺ��캯�� ���

-------------��ʼ����
cout << "~MyD" << endl;
10 11 12
7 8 9
cout << "MyD()"<<endl;
456
123



*/