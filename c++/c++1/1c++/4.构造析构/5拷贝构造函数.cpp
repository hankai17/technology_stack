#include <iostream>
using namespace std;

class Location
{
public:
	Location(int xx=0,int yy=0)
	{
		X = xx; Y = yy; cout << "���캯��" << endl;
	}
	~Location()
	{
		cout << X << "," << Y << "��������" << endl;
	}
	Location(const Location & obj)
	{
		cout << "�������캯��" << endl;
		X = obj.X;
		Y = obj.Y;
	}
	int GetX()
	{
		return X;
	}
	int GetY()
	{
		return Y;
	}
private:
	int X, Y;

};

void f(Location p)//ҵ����
{
	cout << p.GetX()<< endl;

}
void playboj()
{
	Location a(1, 2);
	Location b = a;//�������캯�� a��ʼ��b

	f(b);//bʵ�γ�ʼ���β�b Ҳ����ÿ������캯��

	cout << "b��ʼ�����" << endl;
}
void main1()
{
	playboj();

	system("pause");
	return;
}

//����д���� ����������ߴ�ţ�� �ͷ���һ���¶�����������
Location g()//����һ���������� ��Ϊ�ֲ��������ز��� ���Է���һ���µ����ͼ���������
{
	Location A(1, 2);//���캯��
	return A;//f11  ���뿽�����캯��������A������󴴽�������������һ���������� �������� ����ԭ����A��������������
}                //�����ķ���ֵ��һ��Ԫ�أ��������͵ģ�  
void  objplay2()
{
	g();//���� �ᱻ����
}

void  objplay3()//��m�� ���ᱻ����ֻ����main�����ű�����
{

	Location m=g();//�����������ʼ��m ��ʱ����������������ת��m���������ˣ�
	cout << "�������󱻷��� ���ᱻ����\n" << endl;

}

void  objplay4()
{
	Location m2(1, 2);
	m2 = g();//��������ȺŸ�m2 ���ǳ�ʼ�� ����������
	cout << m2.GetX() << endl;
	cout<<m2.GetY()<<endl;
}

void main()
{
	objplay4();
	system("pause");
	return;

}
//���������ȥ��:��������������ʼ������һ��ͬ���͵Ķ��� ��ô��������ֱ��ת���������� 
//�������������ֵ����һ��ͬ���Ͷ��� ������������
