#ifndef LINKSTACK_H
#define LINKSTACK_H

template<class T>
class StackNode{
public:
	T data;
	StackNode<T>* next;
};

template<class T>
class LinkStack{
public:
	LinkStack(){
		mHeader.next = NULL;
		mSize = 0;
	}

	void Push(T val){
		
		//�����½ڵ�
		StackNode<T>* newnode = new StackNode<T>;
		newnode->data = val;
		newnode->next = NULL;

		//�½ڵ�������
		newnode->next = mHeader.next;
		mHeader.next = newnode;
		mSize++;
	}

	//����ջ��Ԫ��
	void Pop(){
		if (this->mSize == 0){
			return;
		}
		//�����ɾ��Ԫ��
		StackNode<T>* pDel = mHeader.next;
		mHeader.next = pDel->next;
		delete pDel;
		mSize--;
	}

	//���ջ��Ԫ��
	T Top(){
		return mHeader.next->data;
	}

	//ջ�Ƿ�Ϊ��
	bool Empty(){
		return mSize == 0;
	}

	//ջ��Ԫ�ظ���
	int Size(){
		return mSize;
	}

	//��������
	~LinkStack(){
		StackNode<T>* pCurrent = mHeader.next;
		while (pCurrent != NULL){
			//������һ���ڵ�
			StackNode<T>* pNext = pCurrent->next;
			delete pCurrent;
			pCurrent = pNext;
		}
	}

public:
	StackNode<T> mHeader; //ͷ���
	int mSize;
};



#endif