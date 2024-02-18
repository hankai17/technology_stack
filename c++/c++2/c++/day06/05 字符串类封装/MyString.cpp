#include"MyString.h"

MyString::MyString(){
	this->mLength = 0;
	this->pAddress = new char[1];
	this->pAddress[0] = '\0';
}
MyString::MyString(const char* str){
	//�ַ�������
	this->mLength = strlen(str);
	this->pAddress = new char[this->mLength + 1];
	strcpy(this->pAddress,str);
}
MyString::MyString(const MyString& str){
	this->mLength = str.mLength;
	this->pAddress = new char[this->mLength + 1];
	strcpy(this->pAddress,str.pAddress);
}

//�ַ�������
const char* MyString::c_str(){
	return this->pAddress;
}

int MyString::size(){
	return this->mLength;
}

//��ֵ����
MyString& MyString::operator=(const MyString& str){

	if (this->pAddress != NULL){
		delete[] this->pAddress;
		this->pAddress = NULL;
	}

	this->mLength = str.mLength;
	this->pAddress = new char[this->mLength + 1];
	strcpy(this->pAddress, str.pAddress);

	return *this;
}


MyString& MyString::operator=(const char* p){

	if (this->pAddress != NULL){
		delete[] this->pAddress;
		this->pAddress = NULL;
	}

	this->pAddress = new char[strlen(p) + 1];
	strcpy(this->pAddress,p);

	return *this;
}

//MyString str3 = str1 + str2; //str1.operator+(str2) => operator+(&str1,str2)
//����+
MyString MyString::operator+(MyString& str){
	
	//ͳ�������ַ������Ⱥ�
	int newLength = this->mLength + str.mLength;
	//���ݳ��ȿ����ڴ�
	char* buf = new char[newLength + 1];
	//��ʼ��buf
	for (int i = 0; i < newLength + 1; i ++){
		buf[i] = '\0';
	}

	strcat(buf, this->pAddress);
	strcat(buf, str.pAddress);

	//�����µ�MyString����
	MyString newString;
	delete[] newString.pAddress;
	newString.pAddress = buf;
	newString.mLength = newLength;

	return newString;
}

//MyString str3 = str1 + "abcd";
MyString MyString::operator+(const char* str){

	int newLength = this->mLength + strlen(str);
	//���ݳ��ȿ����ڴ�
	char* buf = new char[newLength + 1];
	//��ʼ��buf
	for (int i = 0; i < newLength + 1; i++){
		buf[i] = '\0';
	}
	strcat(buf, this->pAddress);
	strcat(buf, str);

	//�����µ�MyString����
	MyString newString;
	delete[] newString.pAddress;
	newString.pAddress = buf;
	newString.mLength = newLength;

	return newString;
}

// ==, != ����
bool MyString::operator==(const char* p){
	if (strcmp(this->pAddress,p) == 0){
		return true;
	}
	return false;
}
bool MyString::operator!=(const char* p){
	if (strcmp(this->pAddress, p) != 0){
		return true;
	}
	return false;
}
bool MyString::operator==(MyString& obj){
	if (strcmp(this->pAddress,obj.pAddress) == 0){
		return true;
	}
	return false;
}
bool MyString::operator!=(MyString& obj){
	if (strcmp(this->pAddress, obj.pAddress) != 0){
		return true;
	}
	return false;
}
//�������Ʋ�����
std::ostream& operator<<(std::ostream& out, MyString& str){
	out << str.pAddress;
	return out;
}
MyString::~MyString(){
	if (this->pAddress != NULL){
		delete[] this->pAddress;
		this->pAddress = NULL;
		this->mLength = 0;
	}
}