#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//1.������������
void test01(){
	int a = 10;
	char b = static_cast<char>(a);
}

//���ӹ�ϵָ���������ת��
class Animal{};
class Dog : public Animal{};
class Other{};

void test02(){

	Animal* animal = NULL;
	Dog* dog = NULL;

	//��Сת���� ����ת��
	Dog* d = static_cast<Dog*>(animal);
	Animal* a = static_cast<Animal*>(dog);

	//ת�����޹����͵�ָ��
	//Other* o = static_cast<Other*>(animal);
}


int main(){



	system("pause");
	return EXIT_SUCCESS;
}