#include <iostream>
using namespace std;

class Test
{
public: 
	Test()
	{
		a = 0;
		b = 0;
		cout << "�޲ι��캯��" << endl;
	}
	Test(int _a)
	{
		a = _a;
		b = 0;
		cout << "�вι��캯��" << endl;
	}
	void printT()
	{
		cout << "a:" << a << " b:" << b << endl;
	}
	Test(const Test & obj)
	{
		a = obj.a + 100;
		b = obj.b + 100;
		cout << "�������캯��" << endl;

	}
	~Test()
	{
		cout << "�������� �����������ڽ���ʱ c++�������Զ�����" << endl;
	}
private:
	int a;
	int b;

};
void printTest(Test t)
{
	;
}
void objmain()
{
	Test t1;//Test t2()�Ǵ���ĵ��÷���
	t1.printT();
	Test t3(1);//���ŷ�  �������Զ�����
	Test t4 = 4;//�Ⱥŷ�   �������Զ�����
	Test(2);//û�˽���������
	Test t5 = Test(5);//ֱ�ӷ�������ĳ�ʼ���� ����ת����
	Test t6 = t1;//���ÿ������캯��
	t1=Test(99);//����� ��ֵ�� ��������
	t1.printT();
	cout << "***************" << endl;
	Test t7 = Test(t6);//����ÿ������캯��

	return;
}

void objplay1()//�������캯�� �龰1,2
{
	Test t1(1);
	Test t2(t1);//�������캯�� �龰1
	Test t3 = t1;//�������캯�� �龰2
	printTest(t3);//�������캯�� �龰3  //ʵ��t3��ʼ��t ����t�������캯��
}

Test getTestObj()//����һ���� ������������
{
	Test t(1);
	return t;
}
void TestNoName()//���������龰4
{
	Test myt1 = getTestObj();//�����������ʼ����һ������
	Test myt2(1);
	myt2 = getTestObj();//�������������һ������ֵ ����������
}
void main()
{
	objmain();
	//objplay1();
	//TestNoName();
	system("pause");
	return;

}