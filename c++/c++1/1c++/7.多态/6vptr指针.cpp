#include <iostream>
using namespace std;
//��̬�������������� �̳� �麯����д ����ָ��ָ���������
class Parent
{
public:
	Parent(int a = 0)
	{
		this->a = a;
		print();//���ĸ���  ��Ȼ�ڳ�ʼ������ �����õ��Ǹ���� vptr�ֲ���ʼ��
	}
	virtual void print()//�麯����д
	{
		cout << "���ǵ�" << endl;
	}
protected:
private:
	int a;
};

class Child :public Parent
{
public:
	Child(int a = 0, int b = 0) :Parent(a)
	{
		this->b = b;
	}
	virtual void print()//�麯����д �ɼӿɲ���
	{
		cout << "���Ǻ���" << endl;
	}
protected:
private:
	int b;
};

void HowToPlay(Parent * base)
{
	base->print();
}

void main()
{
	Child c1;
	//c1.print();

	system("pause");
	return;
}