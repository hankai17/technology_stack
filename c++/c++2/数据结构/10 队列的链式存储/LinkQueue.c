#include"LinkQueue.h"

//��ʼ��
LinkQueue Init_LinkQueue(){
	LQueue* queue = (LQueue*)malloc(sizeof(LQueue));
	if (queue == NULL){
		return NULL;
	}
	queue->header.next = NULL;
	queue->size = 0;
	return queue;
}
//���
void Push_LinkQueue(LinkQueue queue, QueueNode* data){
	if (queue == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	LQueue* mqueue = (LQueue*)queue;
	//�ҵ����һ��Ԫ��
	QueueNode* pCurrent = &(mqueue->header);
	int i = 0;
	for (; i < mqueue->size; i++){
		pCurrent = pCurrent->next;
	}
	//��Ԫ�������
	data->next = pCurrent->next;
	pCurrent->next = data;
	mqueue->size++;
}
//����
void Pop_LinkQueue(LinkQueue queue){
	if (queue == NULL){
		return;
	}
	LQueue* mqueue = (LQueue*)queue;
	if (mqueue->size == 0){
		return;
	}
	QueueNode* pFirst = mqueue->header.next;
	mqueue->header.next = pFirst->next;
	mqueue->size--;
}
//��ö�ͷԪ��
void* Front_LinkQueue(LinkQueue queue){
	if (queue == NULL){
		return NULL;
	}
	LQueue* mqueue = (LQueue*)queue;
	return mqueue->header.next;
}
//��ö�βԪ��
void* Back_LinkQueue(LinkQueue queue){
	if (queue == NULL){
		return NULL;
	}
	LQueue* mqueue = (LQueue*)queue;
	//�ҵ����һ��Ԫ��
	QueueNode* pCurrent = &(mqueue->header);
	int i = 0;
	for (; i < mqueue->size; i++){
		pCurrent = pCurrent->next;
	}
	return pCurrent;
}
//���д�С
int Size_LinkQueue(LinkQueue queue){
	if (queue == NULL){
		return -1;
	}
	LQueue* mqueue = (LQueue*)queue;
	return mqueue->size;
}
//���ٶ���
void Destroy_LinkQueue(LinkQueue queue){
	if (queue == NULL){
		return;
	}
	free(queue);
}
