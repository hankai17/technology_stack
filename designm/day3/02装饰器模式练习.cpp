#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
class Hero
{
public:
	virtual void status() = 0;
};
class Akali :public Hero
{
public:
	virtual void status()  
    {
		cout << "HP: 1000" << endl;
		cout << "AP: 500" << endl;
		cout << "AD: 50" << endl;
	}
};
//Ӣ�۵�װ����
class Decorator :public Hero
{
public:
	Decorator(Hero *hero)
    {
		this->hero = hero;
	}
	virtual void status() = 0;
protected:
	Hero *hero;
};
//���׶����װ����
class RYDecorator :public Decorator
{
public:
	RYDecorator(Hero *hero) : Decorator(hero) {}
	virtual void status()  
    {
		this->hero->status(); //�ȵ��ñ�װ�ε� Ӣ�۵Ļ���״̬
		cout << "HP + 100000" << endl;
	}
};
//��ԨȨ�ȵ�װ��װ����
class SYQZDecorator :public Decorator
{
public:
	SYQZDecorator(Hero *hero) : Decorator(hero) {}
	virtual void status() 
    {
		this->hero->status();
		cout << "AP +50000 " << endl;
	}
};
int main(void)
{
	Hero *akali = new Akali;
	cout << "akali�ĳ�ʼ״̬" << endl;
	akali->status();
    
	cout << "ͨ����Ȼ���񴴽��µ�akali" << endl;
	Hero *ryAkali = new RYDecorator(akali);
	ryAkali->status();

	cout << "��ͨ�� ��ԨȨ�ȵ�װ��װ���� ��������akali" << endl;
	Hero *syAkali = new SYQZDecorator(ryAkali);
	syAkali->status();

	return 0;
}
