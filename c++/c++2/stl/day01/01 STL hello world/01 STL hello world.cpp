#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>  //��������ͷ�ļ�
#include<string>
#include<algorithm> //�㷨ͷ�ļ�
using namespace std;

void MyPrint(int val){
	cout << val << " ";
}

//1. ������Ż�����������
void test01(){

	//����vector�������󣬲���ͨ��ģ�����ָ�������д�ŵ����ݵ�����
	vector<int> v;
	//�������з�����
	v.push_back(10);
	v.push_back(20);
	v.push_back(30);
	v.push_back(40);

	//ÿһ�����������Լ��ĵ����������������������������е�Ԫ��
	//����õ�vector<int>������

	//v.begin()���ص����������������ָ�������е�һ������
	//v.end()���ص����������������ָ������Ԫ�ص����һ��Ԫ�ص���һ��λ��
	//vector<int>::iterator �õ�vector<int>���������ĵ���������
	vector<int>::iterator pBegin = v.begin(); 
	vector<int>::iterator pEnd = v.end(); 
	//���������������� vector<string> ��ô����ͨ��vector<string>::iterator�õ�����������

	while (pBegin != pEnd){
		cout << *pBegin << " ";
		//MyPrint(*pBegin);
		pBegin++;
	}
	cout << endl;

	//pBegin = v.begin();

	for (vector<int>::iterator it = v.begin(); it != v.end(); it ++){
		cout << *it << " ";
	}
	cout << endl;

	//ʹ���㷨����
	for_each(v.begin(), v.end(), MyPrint);
}


//2. �Զ�����������
class Person{
public:
	Person(string name,int age){
		mName = name;
		mAge = age;
	}
public:
	string mName;
	int mAge;
};
void test02(){

	vector<Person> v;

	//��������
	Person p1("aaa", 10);
	Person p2("bbb", 20);
	Person p3("ccc", 30);
	Person p4("ddd", 40);
	Person p5("eee", 50);
	
	v.push_back(p1);
	v.push_back(p2);
	v.push_back(p3);
	v.push_back(p4);
	v.push_back(p5);

	for (vector<Person>::iterator it = v.begin(); it != v.end(); it ++){
		cout << "Name:" << (*it).mName << " Age:" << (*it).mAge << endl;
	}
}

//3. �Ŷ���ָ��
void test03(){
	
	vector<Person*> v;

	//��������
	Person p1("aaa", 10);
	Person p2("bbb", 20);
	Person p3("ccc", 30);
	Person p4("ddd", 40);
	Person p5("eee", 50);

	v.push_back(&p1);
	v.push_back(&p2);
	v.push_back(&p3);
	v.push_back(&p4);
	v.push_back(&p5);

	for (vector<Person*>::iterator it = v.begin(); it != v.end(); it ++){
		cout << "Name:" << (*it)->mName << " Age:" << (*it)->mAge << endl;
	}
}

//4. ����Ƕ������
void test04(){
	
	vector< vector<int> >  v;

	vector<int> v1;
	vector<int> v2;
	vector<int> v3;
	vector<int> v4;

	for (int i = 0; i < 4;i++){
		v1.push_back(i + 1);
		v2.push_back(i + 2);
		v3.push_back(i + 3);
		v4.push_back(i + 4);
	}

	//������Ԫ�ز��뵽vector v��
	v.push_back(v1);
	v.push_back(v2);
	v.push_back(v3);
	v.push_back(v4);


	for (vector<vector<int>>::iterator it = v.begin(); it != v.end(); it ++){
		
		for (vector<int>::iterator vit = (*it).begin(); vit != (*it).end(); vit++){
			cout << *vit << " ";
		}
		cout << endl;

	}

}

int main(){

	//test01();
	//test02();
	//test03();
	test04();

	system("pause");
	return EXIT_SUCCESS;
}