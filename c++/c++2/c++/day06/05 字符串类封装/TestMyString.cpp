#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
#include"MyString.h"

void test01(){

	MyString str1("john ");
	MyString str2("Smith");

	MyString str3 = str1 + str2;

	MyString str5 = str1 + "hello world!";
	cout << "str5:" << str5 << endl;

	cout << "str3:" << str3 << endl;
	cout << "str3 size:" << str3.size() << endl;

	

	//���Ը�ֵ
	MyString str4;
	str4 = str3;
	cout << "str4:" << str4 << endl;
	str4 = "hello world";
	cout << "str4:" << str4 << endl;


}

void test02(){
	
	MyString str1 = "abcd";
	MyString str2 = "abcd";
	if (str1 == str2){
		cout << "str1 �� str2 ��ͬ���ַ���!" << endl;
	}

	if (str1 == "abcd"){
		cout << "str1 �� abcd ��ͬ���ַ���!" << endl;
	}

	if (str1 != str2){
		cout << "str1 �� str2 ����ͬ���ַ���!" << endl;
	}

	if (str1 != "abcd"){
		cout << "str1 �� abcd ����ͬ���ַ���!" << endl;
	}

}

int main(){

	test01();
	test02();

	system("pause");
	return EXIT_SUCCESS;
}