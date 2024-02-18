#include"SeqStack.h"

//��ʼ��
SeqStack Init_SeqStack(){
	SStack* stack = (SStack*)malloc(sizeof(SStack));
	if (stack == NULL){
		return NULL;
	}
	stack->size = 0;
	for (int i = 0; i < MAXSIZE;i++){
		stack->data[i] = NULL;
	}
	return stack;
}
//ѹջ
void Push_SeqStack(SeqStack stack, void* data){
	if (stack == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	SStack* sstack = (SStack*)stack;
	if (sstack->size == MAXSIZE){
		return;
	}
	sstack->data[sstack->size] = data;
	sstack->size++;
}
//��ջ
void Pop_SeqStack(SeqStack stack){
	if (stack == NULL){
		return;
	}
	SStack* sstack = (SStack*)stack;
	if (sstack->size == 0){
		return;
	}
	sstack->size--;
}
//���ջ��Ԫ��
void* Top_SeqStack(SeqStack stack){
	if (stack == NULL){
		return NULL;
	}
	SStack* sstack = (SStack*)stack;
	if (sstack->size == 0){
		return NULL;
	}
	return sstack->data[sstack->size - 1];
}
//��С
int Size_SeqStack(SeqStack stack){
	if (stack == NULL){
		return -1;
	}
	SStack* sstack = (SStack*)stack;
	return sstack->size;
}
//����ջ
void Destroy_SeqStack(SeqStack stack){
	if (stack == NULL){
		return;
	}
	free(stack);
}