#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;


//1. string��ʼ��
/*
string();//����һ���յ��ַ��� ����: string str;
string(const string& str);//ʹ��һ��string�����ʼ����һ��string����
string(const char* s);//ʹ���ַ���s��ʼ��
string(int n, char c);//ʹ��n���ַ�c��ʼ��
*/
void test01(){

	
	string s1; //�������ַ����������޲ι��캯��
	string s2(s1); //���ÿ������캯��
	//string s3 = s1; //���ÿ������캯��
	const char* str = "hello world!";
	string s4(str); //��c_stringת������string
	string s5(10, 'a');

	cout << s4 << endl;
	cout << s5 << endl;

}

//2. ��ֵ����
/*
string& operator=(const char* s);//char*�����ַ��� ��ֵ����ǰ���ַ���
string& operator=(const string &s);//���ַ���s������ǰ���ַ���
string& operator=(char c);//�ַ���ֵ����ǰ���ַ���
string& assign(const char *s);//���ַ���s������ǰ���ַ���
string& assign(const char *s, int n);//���ַ���s��ǰn���ַ�������ǰ���ַ���
string& assign(const string &s);//���ַ���s������ǰ�ַ���
string& assign(int n, char c);//��n���ַ�c������ǰ�ַ���
string& assign(const string &s, int start, int n);//��s��start��ʼn���ַ���ֵ���ַ���
*/
void test02(){

	string s = "abcd";
	s = "hello world!"; //��ֵ
	s = 'a';

	string s2;
	s2.assign(s);

	cout << s << endl;
	cout << s2 << endl;
}

//3. �ַ���ȡ
void test03(){
	
	string s = "hello world!";

	//[]��at ���ص���Ԫ�ص�����

	for (int i = 0; i < s.size();i++){
		cout << s[i] << " ";
	}
	cout << endl;

	for (int i = 0; i < s.size(); i ++){
		cout << s.at(i) << " ";
		//s.at(i) = 100;
	}
	cout << endl;

	//[]��at����[]����Խ�磬ֱ�ӹҵ���at����Խ����׳�һ��out_of_range�쳣
	try{
		//cout << s[100] << endl;
		cout << s.at(100) << endl;
	}
	catch (...){
		cout << "����Խ��!" << endl;
	}

}

//4.�ַ���ƴ��
/*
string& operator+=(const string& str);//����+=������
string& operator+=(const char* str);//����+=������
string& operator+=(const char c);//����+=������
string& append(const char *s);//���ַ���s���ӵ���ǰ�ַ�����β
string& append(const char *s, int n);//���ַ���s��ǰn���ַ����ӵ���ǰ�ַ�����β
string& append(const string &s);//ͬoperator+=()
string& append(const string &s, int pos, int n);//���ַ���s�д�pos��ʼ��n���ַ����ӵ���ǰ�ַ�����β
string& append(int n, char c);//�ڵ�ǰ�ַ�����β���n���ַ�c
*/
void test04(){

	string s1 = "hello";
	string s2 = " world";

	s1 += s2;

	cout << s1 << endl;
	cout << s2 << endl;

	string s3 = s1 + s2;
	cout << s3 << endl;

	//string + c_string
	string s4 = s1 + " aaaaa";
	cout << s4 << endl;


	s4.append(" bbbbbb");
	cout << s4 << endl;
	s4.append(s1);
	cout << s4 << endl;

}

//5. �ַ�������
/*
int find(const string& str, int pos = 0) const; //����str��һ�γ���λ��,��pos��ʼ����
int find(const char* s, int pos = 0) const;  //����s��һ�γ���λ��,��pos��ʼ����
int find(const char* s, int pos, int n) const;  //��posλ�ò���s��ǰn���ַ���һ��λ��
int find(const char c, int pos = 0) const;  //�����ַ�c��һ�γ���λ��
int rfind(const string& str, int pos = npos) const;//����str���һ��λ��,��pos��ʼ����
int rfind(const char* s, int pos = npos) const;//����s���һ�γ���λ��,��pos��ʼ����
int rfind(const char* s, int pos, int n) const;//��pos����s��ǰn���ַ����һ��λ��
int rfind(const char c, int pos = 0) const; //�����ַ�c���һ�γ���λ��
string& replace(int pos, int n, const string& str); //�滻��pos��ʼn���ַ�Ϊ�ַ���str
string& replace(int pos, int n, const char* s); //�滻��pos��ʼ��n���ַ�Ϊ�ַ���s
*/
void test05(){

	//���û�в��ҵ�����-1
	string s = "abcdefgth";
	int pos = s.find("dep");
	if (pos == -1){
		cout << "û�в��ҵ�!" << endl;
	}
	else{
		cout << "���ҵ�:" << pos << endl;
	}

	s.replace(1, 3, "111");
	cout << s << endl;
}

//6. �ַ����Ƚ�
/*

compare������>ʱ���� 1��<ʱ���� -1��==ʱ���� 0��
�Ƚ����ִ�Сд���Ƚ�ʱ�ο��ֵ�˳����Խǰ���ԽС��
��д��A��Сд��aС��

int compare(const string &s) const;//���ַ���s�Ƚ�
int compare(const char *s) const;//���ַ���s�Ƚ�
*/
void test06(){

	string s1 = "hello";
	string s2 = "hello";
	const char* str = "hello";
	if (s1.compare(s2) == 0){
		cout << "���!" << endl;
	}

	if (s1.compare(str) == 0){
		cout << "���!" << endl;
	}

}

//7. �Ӵ�
/*
	string substr(int pos = 0, int n = npos) const;//������pos��ʼ��n���ַ���ɵ��ַ���
*/

void test07(){
	
	string email = "hello@hotmal.com";
	int pos = email.find("@");
 	string username =  email.substr(0, pos);
	cout << "username:" << username << endl;
}

//8.����
/*
string& insert(int pos, const char* s); //�����ַ���
string& insert(int pos, const string& str); //�����ַ���
string& insert(int pos, int n, char c);//��ָ��λ�ò���n���ַ�c
string& erase(int pos, int n = npos);//ɾ����Pos��ʼ��n���ַ� 
*/

void test08(){

	string s = "hello";
	s.insert(1,"111");
	cout << s << endl;

	s.erase(1,3);  //��1��λ�ÿ�ʼ3���ַ�
	cout << s << endl;
}

//9. c_string��stringת��
void test09(){
	
	//c_string -> string
	const char* str = "hello world";
	string s(str); //����string�Ĵ������Ĺ��캯��

	const char* str2 = s.c_str(); //string->c_string

}

//10.
void test10(){

	string s1 = "hello world!";
	cout << (int*)s1.c_str() << endl;

	char& a = s1[1];
	char& b = s1[3];

	a = '0';
	b = '1';

	cout << s1 << endl;

	s1 = "pppppppppppppppppppp";


	a = '0';
	b = '1';

	cout << (int*)s1.c_str() << endl;
}
int main(){

	//test01();
	//test02();
	//test03();
	//test04();
	//test05();
	//test06();
	//test07();
	//test08();
	test10();

	system("pause");
	return EXIT_SUCCESS;
}