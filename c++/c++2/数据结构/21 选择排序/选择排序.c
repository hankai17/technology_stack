#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define MAX 10

void printArray(int arr[], int len){
	for (int i = 0; i < len; i++){
		printf("%d ", arr[i]);
	}
	printf("\n");
}

//��������Ԫ��
void MySwap(int arr[], int i, int j){
	int temp = arr[i];
	arr[i] = arr[j];
	arr[j] = temp;
}

//ѡ������ ����
void SelectSort(int arr[],int len){

	for (int i = 0; i < len; i ++){
		int min = i;
		for (int j = i; j < len; j ++){
			if (arr[j] < arr[min]){
				min = j;
			}
		}
		if (min != i){
			MySwap(arr, min, i);
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
	printArray(arr, MAX);
	//ð������
	SelectSort(arr, MAX);
	//�����
	printArray(arr, MAX);


	system("pause");
	return EXIT_SUCCESS;
}