#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
#include"LinkList.hpp"
#include<string>

class Person{
public:
	Person(){}
	Person(string name,int age){
		this->mName = name;
		this->mAge = age;
	}
	bool operator==(const Person& p){
		return p.mName == this->mName && p.mAge == this->mAge;
	}
public:
	string mName;
	int mAge;
};

void print(Person* p){
	cout << "Name:" << p->mName << " Age:" << p->mAge << endl;
}
struct print02{
	void operator()(Person& p){
		cout << "Name:" << p.mName << " Age:" << p.mAge << endl;
	}
};

void test01(){

	//��ʼ������
	LinkList<Person> list;
	//��������
	Person p1("aaa", 10);
	Person p2("bbb", 20);
	Person p3("ccc", 30);
	Person p4("ddd", 40);
	Person p5("eee", 50);
	Person p6("fff", 60);
	//ͷ����������
	list.Push_Front(p1);
	list.Push_Front(p2);
	//β��
	list.Push_Back(p3);
	list.Push_Back(p4);
	//ָ��λ�ò���
	list.Insert(2, p5);
	list.Insert(2, p6);
	//��ӡ
	//list.Print(print);
	list.Print(print02());
	//ͷɾ
	list.Pop_Front();
	cout << "ͷɾ���:" << endl;
	list.Print(print02());
	cout << "---------" << endl;
	//βɾ
	list.Pop_Back();
	cout << "βɾ���:" << endl;
	list.Print(print02());
	cout << "---------" << endl;
	//ָ��λ��ɾ��
	list.RemoveByPos(2);
	cout << "ָ��ɾ��λ��2���:" << endl;
	list.Print(print02());
	cout << "---------" << endl;
	//ֵɾ��
	list.RemoveByVal(p6);
	cout << "ֵɾ�����:" << endl;
	list.Print(print02());
	cout << "---------" << endl;
	cout << "Size:" << list.Size() << endl;
	
}

void test02(){
	//��ʼ������
	LinkList<Person*> list;
	//��������
	Person p1("aaa", 10);
	Person p2("bbb", 20);
	Person p3("ccc", 30);
	Person p4("ddd", 40);
	Person p5("eee", 50);
	Person p6("fff", 60);
	//ͷ����������
	list.Push_Front(&p1);
	list.Push_Front(&p2);
	//β��
	list.Push_Back(&p3);
	list.Push_Back(&p4);
	//ָ��λ�ò���
	list.Insert(2, &p5);
	list.Insert(2, &p6);
	//��ӡ
	//list.Print(print);
	list.Print(print);
	//ͷɾ
	list.Pop_Front();
	cout << "ͷɾ���:" << endl;
	list.Print(print);
	cout << "---------" << endl;
	//βɾ
	list.Pop_Back();
	cout << "βɾ���:" << endl;
	list.Print(print);
	cout << "---------" << endl;
	//ָ��λ��ɾ��
	list.RemoveByPos(2);
	cout << "ָ��ɾ��λ��2���:" << endl;
	list.Print(print);
	cout << "---------" << endl;
	//ֵɾ��
	list.RemoveByVal(&p6);
	cout << "ֵɾ�����:" << endl;
	list.Print(print);
	cout << "---------" << endl;
	cout << "Size:" << list.Size() << endl;
}

int main(){

	//test01();
	test02();

	system("pause");
	return EXIT_SUCCESS;
}