#include"MyArray.h"

MyArray::MyArray(){
	this->mCapacity = 100;
	this->mSize = 0;
	//�ڶѿ��ٿռ�
	this->pAdress = new int[this->mCapacity];
}
//�вι��캯�����û�ָ��������ʼ��
MyArray::MyArray(int capacity){
	this->mCapacity = capacity;
	this->mSize = 0;
	//�ڶѿ��ٿռ�
	this->pAdress = new int[capacity];
}

//����λ�����Ԫ��
void MyArray::SetData(int pos, int val){
	if (pos < 0 || pos > mCapacity - 1){
		return;
	}
	pAdress[pos] = val;
}
//���ָ��λ������
int MyArray::GetData(int pos){
	return pAdress[pos];
}
//β�巨
void MyArray::PushBack(int val){
	if (mSize >= mCapacity){
		return;
	}
	this->pAdress[mSize] = val;
	this->mSize++;
}
//��ó���
int MyArray::GetLength(){
	return this->mSize;
}
//�����������ͷ�����ռ�
MyArray::~MyArray(){
	if (this->pAdress != nullptr){
		delete[] this->pAdress;
	}
}