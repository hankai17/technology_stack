#ifndef LINKLIST_HPP
#define LINKLIST_HPP

//����ڵ�
template<class T>
class LinkNode{
public:
	T data;
	LinkNode<T>* next;
};

//����
template<class T>
class LinkList{
public:
	//��ʼ��
	LinkList(){
		this->mHeader.next = NULL;
		this->mSize = 0;
	}
	//ָ��λ�ò���
	void Insert(int pos,T val){
		if (pos < 0 || pos > this->mSize){
			pos = this->mSize;
		}

		//�����µĽڵ�
		LinkNode<T>* newnode = new LinkNode<T>;
		newnode->data = val;
		newnode->next = NULL;

		//�½ڵ�������
		LinkNode<T>* pCurrent = &(this->mHeader);
		for (int i = 0; i < pos;i++){
			pCurrent = pCurrent->next;
		}

		newnode->next = pCurrent->next;
		pCurrent->next = newnode;
		this->mSize++;
	}
	//ͷ������
	void Push_Front(T val){
		Insert(0, val);
	}
	//β������
	void Push_Back(T val){
		Insert(this->mSize, val);
	}
	//ָ��λ��ɾ��
	void RemoveByPos(int pos){

		if (this->mSize == 0){
			return;
		}
		if (pos < 0 || pos > this->mSize - 1){
			return;
		}

		//�ҵ���ɾ���ڵ��ǰһ���ڵ�
		LinkNode<T>* pCurrent = &(this->mHeader);
		for (int i = 0; i < pos;i ++){
			pCurrent = pCurrent->next;
		}

		//���汻ɾ���ڵ�
		LinkNode<T>* pDel = pCurrent->next;
		//�������ӱ�ɾ���ڵ��ǰ���ͺ�̽ڵ�
		pCurrent->next = pDel->next;
		//�ͷű�ɾ���ڵ��ڴ�
		delete pDel;
		this->mSize--;
	}
	//ͷɾ
	void Pop_Front(){
		if (this->mSize == 0){
			return;
		}
		RemoveByPos(0);
	}
	//βɾ
	void Pop_Back(){
		if (this->mSize == 0){
			return;
		}
		RemoveByPos(this->mSize-1);
	}
	//��������С
	int Size(){
		return this->mSize;
	}
	//ֵɾ��
	void RemoveByVal(T val){

		if (this->mSize == 0){
			return;
		}

		//����ָ�����
		LinkNode<T>* pPrev = &(this->mHeader);
		LinkNode<T>* pCurrent = pPrev->next;
		while (pCurrent != NULL){
			
			if (pCurrent->data == val){
				pPrev->next = pCurrent->next;
				delete pCurrent;
				this->mSize--;
				break;
			}

			pPrev = pCurrent;
			pCurrent = pPrev->next;
		}
	}

	//��ӡ����
	template<class MyPrint>
	void Print(MyPrint print){
		LinkNode<Person>* pCurrent = this->mHeader.next;
		while (pCurrent != NULL){
			print(pCurrent->data);
			pCurrent = pCurrent->next;
		}
	}
	
public:
	LinkNode<T> mHeader;
	int mSize;
};


#endif