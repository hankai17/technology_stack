#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;

class Circle{
public:
	//���ð뾶
	void setR(int r){
		mR = r;
	}
	//���㲢����Բ���ܳ�
	double caculateL(){
		return 2 * 3.14 * mR;
	}
private:
	double mR; //Բ�İ뾶
};

void test01(){
	
	//ʵ����Բ ����Բ�Ķ���
	Circle c1;
	c1.setR(10);
	cout << "Բ���ܳ���" << c1.caculateL() << endl;

	c1.setR(20);
	cout << "Բ���ܳ���" << c1.caculateL() << endl;
}


class Student{
public:
    void setName(string name){
		mName = name;
	}
	void setID(int id){
		mID = id;
	}
	void ShowStudent(){
		cout << "Name:" << mName << " Age:" << mID << endl;
	}
private:
	string mName; //ѧ������
	int mID;
};




void test02(){
	Student s1;
	s1.setName("John");
	s1.setID(1001);
	s1.ShowStudent();
}

int main(){

	//test01();
	test02();

	system("pause");
	return EXIT_SUCCESS;
}