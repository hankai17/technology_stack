#include"LinkList.h"

//��ʼ��
LinkList Init_LinkList(){
	LList* list = (LList*)malloc(sizeof(LList));
	list->size = 0;
	list->header.next = NULL;
	return list;
}
//ָ��λ�ò���
void Insert_LinkList(LinkList list, int pos, void* data){
	if (list == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	LList* mlist = (LList*)list;
	//�ж�λ���Ƿ�Խ�� λ�ô�0��ʼ,���Խ�磬Ĭ�ϲ��뵽β��
	if (pos < 0 || pos > mlist->size){
		pos = mlist->size;
	}

	//���Ҳ���λ��
	LinkNode* pCurrent = &(mlist->header);
	int i = 0;
	for (; i < pos;i++){
		pCurrent = pCurrent->next;
	}

	//�����½ڵ�
	LinkNode* newnode = (LinkNode*)malloc(sizeof(LinkNode));
	newnode->data = data;
	newnode->next = NULL;

	//�½ڵ�������
	newnode->next = pCurrent->next;
	pCurrent->next = newnode;
	mlist->size++;
}
//ͷ���������
void PushFront_LinkList(LinkList list, void* data){
	if (list == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	Insert_LinkList(list,0,data);
}
//β���������
void PushBack_LinkList(LinkList list, void* data){
	if (list == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	LList* mlist = (LList*)list;
	Insert_LinkList(list, mlist->size, data);
}
//ָ��λ��ɾ��
void RemoveByPos_LinkList(LinkList list, int pos){
	if (list == NULL){
		return;
	}
	LList* mlist = (LList*)list;
	if (pos < 0 || pos >= mlist->size){
		return;
	}
	//�ж������Ƿ�Ϊ��
	if (mlist->size == 0){
		return;
	}

	//����ɾ���ڵ��ǰһ���ڵ�
	LinkNode* pCurrent = &(mlist->header);
	int i = 0;
	for (; i < pos;i++){
		pCurrent = pCurrent->next;
	}
	//�����ɾ���ڵ�
	LinkNode* pDel = pCurrent->next;
	//�������ӱ�ɾ���ڵ�ǰ���ͺ�̽ڵ�
	pCurrent->next = pDel->next;
	//�ͷű�ɾ���ڵ��ڴ�
	free(pDel);
	mlist->size--;
}
//ͷ��ɾ������
void PopFront_LinkList(LinkList list){
	if (list == NULL){
		return;
	}
	LList* mlist = (LList*)list;
	if (mlist->size == 0){
		return;
	}
	RemoveByPos_LinkList(list, 0);
}
//β��ɾ������
void PopBack_LinkList(LinkList list){
	if (list == NULL){
		return;
	}
	LList* mlist = (LList*)list;
	if (mlist->size == 0){
		return;
	}
	RemoveByPos_LinkList(list, mlist->size - 1);
}
//ֵɾ��
void RemoveByVal_LinkList(LinkList list, void* data, DATA_COMPARE compare){

	if (list == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	if (compare == NULL){
		return;
	}
	LList* mlist = (LList*)list;
	if (mlist->size == 0){
		return;
	}

	//����ɾ���ڵ�
	LinkNode* pPrev = &(mlist->header);
	LinkNode* pCurrent = pPrev->next;
	while (pCurrent != NULL){
		if (compare(pCurrent->data,data)){
			pPrev->next = pCurrent->next;
			//�ͷ�ɾ���ڵ��ڴ�
			free(pCurrent);
			mlist->size--;
			break;
		}
		pPrev = pCurrent;
		pCurrent = pCurrent->next;
	}
	
}
//���ָ��λ��Ԫ��
void* Get_LinkList(LinkList list, int pos){

	if (list == NULL){
		return NULL;
	}
	LList* mlist = (LList*)list;
	if (pos < 0 || pos > mlist->size){
		return NULL;
	}

	//����posǰһ��λ�õĽڵ�
	LinkNode* pCurrent = &(mlist->header);
	for (int i = 0; i < pos;i++){
		pCurrent = pCurrent->next;
	}

	return pCurrent->next;
}
//�����С
int Size_LinkList(LinkList list){
	if (list == NULL){
		return -1;
	}
	LList* mlist = (LList*)list;
	return mlist->size;
}
//��������
void Print_LinkList(LinkList list, DATA_PRINT print){	
	if (list == NULL){
		return;
	}
	LList* mlist = (LList*)list;
	//��ֵָ��
	LinkNode* pCurrent = mlist->header.next;
	while (pCurrent!=NULL){
		print(pCurrent->data);
		pCurrent = pCurrent->next;
	}
}
//��������
void Destroy_LinkList(LinkList list){
	if (list == NULL){
		return;
	}
	LList* mlist = (LList*)list;
	LinkNode* pCurrent = mlist->header.next;
	while (pCurrent != NULL){
		//�����ɾ���ڵ���һ���ڵ�
		LinkNode* pNext = pCurrent->next;
		free(pCurrent);
		pCurrent = pNext;
	}

	//����ͷ������ڴ�
	free(mlist);
	printf("�����ڴ��ͷ�!\n");
}