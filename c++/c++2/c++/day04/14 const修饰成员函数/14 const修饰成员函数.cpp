#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Person{
public:
	Person(){
		mID = 0;
		mAge = 0;
 	}

	//thisָ�� const Person* const this;
	//��Ա���������const��ʾ����ֻ��ȥʹ�ö�������ݣ�����ȥ�޸Ķ��������
	void PrintPerson() const{
		//this->mID = 10;
		//this->mAge = 20;
	}

public:
	//mutable ���ڱ䶯�� ���׸ı�
	mutable int mID;
	int mAge;
};


//const���ζ���
void test01(){

	const Person person;
	//���������޸���ͨ�ĳ�Ա�����������޸�mutable���εĳ�Ա����
	//person.mAge = 100;
	person.mID = 100;

	//������ֻ�ܵ���const���εĺ���
	person.PrintPerson();


}
int main(){



	system("pause");
	return EXIT_SUCCESS;
}