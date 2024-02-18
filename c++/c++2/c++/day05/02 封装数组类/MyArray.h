#ifndef MYARRAY_H
#define MYARRAY_H

#include<stdlib.h>

class MyArray{
public:
	MyArray();
	MyArray(int capacity);
	//����λ�����Ԫ��
	void SetData(int pos,int val);
	int GetData(int pos);
	//β�巨
	void PushBack(int val);
	//β��ɾ��
	void PopBack();
	//ָ��λ��ɾ��
	void RemoveByPos(int pos);
	//�������
	int GetCapacity();
	//��ô�С
	int GetSize();

	//д������ ���߱��� mArray[0] Ҫ��ʲô��
	int& operator[](int index){
		return this->pAddress[index];
	}


	//��������
	~MyArray();
private:
	int* pAddress; //������׵�ַ
	int mCapacity; //��������ʾ�������һ���������ɶ��ٸ�Ԫ��
	int mSize; //��ʾ��ǰ�����ж��ٸ�Ԫ��
};




#endif