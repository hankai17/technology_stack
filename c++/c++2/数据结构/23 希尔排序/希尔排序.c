#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 10

void printArray(int arr[], int len){
	for (int i = 0; i < len; i++){
		printf("%d ", arr[i]);
	}
	printf("\n");
}

//ϣ������
void ShellSort(int arr[],int len){
	
	int increasement = len;
	do{
		//��������
		increasement = increasement / 3 + 1;
		for (int i = 0; i < increasement; i ++){
			//����ÿһ��
			for (int j = i + increasement; j < len; j += increasement){
				
				if (arr[j] < arr[j - increasement]){
					
					int temp = arr[j];
					int k = j - increasement;
					for (; k >= 0 && temp < arr[k];k -= increasement){
						arr[k + increasement] = arr[k];
					}
					arr[k + increasement] = temp;
				}
			}
		}
	} while (increasement > 1);

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
	ShellSort(arr, MAX);
	//�����
	printArray(arr, MAX);


	system("pause");
	return EXIT_SUCCESS;
}