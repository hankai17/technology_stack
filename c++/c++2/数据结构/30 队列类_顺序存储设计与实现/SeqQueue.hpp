#ifndef SEQQUEUE_HPP
#define SEQQUEUE_HPP

#define MAX 1024

template<class T>
class SeqQueue{
public:
	//���캯��
	SeqQueue(){
		mSize = 0;
	}

	//���
	void Push(T val){
		if (this->mSize == MAX){
			return;
		}
		this->mArray[this->mSize] = val;
		this->mSize++;
	}
	//����
	void Pop(){
		if (this->mSize == 0){
			return;
		}
		for (int i = 0; i < this->mSize - 1; i ++){
			this->mArray[i] = this->mArray[i + 1];
		}
		this->mSize--;
	}

	//��ö�ͷԪ��
	T Front(){
		return this->mArray[0];
	}
	//��ö�βԪ��
	T Back(){
		return this->mArray[this->mSize - 1];
	}
	//��С
	int Size(){
		return mSize;
	}

public:
	T mArray[MAX];
	int mSize;
};


#endif