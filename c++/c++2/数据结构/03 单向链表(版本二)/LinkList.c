#include"LinkList.h"

//��ʼ��
LinkList Init_LinkList(){
	LList* list = (LList*)malloc(sizeof(LList));
	if (list == NULL){
		return NULL;
	}
	list->header.next = NULL;
	list->size = 0;

	return list;
}
//ָ��λ�ò���
void Insert_LinkList(LinkList list, int pos, LinkNode* data){
	if (list == NULL){
		return;
	}
	LList* mlist = (LList*)list;
	if (pos < 0 || pos > mlist->size){
		pos = mlist->size;
	}
	if (data == NULL){
		return;
	}

#if 0
	//�������²������Ͳ���Ч�ʣ����û��Ծ�������ظ�Ԫ��
	//�ж�ָ���ַ�Ƿ��ظ� 
	LinkNode* pTest = mlist->header.next;
	while (pTest != NULL){
		if (pTest == data){
			printf("ָ���ظ�!\n");
			return;
		}
		pTest = pTest->next;
	}
#endif

	//����ָ�����
	LinkNode* pCurrent = &(mlist->header);
	int i = 0;
	for (; i < pos;i++){
		pCurrent = pCurrent->next;
	}

	//������������
	data->next = pCurrent->next;
	pCurrent->next = data;
	mlist->size++;
}

//ͷ���������
void PushFront_LinkList(LinkList list, LinkNode* data){
	if (list == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	Insert_LinkList(list, 0, data);
}
//β���������
void PushBack_LinkList(LinkList list, LinkNode* data){
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
	if (pos < 0 || pos > mlist->size - 1){
		return;
	}
	if (mlist->size == 0){
		return;
	}
	
	//����ָ�����
	LinkNode* pCurernt = &(mlist->header);
	int i = 0;
	for (; i < pos;i++){
		pCurernt = pCurernt->next;
	}

	//�����±�ɾ���ڵ�
	LinkNode* pDel = pCurernt->next;
	//��ɾ���ڵ�ǰ���ڵ�ĺ��ָ��ָ���ɾ���ڵ�ĺ�̽ڵ�
	pCurernt->next = pDel->next;
	mlist->size--;
}
//ͷ��λ��ɾ��
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
//β��λ��ɾ��
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
//����ֵɾ��
void RemoveByVal_LinkList(LinkList list, LinkNode* data, DATA_COMPARE compare){
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

	//����ָ�����
	LinkNode* pPrev = &(mlist->header);
	LinkNode* pCurrent = pPrev->next;
	while (pCurrent != NULL){
		if (compare(pCurrent,data)){
			pPrev->next = pCurrent->next;
			mlist->size--;
			break;
		}

		pPrev = pCurrent;
		pCurrent = pPrev->next;
	}
}
//�����С
int Size_LinkList(LinkList list){
	if (list == NULL){
		return -1;
	}
	LList* mlist = (LList*)list;
	return mlist->size;
}
//��ӡ����
void Print_LinkList(LinkList list, DATA_PRINT print){
	if (list == NULL){
		return;
	}
	if (print == NULL){
		return;
	}
	LList* mlist = (LList*)list;
	//����ָ�����
	LinkNode* pCurrent = mlist->header.next;
	while (pCurrent != NULL){
		print(pCurrent);
		pCurrent = pCurrent->next;
	}
}
//��������
void Destroy_LinkList(LinkList list){
	if (list == NULL){
		return;
	}
	free(list);
}