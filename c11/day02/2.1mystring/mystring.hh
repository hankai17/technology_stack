#ifndef MYSTRING_H
#define MYSTRING_H

#include<iostream>
#include<string.h>

//��װһЩc�ַ�����������//����ȷ�����������ԣ�char*�ַ��� �� �䳤��
class mystring {
  public:
	mystring();  //���ַ���c mystring str;
	mystring(const char* str); //��c����ַ�����ʼ���ַ���  mystring str = "abcdefg";
	mystring(const mystring& str);

	//�ַ�������
	const char* c_str(); //��mystring�ַ���ת����c-style�ַ���
	int size();  //����ַ����ĳ���

	//��ֵ����
	mystring& operator=(const mystring& str);
	mystring& operator=(const char* p);

	//����+
	mystring operator+(mystring& str);
	mystring operator+(const char* str);

	// ==, != ����
	bool operator==(const char* p);
	bool operator!=(const char* p);
	bool operator==(mystring& obj);
	bool operator!=(mystring& obj);

	//�������Ʋ�����
	friend std::ostream& operator<<(std::ostream& out, mystring& str);

	~mystring();

  private:
	char* pAddress; //ָ��һ���ַ���
	int mLength;
};

#endif
