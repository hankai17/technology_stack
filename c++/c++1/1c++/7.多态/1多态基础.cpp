//�����������������󾳽� ͻ�ƺ������� ֱ�Ӹ��ñ���д�Ĵ��� �ܼ̳и��ñ��˵Ĵ��뻹����չ����д�Ĵ���
#include <iostream>
using namespace std;

class Parent
{
public:
	Parent(int a)
	{
		this->a = a;
		cout << " Parent a"<<a << endl;
	}
	virtual void print()
	{
		cout << "Parent ��ӡ a" << endl;
	}
protected:
private:
	int a;

};
class Child :public Parent
{
public:
	Child(int b) :Parent(10)
	{
		this->b = b;
		cout << "Child b" << b << endl;
	}
	void print()
	{
		cout << "Child ��ӡ b" << endl;
	}
private:
	int b;
};

void howToPrint(Parent * base)
{
	base->print();
}

void howToPrint2(Parent & base)
{
	base.print();
}

void main()
{
	Parent *base = NULL;
	Parent p1(20);
	Child c1(30);
	/*
	base = &p1;
	base->print();//�����ӡ����

	base = &c1;//���ͼ�����ԭ��
	base->print();//ִ�и����ӡ����//�������������
	*/
	/*
	Parent &base2 = p1;//����
	base2.print();//��ӡ����
	Parent &base3 = c1;//����
	base2.print();//�Դ�ӡ����
	*/
	
	howToPrint(&p1);
	howToPrint(&c1);//����
	howToPrint2(p1);
	howToPrint2(c1);//����

	//virtual ����д�ˣ������д�ɲ�дһ��д��

	system("pause");
	return;
}
//һ���������base->print(); �ж��ر�����̬ ���Ƕ�̬ 