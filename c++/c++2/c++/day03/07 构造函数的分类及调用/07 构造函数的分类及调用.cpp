#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;


//��������Ϊ�вι��캯�����޲ι��캯��
    //�вι��캯�����������캯�� ��  ��ͨ���캯��
class Animal{
public:
	Animal(){
		cout << "�޲εĹ��캯����" << endl;
		mName = "undefined";
		mAge = 0;
	}
	Animal(int age){
		cout << "һ���������캯��!" << endl;
		mName = "undefined";
		mAge = age;
	}
	Animal(string name){
		cout << "һ���������캯��!" << endl;
		mName = name;
		mAge = 0;
	}
	Animal(string name,int age){
		cout << "�����������캯��!" << endl;
		mName = name;
		mAge = age;
	}
	//����(����)���캯��  ��һ����������ʼ����һ������
	Animal(const Animal& animal){
		cout << "�������캯��!" << endl;
		mName = animal.mName;
		mAge = animal.mAge;
	}
	//��ӡ
	void PrintAnimal(){
		cout << "Name:" << mName << " Age:" << mAge << endl;
	}
	~Animal(){
		cout << "����������" << endl;
	}
private:
	string mName;
	int mAge;
};

//�޲ι��캯������
void test01(){

	//1. �����޲ι��캯��
	Animal animal;
	animal.PrintAnimal();

	//2. ������� ������������������
	//Animal animal2();
	//animal2.PrintAnimal();
}

//�����вι��캯��
void test02(){

#if 0
	//1. ���ŷ�
	Animal animal1("John",30);
	animal1.PrintAnimal();
	Animal animal2("John");
	Animal animal3(animal1);
	animal3.PrintAnimal();

	//2. ��ʾ���ù��캯��
	Animal("Smith",30); //���������������ڽ����ڵ�ǰ��
#endif
	Animal animal4 = Animal(30);
	animal4.PrintAnimal();

	//Animal animal5(Animal("Smith", 30));
	//Animal animal5(Animal(30)); //  Animal animal5 = Animal(30)
	//Animal animal6(Animal(animal4)); // Animal animal6 = Animal(animal4)

	//Animal(animal4); // Animal animal4;
	                  //"��������"����б������ӣ���ô��������������
					  // ����б������ӣ���ô����һ��ʵ��������

	//3. �Ⱥŷ� ֻ�����һ�������Ĺ��캯����ʹ��
	//Animal animal7 = 10; // Animal animal7 = Animal(10);

	Animal animal8 = animal4; // Animal animal7 = Animal(animal4);
}


int main(){

	//test01();
	test02();

	system("pause");
	return EXIT_SUCCESS;
}