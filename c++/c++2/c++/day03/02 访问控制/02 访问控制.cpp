#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Daguniang{
public:
	void test(){
		mName;
		mMoney;
		mAge;
	}
public:
	string mName; //��������ֺ�����ϣ��������֪��
protected:
	int mMoney; //�����֪���ɶ���Т˳���Լ��ж���Ǯֻ���ö���֪��
private:
	int mAge; //���䲻Ҫ����֪��
};

void test01(){

	//����public,private��protected������ڲ����ɷ��ʣ�û������
	//��������ⲿ��public�ɷ��ʣ�protected ��private���ɷ���
	Daguniang bigGirl;
	bigGirl.mName;
	//bigGirl.mAge;
	//bigGirl.mMoney;
}

class Daguniangdeerzi : public Daguniang{
public:
	void Erzi(){
		mName;
		//mAge;
		mMoney;
	}
};

//struct��class����
struct A{
	int age;
};
class B{
	int age;
};

void test02(){

	A a;
	a.age;

	B b;
	b.age;
}

int main(){



	system("pause");
	return EXIT_SUCCESS;
}