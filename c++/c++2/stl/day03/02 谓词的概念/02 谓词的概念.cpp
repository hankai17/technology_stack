#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
#include<algorithm>
#include<functional>
using namespace std;

//1.һԪν��


struct GreaterThan20{
	bool operator()(int val){
		return val >= 20;
	}
};


void test01(){

	vector<int> v;
	v.push_back(20);
	v.push_back(10);
	v.push_back(30);
	v.push_back(40);

	vector<int>::iterator it = find_if(v.begin(), v.end(), GreaterThan20());
	if (it == v.end()){
		cout << "û�ҵ�!" << endl;
	}
	else{
		cout << "�ҵ�:" << *it << endl;
	}

}

//��Ԫν��
struct MyComparre{
	bool operator()(int v1,int v2){
		return v1 > v2;
	}
};
void test02(){
	

	vector<int> v;
	v.push_back(20);
	v.push_back(10);
	v.push_back(30);
	v.push_back(40);

	for_each(v.begin(), v.end(), [](int val)->void{cout << val << " "; });
	sort(v.begin(), v.end(),  less<int>() );
	//���ʹ���ʽ
	for_each(v.begin(), v.end(), [](int val)->void{cout << val << " "; });
	cout << endl;

}

//3.�ڽ���������
/*
template<class T> T plus<T>//�ӷ��º���
template<class T> T minus<T>//�����º���
template<class T> T multiplies<T>//�˷��º���
template<class T> T divides<T>//�����º���
template<class T> T modulus<T>//ȡģ�º���
template<class T> T negate<T>//ȡ���º���
*/
void test03(){

	plus<int> plus;
	cout << plus(10, 20) << endl;

	cout << minus<int>()(30, 10) << endl;
}

int main(){

	test01();
	//test02();
	//test03();

	system("pause");
	return EXIT_SUCCESS;
}