#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define MAX 20

void printArray(int arr[], int len){
	for (int i = 0; i < len; i++){
		printf("%d ", arr[i]);
	}
	printf("\n");
}

void MySwap(int arr[],int i,int j){

	int temp = arr[i];
	arr[i] = arr[j];
	arr[j] = temp;

}

//������
void HeapAdjust(int arr[],int index, int len){

	int lchild = index * 2 + 1;
	int rchild = index * 2 + 2;
	int max = index;

	if (index < len / 2){
	
		if (lchild < len && arr[lchild] > arr[max]){
			max = lchild;
		}

		if (rchild < len && arr[rchild] > arr[max]){
			max = rchild;
		}

		if (max != index){
			MySwap(arr, index, max);
			HeapAdjust(arr,max,len);
		}

	}
}

//�ѵ���
void HeapAdjust2(int arr[], int index, int len){

	//�������±�
	int lchild = index * 2 + 1;
	//�������±�
	int rchild = index * 2 + 2;

	int max = index;

	while (lchild < len || rchild < len){

		if (lchild < len && arr[lchild] > arr[max]){
			max = lchild;
		}

		if (rchild < len && arr[rchild] > arr[max]){
			max = rchild;
		}

		if (max != index){
			MySwap(arr, max, index);
			index = max; //index��ʾ��ǰҪ������λ��
			lchild = max * 2 + 1; //��ǰ����λ�õ���ڵ�
			rchild = max * 2 + 2; //��ǰ����λ�õ��ҽڵ�
		}
		else{
			break;
		}

	}

}

void HeapSort(int arr[],int len){

	//��ʼ����
	for (int i = len / 2 - 1; i >= 0; i --){
		//������
		HeapAdjust(arr,i,len);
	}


	for (int i = len - 1; i >= 0; i --){
		//�����Ѷ���ĩβԪ��
		MySwap(arr, 0, i);
		//������
		HeapAdjust(arr,0,i);
	}

}


int main(){

	int arr[MAX];
	srand((unsigned int)time(NULL));
	for (int i = 0; i < MAX; i++){
		arr[i] = rand() % MAX;
	}

	//����ǰ
	printArray(arr, MAX);
	//������
	HeapSort(arr, MAX);
	//�����
	printArray(arr, MAX);


	system("pause");
	return EXIT_SUCCESS;
}