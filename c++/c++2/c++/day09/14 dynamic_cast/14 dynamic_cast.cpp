#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//dynamic_castת�����и��ӹ�ϵ��ָ���������
//ֻ�ܽ�����ָ��ת���ɸ���ָ�룬���ܽ�����ָ��ת��������ָ��

class Animal{};
class Dog : public Animal{};
void test01(){
	
	Animal* animal = NULL;
	Dog* dog = NULL;

	//����ת�����࣬��Сת����
	//Dog* d = dynamic_cast<Dog*>(animal);

	//����ת�ɸ��࣬�Ӵ�С
	Animal* a = dynamic_cast<Animal*>(dog);

	//int a = 10;
	//char b = dynamic_cast<char>(a);

}

int main(){



	system("pause");
	return EXIT_SUCCESS;
}