#include <iostream>
using namespace std;

class programmer
{
public:
	virtual void getSal() = 0;
protected:
private:

};

class junior_programmer :public programmer
{
public:
	junior_programmer(char * name, char *job, int sal)
	{
		this->name = name;//����ǳ���� ��������ǳ���� ��malloc
		this->job = job;
		this->sal = sal;
	}
	virtual void getSal()
	{
		cout << name << " " << job << " " << sal << endl;

	}
private:
	char * name;
	char * job;
	int sal;

};

class mid_programmer :public programmer
{
public:
	mid_programmer(char * name, char *job, int sal)
	{
		this->name = name;//����ǳ���� ��������ǳ���� ��malloc
		this->job = job;
		this->sal = sal;
	}
	virtual void getSal()
	{
		cout << name << " " << job << " " << sal << endl;

	}
private:
	char * name;
	char * job;
	int sal;
};

class adv_programmer :public programmer
{
public:
	adv_programmer(char * name, char *job, int sal)
	{
		this->name = name;//����ǳ���� ��������ǳ���� ��malloc
		this->job = job;
		this->sal = sal;
	}
	virtual void getSal()
	{
		cout << name << " " << job << " " << sal << endl;

	}
private:
	char * name;
	char * job;
	int sal;
};

class architect_programmer :public programmer
{
public:
	architect_programmer(char * name, char *job, int sal)
	{
		this->name = name;//����ǳ���� ��������ǳ���� ��malloc
		this->job = job;
		this->sal = sal;
	}
	virtual void getSal()
	{
		cout << name << " " << job << " " << sal << endl;

	}
private:
	char * name;
	char * job;
	int sal;
};

void CalProSal(programmer * base)
{
	base->getSal();
}


void main()
{	
	adv_programmer ap("С��", "�߼�", 15000);
	mid_programmer mp("С��", "�м�", 8600);
	architect_programmer ar("������", "�ܹ�ʦ", 20000);

	CalProSal(&ap);
	CalProSal(&mp);
	CalProSal(&ar);
	system("pause");
	return;
}