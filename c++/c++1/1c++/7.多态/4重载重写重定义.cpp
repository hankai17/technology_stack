#include <iostream>
using namespace std;

class Parent
{
public:
	virtual void func()
	{
		cout << "func() do..." << endl;
	}
	virtual void func(int i)
	{
		cout << "func() do..." << endl;
	}
	virtual void func(int i, int j)
	{
		cout << "func() do..." << endl;
	}
protected:
private:

};

class Child :public Parent
{
public:
	virtual void func(int i, int j)
	{
		cout << "func() do..." << endl;
	}
	virtual void func(int i,int j,int k )
	{
		cout << "func() do..." << endl;
	}
protected:
private:
};


void main()
{
	Child c1;
	//c1.func(); �����޷����ظ���ĺ��� ��������븸��ĺ�������ͬ �����Ƹ��ǲ��ᷢ������ 
	c1.Parent::func();//ֻ���������÷�
	c1.func(1, 2);//c++������ ����func���� ���������Ѿ������� ���Ա����������Ҹ����4��������func����
	              //������ֻ���������� ����func���� �ҵ�������func һ�������������� һ����3��������
	              //�������ø���ĺ��� ������ʾ���ü������÷���
	system("pause");
	return;
}
//���� ������ͬһ�����н���
//��д ��������֮�� һģһ���ĺ��� 1�麯����д��������̬�� 2���麯����д���ض��壩