#include <iostream>
using namespace std;
//��̬�������������� �̳� �麯����д ����ָ��ָ���������
class Parent
{
public:
	Parent(int a = 0)
	{
		this->a = a;
	}
	virtual void print()//�麯����д
	{
		cout << "���ǵ�" << endl;
	}
protected:
private:
	int a;
};

class Child:public Parent
{
public:
	Child(int b = 0)
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
	base->print();//���ж�̬����
}//C++��������������Ҫ���������໹�Ǹ��� ��zhe����vprָ�� �Ҹ����麯���� ���Ҹ���������ڵ�ַ

void main()
{
	Parent p1;//���ඨ������ʱ�� ������͵͵�ļ���һ��vprָ�� 
	Child c1;

	HowToPlay(&p1);
	HowToPlay(&c1);

	system("pause");
	return;
}