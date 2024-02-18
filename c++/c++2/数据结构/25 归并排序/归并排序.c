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

//�ϲ�������������
void Merge(int arr[], int start, int end, int mid, int tempSpace[]){
	
	int iStart = start;
	int iEnd = mid;
	int jStart = mid+1;
	int jEnd = end;
	int length = 0;

	while (iStart <= iEnd && jStart <= jEnd){
		
		if (arr[iStart] < arr[jStart]){
			tempSpace[length] = arr[iStart];
			iStart++;
		}
		else{
			tempSpace[length] = arr[jStart];
			jStart++;
		}

		length++;
	}

	//������������һ������ʣ��Ԫ��
	while (iStart <= iEnd){
		tempSpace[length] = arr[iStart];
		iStart++;
		length++;
	}

	while (jStart <= jEnd){
		tempSpace[length] = arr[jStart];
		jStart++;
		length++;
	}

	//����ԭ�ռ������
	for (int i = 0; i < length;i++){
		arr[start + i] = tempSpace[i];
	}

}

//�鲢����
void MergeSort(int arr[],int start,int end,int tempSpace[]){

	if (start == end){
		return;
	}

	int mid = (start + end) / 2;
	//�����벿��
	MergeSort(arr, start, mid, tempSpace);
	//����Ұ벿��
	MergeSort(arr, mid + 1, end, tempSpace);
	//������� �ϲ���������
	Merge(arr,start,end,mid,tempSpace);
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
	int tempSpace[MAX];
	MergeSort(arr, 0, MAX - 1, tempSpace);
	//�����
	printArray(arr, MAX);


	system("pause");
	return EXIT_SUCCESS;
}