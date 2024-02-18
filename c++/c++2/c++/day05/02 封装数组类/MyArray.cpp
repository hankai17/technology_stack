#include"MyArray.h"

MyArray::MyArray(){

	//Ĭ�ϵ���������
	int capacity = 20;
	this->pAddress = new int[capacity];
	this->mCapacity = capacity;
	this->mSize = 0;

}
MyArray::MyArray(int capacity){
	if (capacity < 0){
		capacity = 20;
	}
	this->pAddress = new int[capacity];
	this->mCapacity = capacity;
	this->mSize = 0;
}
//����λ�����Ԫ��
void MyArray::SetData(int pos, int val){
	if (pos < 0 || pos > this->mCapacity - 1){
		return;
	}

	this->pAddress[pos] = val;
}
int MyArray::GetData(int pos){
	return this->pAddress[pos];
}
//β�巨
void MyArray::PushBack(int val){
	if (this->mSize == this->mCapacity){
		return;
	}
	this->pAddress[this->mSize] = val;
	this->mSize++;
}
//β��ɾ��
void MyArray::PopBack(){
	if (this->mSize <= 0){
		return;
	}
	this->mSize--;
}
//ָ��λ��ɾ��
void MyArray::RemoveByPos(int pos){
	if (pos < 0 || pos > this->mSize - 1){
		return;
	}
	for (int i = pos; i < this->mSize - 1; i ++){
		this->pAddress[i] = this->pAddress[i + 1];
	}
	this->mSize--;
}
//�������
int MyArray::GetCapacity(){
	return this->mCapacity;
}
//��ô�С
int MyArray::GetSize(){
	return this->mSize;
}
//��������
MyArray::~MyArray(){
	if (this->pAddress != NULL){
		delete[] this->pAddress;
		this->pAddress = NULL;
		this->mCapacity = 0;
		this->mSize = 0;
	}
}