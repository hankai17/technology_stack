//��̳ж����ԣ�һ��������virtual�Ϳ��Խ�� ��û��������ֻ�������÷����

#include <iostream>
using namespace std;

class B
{
public:
	int b;
protected:
private:

};

class B1:virtual public B
{
public:
	int b1;
protected:
private:

};

class B2 :virtual public B
{
public:
	int b2;
protected:
private:

};

class C :public B1, public B2//��̳�
{
public:
	int c;
protected:
private:

};


/*  ���ֶ�̳в�����
void main()
{
	C myc;
	myc.c = 10;
	//myc.b=2 ����������� ����virtual�Ϳ��Թ���һ��
	myc.b = 2;
	system("pause");
	return;
}
*/