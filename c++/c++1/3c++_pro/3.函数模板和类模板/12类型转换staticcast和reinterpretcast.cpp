#include <iostream>
using namespace std;

class Animal
{
public:
	virtual void cry() = 0;
};

class Dog :public Animal
{
public:
	virtual void cry()
	{
		cout << "wang!!!" << endl;
	}
	void doHome()
	{
		cout << "���ң�" << endl;
	}

};

class Cat :public Animal
{
public:

	virtual void cry()
	{
		cout << "miaomiao" << endl;
	}
	void doThing()
	{
		cout << "׽����" << endl;
	}
};

void playObj(Animal * base)
{
	base->cry();//�̳� �麯����д ����ָ��ָ���������=������̬
	//Ҫ����ʶ���������
	//dynamic_cast//����ʱ���ʶ��
	Dog *pDog = dynamic_cast<Dog*>(base);//�ù������й���
		if (pDog != NULL)
		{
			pDog->doHome();
		}

		Cat *pCat = dynamic_cast<Cat *>(base);//������ת��С��
		if (pCat != NULL)
		{
			pCat->doThing();
		}

}

class Tree {};



void main2()
{
	Dog d1;
	Cat c1;
	Animal *pBase = NULL;
	pBase = &d1;
	
	pBase = static_cast<Animal *>(&d1);
	pBase = reinterpret_cast<Animal*>(&d1);

	
	{
		Tree t1;

		//pBase = static_cast<Animal *>(&t1);�������������ͼ��  ת����
		pBase = reinterpret_cast<Animal*>(&t1);//����ת
	}

	playObj(&d1);
	playObj(&c1);

	system("pause");
	return;
}


void main1()
{
	double Pi = 3.1415926;
	int num1 =(int) Pi;//c�е�ת��

	int num2 = static_cast<int>(Pi);//c++���Ǿ�̬����ת��
	//c�� ��ʽ����ת���ĵط�������ʹ�� static_cast<>ת��
	//char *---->int *
	char * p1 = "hello...itcast";
	int *p2 = NULL;
	//p2 = static_cast<int *>(p1);  ת������ֻ���� rein
	p2 = reinterpret_cast<int *>(p1);
	cout << p1 << endl;//%s ��Ϊ��char �����ַ�
	cout << p2 << endl;//%d ��Ϊ���ص���int  ���������ַ


	system("pause");
	return;
}
//static cast�Ǿ�̬����ת�� int ת��char
//rein cast �������ͽ���
//dynamic cast ������ת��
//const cast ȥconstֻ������

void printB(const char * p)//pָ������ݲ��ܸ�
{
	cout << p << endl;
	char * p1 = NULL;
	//p1[0] = 'Z';
	p1 = const_cast<char *>(p);//const char *------> char *
	p1[0] = 'Z';
	cout << p << endl;
}

void main()
{
	char buf[] = "aaaaaaaaaaaaa";
	char * myp = "aaaaaaaaaaaaaa";
	printB(buf);
	//printB(myp);  ��Ϊû�з����ڴ�ռ� ������֮ǰҪȷ��p��ָ����ڴ�ռ��ܱ��޸�
	system("pause");
	return;
}