#include"SeqQueue.h"


//��ʼ��
SeqQueue Init_SeqQueue(){
	SQueue* queue = (SQueue*)malloc(sizeof(SQueue));
	if (queue == NULL){
		return NULL;
	}
	queue->size = 0;
	return queue;
}
//���
void Push_SeqQueue(SeqQueue queue, void* data){
	if (queue == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	SQueue* mqueue = (SQueue*)queue;
	if (mqueue->size == MAXSIZE){
		return;
	}
	
	//�ƶ�Ԫ��
	int i = mqueue->size - 1;
	for (; i >= 0; i --){
		mqueue->data[i + 1] = mqueue->data[i];
	}
	mqueue->data[0] = data;
	mqueue->size++;
}
//����
void Pop_SeqQueue(SeqQueue queue){
	if (queue == NULL){
		return;
	}
	SQueue* mqueue = (SQueue*)queue;
	if (mqueue->size == 0){
		return;
	}
	mqueue->size--;
}
//��ö�ͷԪ��
void* Front_SeqQueue(SeqQueue queue){
	if (queue == NULL){
		return NULL;
	}
	SQueue* mqueue = (SQueue*)queue;
	if (mqueue->size == 0){
		return NULL;
	}
	return mqueue->data[mqueue->size-1];
}
//��ö�βԪ��
void* Back_SeqQueue(SeqQueue queue){
	if (queue == NULL){
		return NULL;
	}
	SQueue* mqueue = (SQueue*)queue;
	if (mqueue->size == 0){
		return NULL;
	}
	return mqueue->data[0];
}
//��ö��еĳ���
int Size_SeqQueue(SeqQueue queue){
	if (queue == NULL){
		return -1;
	}
	SQueue* mqueue = (SQueue*)queue;
	return mqueue->size;
}
//���ٶ���
void Destroy_SeqQueue(SeqQueue queue){
	if (queue == NULL){
		return;
	}
	free(queue);
}