#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//����
class A{
public:
	int mA;
protected:
	int mB;
private:
	int mC;
};

//1. ����(public)�̳�
class B : public A{
public:
	void PrintB(){
		cout << mA << endl; //�ɷ��ʻ���public����
		cout << mB << endl; //�ɷ��ʻ���protected����
		//cout << mC << endl; //���ɷ��ʻ���private����
	}
};
class SubB : public B{
	void PrintSubB(){
		cout << mA << endl; //�ɷ��ʻ���public����
		cout << mB << endl; //�ɷ��ʻ���protected����
		//cout << mC << endl; //���ɷ��ʻ���private����
	}
};


//2. ˽��(private)�̳�
class C : private A{
public:
	void PrintC(){
		cout << mA << endl; //�ɷ��ʻ���public����
		cout << mB << endl; //�ɷ��ʻ���protected����
		//cout << mC << endl; //���ɷ��ʻ���private����
	}
};
class SubC : public C{
	void PrintSubC(){
		//cout << mA << endl; //���ɷ��ʻ���public����
		//cout << mB << endl; //���ɷ��ʻ���protected����
		//cout << mC << endl; //���ɷ��ʻ���private����
	}
};
void test02(){
	C c;
	//cout << c.mA << endl; //���ɷ��ʻ���public����
	//cout << c.mB << endl; //���ɷ��ʻ���protected����
	//cout << c.mC << endl; //���ɷ��ʻ���private����
}






//3. ����(protected)�̳�
class D : protected A{
public:
	void PrintD(){
		cout << mA << endl; //�ɷ��ʻ���public����
		cout << mB << endl; //�ɷ��ʻ���protected����
		//cout << mC << endl; //���ɷ��ʻ���private����
	}
};
class SubD : public D{
	void PrintD(){
		cout << mA << endl; //�ɷ��ʻ���public����
		cout << mB << endl; //�ɷ��ʻ���protected����
		//cout << mC << endl; //���ɷ��ʻ���private����
	}
};
void test03(){
	D d;
	//cout << d.mA << endl; //���ɷ��ʻ���public����
	//cout << d.mB << endl; //���ɷ��ʻ���protected����
	//cout << d.mC << endl; //���ɷ��ʻ���private����
}




//����
class A{
public:
	int mA;
protected:
	int mB;
private:
	int mC;
};

class B : public A{

protected:
	int mA;
protected:
	int mB;
};


int main(){



	system("pause");
	return EXIT_SUCCESS;
}