#define _CRT_SECURE_NO_WARNINGS
#ifndef MYSTRING_H
#define MYSTRING_H

#include<iostream>

//��װһЩc�ַ�����������
class MyString{
public:
	MyString();  //���ַ���c MyString str;
	MyString(const char* str); //��c����ַ�����ʼ���ַ���  MyString str = "abcdefg";
	MyString(const MyString& str);

	//�ַ�������
	const char* c_str(); //��MyString�ַ���ת����c-style�ַ���
	int size();  //����ַ����ĳ���

	//��ֵ����
	MyString& operator=(const MyString& str);
	MyString& operator=(const char* p);

	//����+
	MyString operator+(MyString& str);
	MyString operator+(const char* str);

	// ==, != ����
	bool operator==(const char* p);
	bool operator!=(const char* p);
	bool operator==(MyString& obj);
	bool operator!=(MyString& obj);

	//�������Ʋ�����
	friend std::ostream& operator<<(std::ostream& out, MyString& str);

	~MyString();
private:
	char* pAddress; //ָ��һ���ַ���
	int mLength;
};

#endif