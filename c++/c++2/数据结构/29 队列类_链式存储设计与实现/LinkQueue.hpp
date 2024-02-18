#ifndef LINKQUEUE_HPP
#define LINKQUEUE_HPP

template<class T>
class QueueNode{
public:
	T data;
	QueueNode<T>* next;
};

template<class T>
class LinkQueue{
public:
	LinkQueue(){
		mHeader.next = NULL;
		mSize = 0;
	}
	//���
	void Push(T val){
		//�����½ڵ�
		QueueNode<T>* newnode = new QueueNode<T>;
		newnode->data = val;
		newnode->next = NULL;

		//����ָ��
		QueueNode<T>* pCurrent = &(this->mHeader);
		for (int i = 0; i < this->mSize; i++){
			pCurrent = pCurrent->next;
		}

		//�½ڵ������� ͷ��Ϊ��ͷ
		newnode->next = pCurrent->next;
		pCurrent->next = newnode;
		mSize++;
	}
	//����
	void Pop(){
		if (this->mSize == 0){
			return;
		}	
		//��ɾ���ڵ�
		QueueNode<T>* pDel = mHeader.next;
		mHeader.next = pDel->next;
		delete pDel;
		this->mSize--;
	}
	//��ö�ͷԪ��
	T Front(){
		return mHeader.next->data;
	}
	//��ö�βԪ��
	T Back(){
		//����ָ��
		QueueNode<T>* pCurrent = &(this->mHeader);
		for (int i = 0; i < this->mSize; i++){
			pCurrent = pCurrent->next;
		}

		return pCurrent->data;
	}
	//��ö��г���
	int Size(){
		return this->mSize;
	}
	//��������
	~LinkQueue(){
		QueueNode<T>* pCurrent = this->mHeader.next;
		while (pCurrent != NULL){
			QueueNode<T>* pNext = pCurrent->next;
			delete pCurrent;
			pCurrent = pNext;
		}
	}
public:
	QueueNode<T> mHeader;
	int mSize;
};



#endif