
#include "MyString.h"

//c��û���ַ��� ���Զ���һ���ַ����ࣨ��0��β�ģ�


void main1()
{
	MyString s1;
	MyString s2("s2");
	MyString s2_2 = NULL;
	MyString s3 = s2;
	MyString s4 = "s444444444";
	
	//s4 = "s2222";
	//s4 = s2;
	//MyString& operator=(const char * p);
	//MyString& operator=(const MyString & s);

	//s4 = "s2222";
	//s4[1] = '4';
	//printf("%c\n", s4[1]);

	//cout << s4 << endl;
	//ostream &operator<<(ostream & out,MyString &s)


	system("pause");
	return;
}


//if����==

void main2()
{
	MyString s1;
	MyString s2("s2");
	MyString s3 = s2;


	if (s2 == "s222222")// bool operator==(const char * p);
	{
		printf("���");
	}
	else
	{
		printf("�����");
	}

	if (s2 == s3)//bool operator==(const MyString & s);bool operator==(const char * p);
	{
		printf("���");
	}
	else
	{
		printf("�����");
	}
}

void main3()
{
	MyString s1;
	MyString s2("s2");
	MyString s3 = s2;
	s3 = "aaa";
	if (s3 < "bbbb")// int operator<(const char * p);
	{
		printf("s3С��bbb");
	}
	else 
	{
		printf("s3����bbb");
	}
	MyString s4 = "sdafadsf";
	strcpy(s4.c_str(), "aa33");//MFC
	//cout << s4 << endl;
}

