#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
#include<algorithm>
#include<string>
#include<functional>
#include<list>
using namespace std;

//1. merge �ϲ���������������
struct MyCompare{
	bool operator()(int v1,int v2){
		return v1 > v2;
	}
};
void test01(){
	

	vector<int> v1;
	vector<int> v2;
	for (int i = 10; i > 0; i --){
		v1.push_back(i);
		v2.push_back(i + 1);
	}

	vector<int> vtarget;
	vtarget.resize(v1.size() + v2.size());
	//Ĭ�ϴ�С����
	//merge(v1.begin(),v1.end(),v2.begin(),v2.end(),vtarget.begin());
	//�Ӵ�С
	merge(v1.begin(), v1.end(), v2.begin(), v2.end(), vtarget.begin(), MyCompare());
	for_each(vtarget.begin(), vtarget.end(), [](int v){cout << v << " "; });
}


//sort�㷨
void test02(){

#if 0
	//ʹ��sort�㷨 ��������֧���������
	list<int> mylist;
	mylist.push_back(10);
	mylist.push_back(70);
	mylist.push_back(50);
	mylist.push_back(20);

	sort(mylist.begin(), mylist.end());
#endif

	vector<int> v;
	v.push_back(10);
	v.push_back(70);
	v.push_back(50);
	v.push_back(20);

	//sortĬ�ϴ�С��������
	sort(v.begin(),v.end());
	sort(v.begin(), v.end(),greater<int>());
	for_each(v.begin(), v.end(), [](int val){cout << val << " "; });
}

//random_shuffle ϴ�ƣ�����������Ԫ�صĴ���
void test03(){
	
	vector<int> v;
	v.push_back(10);
	v.push_back(70);
	v.push_back(50);
	v.push_back(20);

	for_each(v.begin(), v.end(), [](int val){cout << val << " "; }); cout << endl;
	random_shuffle(v.begin(),v.end());
	for_each(v.begin(), v.end(), [](int val){cout << val << " "; }); cout << endl;
}

//reverse ��ת
void test04(){

	vector<int> v;
	v.push_back(10);
	v.push_back(70);
	v.push_back(50);
	v.push_back(20);

	for_each(v.begin(), v.end(), [](int val){cout << val << " "; }); cout << endl;
	reverse(v.begin(),v.end());
	for_each(v.begin(), v.end(), [](int val){cout << val << " "; }); cout << endl;
}



int main(){

	//test01();
	//test02();
	//test03();
	test04();


	system("pause");
	return EXIT_SUCCESS;
}