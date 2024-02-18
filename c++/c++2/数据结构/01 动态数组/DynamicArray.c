#include "DynamicArray.h"

//��ʼ������
DynamicArray Init_DynamicArray(){

	DArray* darray = malloc(sizeof(DArray));
	if (darray == NULL){
		return NULL;
	}
	darray->capacity = 5; //��ʼ������5
	darray->size = 0;
	darray->address = (void**)malloc(sizeof(void*) * darray->capacity);

	return darray;
}
//ָ��λ�ò���
void Insert_DynamicArray(DynamicArray darray, int pos, void* data){
	if (darray == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	DArray* arr = (DArray*)darray;
	//�жϿռ��Ƿ��㹻
	if (arr->size == arr->capacity){
		
		int newcapacity = arr->capacity * 2; //�����ռ���չ
		//����ռ�
		void** newspace = (void**)malloc(sizeof(void*) * newcapacity);
		//����ԭ�ռ����ݵ��¿ռ�
		memcpy(newspace,arr->address,sizeof(void*) * arr->capacity);
		//�ͷžɿռ�
		free(arr->address);
		arr->address = newspace;
		arr->capacity = newcapacity;

	}

	//�ж�λ���Ƿ���Ч�����posԽ�磬����Ϊ��β������
	if (pos < 0 || pos > arr->size){
		pos = arr->size;
	}
	//�ƶ�Ԫ��
	int i = arr->size - 1;
	for (; i >= pos;i--){
		arr->address[i + 1] = arr->address[i];
	}
	arr->address[pos] = data;
	arr->size++;
}
//β���������
void PushBack_DynamicArray(DynamicArray darray, void* data){
	if (darray == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	DArray* arr = (DArray*)darray;
	Insert_DynamicArray(darray, arr->size, data);
}
//ͷ���������
void PushFront_DynamicArray(DynamicArray darray, void* data){
	if (darray == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	DArray* arr = (DArray*)darray;
	Insert_DynamicArray(darray, 0, data);
}
//λ��ɾ��
void RemoveByPos_DynamicArray(DynamicArray darray, int pos){
	if (darray == NULL){
		return;
	}
	DArray* arr = (DArray*)darray;
	//�����Ƿ�Ϊ��
	if (arr->size == 0){
		return;
	}
	if (pos < 0 || pos > arr->size - 1){
		return;
	}
	//ɾ��Ԫ��
	int i = pos;
	for (; i < arr->size - 1; i++){
		arr->address[i] = arr->address[i + 1];
	}

	arr->size--;
}
//β��ɾ������
void PopBack_DynamicArray(DynamicArray darray){
	if (darray == NULL){
		return;
	}
	DArray* arr = (DArray*)darray;
	//�����Ƿ�Ϊ��
	if (arr->size == 0){
		return;
	}
	RemoveByPos_DynamicArray(darray, arr->size - 1);
}
//ͷ��ɾ������
void PopFront_DynamicArray(DynamicArray darray){
	if (darray == NULL){
		return;
	}
	DArray* arr = (DArray*)darray;
	//�����Ƿ�Ϊ��
	if (arr->size == 0){
		return;
	}
	RemoveByPos_DynamicArray(darray, 0);
}
//ֵɾ��
void RemoveByVal_DynamicArray(DynamicArray darray, void* data, DATACOMPARE compare){
	if (darray == NULL){
		return;
	}
	if (data == NULL){
		return;
	}
	DArray* arr = (DArray*)darray;
	//�����Ƿ�Ϊ��
	if (arr->size == 0){
		return;
	}
	//����ɾ��Ԫ��
	int i = 0;
	for (; i < arr->size;i++){
		if (compare(arr->address[i], data)){
			RemoveByPos_DynamicArray(darray, i);
			break;
		}
	}
}

//���ָ��λ��Ԫ��
void* Get_DynamicArray(DynamicArray darray, int pos){
	if (darray == NULL){
		return NULL;
	}
	DArray* arr = (DArray*)darray;
	if (pos < 0 || pos >= arr->size){
		return NULL;
	}
	return arr->address[pos];
}

//�����С
int Size_DynamicArray(DynamicArray darray){
	if (darray == NULL){
		return -1;
	}
	DArray* arr = (DArray*)darray;
	return arr->size;
}
//��������
int Capacity_DynamicArray(DynamicArray darray){
	if (darray == NULL){
		return -1;
	}
	DArray* arr = (DArray*)darray;
	return arr->capacity;
}
//����
void Print_DynamicArray(DynamicArray darray, DATAPRINT print){
	if (darray == NULL){
		return;
	}
	DArray* arr = (DArray*)darray;
	int i = 0;
	for (; i < arr->size;i++){
		print(arr->address[i]);
	}
}
//��������
void Destroy_DynamicArray(DynamicArray darray){
	if (darray == NULL){
		return;
	}
	free(darray);
}