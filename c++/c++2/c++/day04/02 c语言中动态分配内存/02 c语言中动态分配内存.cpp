#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;


class Person{
public:
	Person(){
		cout << "�޲ι��캯����" << endl;
		mName = "undefined!";
		mAge = 0;
	}

	Person(string name,int age){
		cout << "2���ι��캯����" << endl;
		mName = name;
		mAge = age;
	}

	~Person(){
	
	}
public:
	string mName;
	int mAge;
};

void test01(){
	Person* person = (Person*)malloc(sizeof(Person));
	if (person == NULL){
		//return 0;
	}
	//person->Init();

}

void test02(){
	

	//new�ڶ��Ϸ����ڴ棬���ҵ��ù��캯����ɳ�ʼ��
	Person* person1 = new Person;
	Person* person2 = new Person("John",20);

	//�ͷŶ��ڴ�
	delete person1;
	delete person2;
	//delete���ͷŶ��ڴ�ǰ�����һ����������
}

int main(){

	test02();


	system("pause");
	return EXIT_SUCCESS;
}