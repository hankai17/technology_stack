#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define MAX 10

void printArray(int arr[],int len){
	for (int i = 0; i < len;i++){
		printf("%d ",arr[i]);
	}
	printf("\n");
}

//��������Ԫ��
void MySwap(int arr[],int i, int j){
	int temp = arr[i];
	arr[i] = arr[j];
	arr[j] = temp;
}

//ð������
void BubbleSort(int arr[], int len){

	for (int i = 0; i < len; i ++){
		for (int j = len - 1; j > i;j--){
			if (arr[j] < arr[j-1]){
				MySwap(arr,j,j-1);
			}
		}
	}

}

//ð������Ľ�
void BubbleSort2(int arr[],int len){
	
	int flag = 0; //û���ź�
	for (int i = 0; i < len && flag == 0; i++){
		flag = 1; //��Ϊ�Ѿ��ź�
		for (int j = len - 1; j > i; j --){
			if (arr[j] < arr[j-1]){
				flag = 0; //û���ź� ����һ��
				MySwap(arr,j,j-1);
			}
		}
	}

}

int main(){

	int arr[MAX];
	srand((unsigned int)time(NULL));
	for (int i = 0; i < MAX; i++){
		arr[i] = rand() % MAX;
	}

	//����ǰ
	printArray(arr,MAX);
	//ð������
	//BubbleSort(arr,MAX);
	BubbleSort2(arr, MAX);
	//�����
	printArray(arr, MAX);


	system("pause");
	return EXIT_SUCCESS;
}