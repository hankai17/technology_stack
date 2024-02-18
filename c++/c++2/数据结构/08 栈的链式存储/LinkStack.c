#include"LinkStack.h"

//��ʼ��
LinkStack Init_LinkStack(){
	LStack* stack = (LStack*)malloc(sizeof(LStack));
	stack->header.next = NULL;
	stack->size = 0;
	return stack;
}
//ѹջ
void Push_LinkStack(LinkStack stack, StackNode* data){
	if (stack == NULL){
		return;
	}
	if (data == NULL){
		return;
	}

	LStack* lstack = (LStack*)stack;
	data->next = lstack->header.next;
	lstack->header.next = data;
	lstack->size++;
}
//��ջ
void Pop_LinkStack(LinkStack stack){
	if (stack == NULL){
		return;
	}
	LStack* lstack = (LStack*)stack;
	if (lstack->size == 0){
		return;
	}

	//��һ���ڵ�
	StackNode* pFirst = lstack->header.next;
	lstack->header.next = pFirst->next;
	lstack->size--;
}
//���ջ��Ԫ��
void* Top_LinkStack(LinkStack stack){
	if (stack == NULL){
		return NULL;
	}
	LStack* lstack = (LStack*)stack;
	return lstack->header.next;
}
//��С
int Size_LinkStack(LinkStack stack){
	if (stack == NULL){
		return -1;
	}
	LStack* lstack = (LStack*)stack;
	return lstack->size;
}
//����ջ
void Destroy_LinkStack(LinkStack stack){
	if (stack == NULL){
		return;
	}
	free(stack);
}