#ifndef SEQSTACK_HPP
#define SEQSTACK_HPP

#define MAX 1024

template<class T>
class SeqStack{
public:
	//���캯��
	SeqStack(){
		mSize = 0;
	}
	//ѹջ
	void Push(T val){
		if (this->mSize == MAX){
			return;
		}
		this->mArray[this->mSize] = val;
		this->mSize++;
	}
	//���ջ��Ԫ��
	T Top(){
		return this->mArray[this->mSize - 1];
	}
	//����ջ��Ԫ��
	void Pop(){
		if (this->mSize == 0){
			return;
		}
		this->mSize--;
	}
	//ջ�Ƿ�Ϊ��
	bool Empty(){
		return this->mSize == 0;
	}
	//ջ��Ԫ�ظ���
	int Size(){
		return this->mSize;
	}
public:
	int mSize;
	T mArray[MAX];
};


#endif