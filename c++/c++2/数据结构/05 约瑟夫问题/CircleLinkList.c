#include"CircleLinkList.h"

//��ʼ��
CircleLinkList Init_CircleLinkList(){
	CLinkList* clist = (CLinkList*)malloc(sizeof(CLinkList));
	clist->size = 0;
	clist->header.next = &(clist->header);
	return clist;
}
//ָ��λ�ò���
void Insert_CircleLinkList(CircleLinkList clist, int pos, CircleLinkNode* data){
	if (clist == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	CLinkList* list = (CLinkList*)clist;
	if (pos < 0 || pos > list->size){
		pos = list->size;
	}
	
	//�ҵ�����λ��
	CircleLinkNode* pCurrent = &(list->header);
	int i = 0;
	for (; i < pos;i++){
		pCurrent = pCurrent->next;
	}
	data->next = pCurrent->next;
	pCurrent->next = data;
	list->size++;
}
//ͷ���������
void PushFront_CircleLinkList(CircleLinkList clist, CircleLinkNode* data){
	if (clist == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	Insert_CircleLinkList(clist, 0, data);
}
//β���������
void PushBack_CircleLinkList(CircleLinkList clist, CircleLinkNode* data){
	if (clist == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	CLinkList* list = (CLinkList*)clist;
	Insert_CircleLinkList(clist, list->size, data);
}
//ָ��λ��ɾ��
void RemoveByPos_CircleLinkList(CircleLinkList clist, int pos){
	if (clist == NULL){
		return;
	}
	CLinkList* list = (CLinkList*)clist;
	if (pos < 0 || pos > list->size - 1){
		return;
	}
	if (list->size == 0){
		return;
	}

	//���ұ�ɾ���ڵ��ǰһ��λ�õĽڵ�
	CircleLinkNode* pCurrent = &(list->header);
	int i = 0;
	for (; i < pos;i++){
		pCurrent = pCurrent->next;
	}
	//���汻ɾ���ڵ�
	CircleLinkNode* pDel = pCurrent->next;
	pCurrent->next = pDel->next;
	list->size--;
}
//ͷ��ɾ������
void PopFront_CircleLinkList(CircleLinkList clist){
	if (clist == NULL){
		return;
	}
	CLinkList* list = (CLinkList*)clist;
	if (list->size == 0){
		return;
	}
	RemoveByPos_CircleLinkList(clist, 0);
}
//β��ɾ������
void PopBack_CircleLinkList(CircleLinkList clist){
	if (clist == NULL){
		return;
	}
	CLinkList* list = (CLinkList*)clist;
	if (list->size == 0){
		return;
	}
	RemoveByPos_CircleLinkList(clist, list->size);
}
//ֵɾ��
void RemoveByVal_CircleLinkList(CircleLinkList clist, CircleLinkNode* data, DATA_COMPARE compare){
	if (clist == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	if (compare == NULL){
		return;
	}
	CLinkList* list = (CLinkList*)clist;
	if (list->size == 0){
		return;
	}

	//��ֵָ�����
	CircleLinkNode* pPrev = &(list->header);
	CircleLinkNode* pCurrent = pPrev->next;
	while (pCurrent != NULL){
		if (compare(pCurrent,data)){
			pPrev->next = pCurrent->next;
			list->size--;
			break;
		}
		pPrev = pCurrent;
		pCurrent = pPrev->next;
	}

}
//���ָ��λ��Ԫ��
void* Get_CircleLinkList(CircleLinkList clist, int pos){
	if (clist == NULL){
		return NULL;
	}

	CLinkList* list = (CLinkList*)clist;
	if (pos < 0 || pos > list->size - 1){
		return NULL;
	}
	
	//��ֵָ�����
	CircleLinkNode* pCurrent = &(list->header);
	int i = 0;
	for (; i < pos;i++){
		pCurrent = pCurrent->next;
	}

	return pCurrent->next;
}
//�����С
int Size_CircleLinkList(CircleLinkList clist){
	if (clist == NULL){
		return -1;
	}
	CLinkList* list = (CLinkList*)clist;
	return list->size;
}
//��ӡ����
void Print_CircleLinkList(CircleLinkList clist, DATA_PRINT print){
	if (clist == NULL){
		return;
	}
	CLinkList* list = (CLinkList*)clist;
	//����ָ�����
	CircleLinkNode* pCurrent = list->header.next;
	int i = 0;
	for (; i < list->size;i++){
		print(pCurrent);
		pCurrent = pCurrent->next;
		if (pCurrent == &(list->header)){
			pCurrent = pCurrent->next;
		}
	}

}
//��������
void Destroy_CircleLinkList(CircleLinkList clist){
	if (clist == NULL){
		return;
	}
	free(clist);
}