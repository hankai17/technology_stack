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

//��������
void QuickSort(int arr[],int start, int end){

	int i = start;
	int j = end;
	//ȡ��׼��
	int target = arr[start];
	if (i < j){
		
		while (i < j){
			
			//���������ұȻ�׼�����
			while (i < j && arr[j] < target){
				j--;
			}
			if (i < j){
				arr[i] = arr[j];
				i++;
			}

			//���������ұȻ�׼��С��
			while (i < j && arr[i] > target){
				i++;
			}
			if (i < j){
				arr[j] = arr[i];
				j--;
			}
		}
		
		//i��λ�þ��ǻ�׼������ʵ�λ��
		arr[i] = target;
		//����������벿��
		QuickSort(arr,start,i - 1);
		//���������а벿��
		QuickSort(arr, i + 1, end);

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
	QuickSort(arr, 0,MAX - 1);
	//�����
	printArray(arr, MAX);

	system("pause");
	return EXIT_SUCCESS;
}