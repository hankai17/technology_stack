#include"DoubleLinkList.h"

//��ʼ��
DoubleLinkList Init_DoubleLinkList(){
	DLinkList* dlist = (DLinkList*)malloc(sizeof(DLinkList));
	dlist->header.next = NULL;
	dlist->header.prev = NULL;
	dlist->size = 0;
	return dlist;
}
//ָ��λ�ò���
void Insert_DoubleLinkList(DoubleLinkList dlist, int pos, DoubleLinkNode* data){
	if (dlist == NULL){
		return;
	}
	DLinkList* list = (DLinkList*)dlist;
	if (pos < 0 || pos > list->size){
		pos = list->size;
	}

	//�ж������Ƿ�Ϊ��
	if (list->size == 0){
		data->next = list->header.next;
		data->prev = &(list->header);
		list->header.next = data;
		list->size++;
		return;
	}

	//���Ҳ���λ�õ�ǰһ��λ��
	DoubleLinkNode* pCurrent = &(list->header);
	int i = 0;
	for (; i < pos;i++){
		pCurrent = pCurrent->next;
	}

	//�ж��Ƿ���β������ڵ�
	if (pos == list->size){
		data->next = pCurrent->next;
		data->prev = pCurrent;
		pCurrent->next = data;
		list->size++;
		return;
	}

	//�����������
	data->next = pCurrent->next;
	data->prev = pCurrent;
	pCurrent->next->prev = data;
	pCurrent->next = data;
	list->size++;
}
//ͷ���������
void PushFront_DoubleLinkList(DoubleLinkList dlist, DoubleLinkNode* data){
	if (dlist == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	Insert_DoubleLinkList(dlist, 0, data);
}
//β���������
void PushBack_DoubleLinkList(DoubleLinkList dlist, DoubleLinkNode* data){
	if (dlist == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	DLinkList* list = (DLinkList*)dlist;
	Insert_DoubleLinkList(dlist, list->size, data);
}
//ָ��λ��ɾ��
void RemoveByPos_DoubleLinkList(DoubleLinkList dlist, int pos){
	if (dlist == NULL){
		return;
	}
	DLinkList* list = (DLinkList*)dlist;
	if (pos < 0 || pos > list->size - 1){
		return;
	}

	if (list->size == 0){
		return;
	}

	//�ж������Ƿ�Ϊ1���ڵ�
	if (list->size == 1){
		list->header.next = NULL;
		list->size--;
		return;
	}

	//����ɾ���ڵ��ǰһ���ڵ�
	DoubleLinkNode* pCurrent = &(list->header);
	int i = 0;
	for (; i < pos;i++){
		pCurrent = pCurrent->next;
	}

	//�ж�ɾ���Ľڵ��Ƿ������һ���ڵ�
	if (pos == list->size - 1){
		pCurrent->next = NULL;
		list->size--;
		return;
	}

	//����ɾ�����
	//�����ɾ���ڵ�
	DoubleLinkNode* pDel = pCurrent->next;
	pDel->next->prev = pCurrent;
	pCurrent->next = pDel->next;
	list->size--;
}
//ͷ��ɾ������
void PopFront_DoubleLinkList(DoubleLinkList dlist){
	if (dlist == 0){
		return;
	}
	DLinkList* list = (DLinkList*)dlist;
	if (list->size == 0){
		return;
	}
	RemoveByPos_DoubleLinkList(dlist,0);
}
//β��ɾ������
void PopBack_DoubleLinkList(DoubleLinkList dlist){
	if (dlist == 0){
		return;
	}
	DLinkList* list = (DLinkList*)dlist;
	if (list->size == 0){
		return;
	}
	RemoveByPos_DoubleLinkList(dlist, list->size - 1);
}
//�����С
int Size_DoubleLinkList(DoubleLinkList dlist){
	if (dlist == NULL){
		return -1;
	}
	DLinkList* list = (DLinkList*)dlist;
	return list->size;
}
//�����ӡ����
void Print_DoubleLinkList(DoubleLinkList dlist, DATA_PRINT print){
	if (dlist == NULL){
		return;
	}
	DLinkList* list = (DLinkList*)dlist;
	//����ָ��
	DoubleLinkNode* pCurrent = list->header.next;
	while (pCurrent != NULL){
		print(pCurrent);
		pCurrent = pCurrent->next;
	}
}
//�����ӡ����
void RPrint_DoubleLinkList(DoubleLinkList dlist, DATA_PRINT print){
	if (dlist == NULL){
		return;
	}
	DLinkList* list = (DLinkList*)dlist;
	//�ҵ�β�ڵ�
	DoubleLinkNode* pCurrent = &(list->header);
	int i = 0;
	for (; i < list->size;i++){
		pCurrent = pCurrent->next;
	}

	//�����ӡ
	while (pCurrent != &(list->header)){
		print(pCurrent);
		pCurrent = pCurrent->prev;
	}
}
//��������
void Destroy_DoubleLinkList(DoubleLinkList dlist){
	if (dlist == NULL){
		return;
	}
	free(dlist);
}