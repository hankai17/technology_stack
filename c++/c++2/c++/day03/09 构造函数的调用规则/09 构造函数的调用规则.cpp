#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Myclass{
public:
	Myclass(){
	
	}
	//1. ����ṩ�������캯������ô��������������ṩ�κε�Ĭ�Ϲ��캯��
	Myclass(const Myclass& mclass){
	
	}
};

class Myclass2{
public:
	Myclass2(){

	}

	//�ṩĬ�ϵĿ�������
};

int main(){

	Myclass mclass1;
	Myclass mlcass2 = mclass1;

	Myclass2 mclass3;
	Myclass2 mclass4 = mclass3;

	system("pause");
	return EXIT_SUCCESS;
}