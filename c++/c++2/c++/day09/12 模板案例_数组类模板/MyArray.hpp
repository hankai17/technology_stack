#pragma once


template<class T>
class MyArray{
public:
	MyArray(int capacity){
		this->mCapacity = capacity;
		this->mSize = 0;
		this->pAddress = new T[this->mCapacity];
	}

	//��������
	MyArray(const MyArray& arr){
		this->mCapacity = arr.mCapacity;
		this->mSize = arr.mSize;
		this->pAddress = new T[this->mCapacity];
		for (int i = 0; i < this->mSize; i ++){
			this->pAddress[i] = arr.pAddress[i];
		}
	}
	//����=�Ų�����
	MyArray& operator=(const MyArray& arr){
	
		if (this->pAddress != NULL){
			delete[] this->pAddress;
			this->pAddress = NULL;
		}

		this->mCapacity = arr.mCapacity;
		this->mSize = arr.mSize;
		this->pAddress = new T[this->mCapacity];
		for (int i = 0; i < this->mSize; i++){
			this->pAddress[i] = arr.pAddress[i];
		}

		return *this;
	}
	//����[]������
	T& operator[](int index){
		return this->pAddress[index];
	}

	//β�巨
	void PushBack(T val){
		if (this->mSize == this->mCapacity){
			return;
		}
		this->pAddress[this->mSize] = val;
		this->mSize++;
	}

	//βɾ��
	void PopBack(){
		this->mSize--;
	}

	~MyArray(){
		if (this->pAddress != NULL){
			delete[] this->pAddress;
			this->pAddress = NULL;
			this->mCapacity = 0;
			this->mSize = 0;
		}
	}


private:
	T* pAddress; //ָ�������ָ��
	int mCapacity; //����
	int mSize; //��ǰ�ж��ٸ�Ԫ��
};