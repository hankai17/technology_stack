#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<functional>
using namespace std;


//1. ���Լ���д�ĺ�������̳л���,�����һԪ����������Ҫ�̳�unary_function,
//����Ƕ�Ԫ��������̳�binary_function

//2. ��������operator()���������� const
//3. ʹ��bind2nd bind1st

//bind2nd��ʵ�ͽ�һ����Ԫ�ĺ�������ת��Ϊ��һԪ��������

struct MyPrint : public binary_function<int,int,void> {
	void operator()(int v,int v2) const{
		cout << "v:" << v << " v2:" << v2 << endl;
		cout << v + v2 << " ";
	}
};

//�����for_each�㷨���ö�Ԫ�������󣬰Ѷ�Ԫ��������ת��ΪһԪ��������

void test01(){
	
	vector<int> v;
	for (int i = 0; i < 10;i ++){
		v.push_back(i + 1);
	}
	
	for_each(v.begin(), v.end(), bind1st(MyPrint(),300)); 
	//bind2nd��300��Ϊ��������ĵڶ�������
	//bind1st��300��Ϊ��������ĵ�һ������
	cout << endl;
}

//2. ȡ�������� not1 not2
struct GreaterThan5 : public unary_function<int,bool>{
	bool operator()(int v) const{
		return v > 5;
	}
};
void test02(){

	vector<int> v;
	for (int i = 0; i < 10;i++){
		v.push_back(i + 1);
	}

	//vector<int>::iterator it = find_if(v.begin(), v.end(), not1(GreaterThan5()));
	vector<int>::iterator it = find_if(v.begin(), v.end(), not1( bind2nd(greater<int>(),5)) );
	if (it == v.end()){
		cout << "ô���ҵ�!" << endl;
	}
	else{
		cout << "�ҵ�:" << *it << endl;
	}


	//����
	sort(v.begin(),v.end(),not2(less<int>()));
	for_each(v.begin(), v.end(), [](int val){cout << val << " "; });


	//not1 ��һԪν��ȡ��
	//not2 �Զ�Ԫν��ȡ��
}

//3. ptr_fun ����ָ��������
void MyPrint03(int val,int v2){
	cout << val + v2 << " ";
}
//��һ����ͨ�ĺ���ָ������ɺ�������
void test03(){

	vector<int> v;
	for (int i = 0; i < 10; i++){
		v.push_back(i + 1);
	}

	for_each(v.begin(), v.end(), bind2nd( ptr_fun(MyPrint03), 100));
}

//4. ��Ա���������� mem_fun��mem_fun_ref
class Person{
public:
	Person(string name,int age){
		this->mName = name;
		this->mAge = age;
	}

	//��ӡ����
	void ShowPerson(){
		cout << "��Ա����:" << "Name:" << mName << " Age:" << mAge << endl;
	}

	void AgePlus100(){
		this->mAge += 100;
	}

public:
	string mName;
	int mAge;
};

void MyPrint04(Person& p){
	cout << "ȫ�ֺ���:" << "Name:" << p.mName << " Age:" << p.mAge << endl;
}

void test04(){
	

	vector<Person> v1;
	//��������
	Person p1("aaa", 10);
	Person p2("bbb", 20);
	Person p3("ccc", 30);
	Person p4("ddd", 40);

	v1.push_back(p1);
	v1.push_back(p2);
	v1.push_back(p3);
	v1.push_back(p4);

	for_each(v1.begin(), v1.end(), MyPrint04);

	cout << "-------------" << endl;

	//���ó�Ա������ӡ
	for_each(v1.begin(), v1.end(), mem_fun_ref(&Person::ShowPerson));
	for_each(v1.begin(), v1.end(), mem_fun_ref(&Person::AgePlus100));
	for_each(v1.begin(), v1.end(), mem_fun_ref(&Person::ShowPerson));
}

void test05(){

	vector<Person*> v1;
	//��������
	Person p1("aaa", 10);
	Person p2("bbb", 20);
	Person p3("ccc", 30);
	Person p4("ddd", 40);

	v1.push_back(&p1);
	v1.push_back(&p2);
	v1.push_back(&p3);
	v1.push_back(&p4);

	for_each(v1.begin(), v1.end(), mem_fun(&Person::ShowPerson));
}

//mem_fun ���������ŵ��Ƕ���ָ�룬��ô��men_fun
//��������д�ŵ��Ƕ���ʵ�壬��ô��mem_fun_ref


int main(){

	//test01();
	//test02();
	//test03();
	//test04();
	test05();

	system("pause");
	return EXIT_SUCCESS;
}