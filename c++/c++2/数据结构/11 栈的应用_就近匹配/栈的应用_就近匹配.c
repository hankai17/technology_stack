#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"SeqStack.h"

int IsLeft(char ch){
	return ch == '(';
}
int IsRight(char ch){
	return ch == ')';
}

typedef struct _INFO{
	char* p;
	int pos;
}Info;


Info* CreateInfo(char* p,int pos){
	Info* info = (Info*)malloc(sizeof(Info));
	info->p = p;
	info->pos = pos;
	return info;
}
//��ӡ����
void printError(char* exp,int pos,char* errMsg){

	printf("%s\n", errMsg);
	printf("%s\n",exp);
	for (int i = 0; i < pos;i++){
		printf(" ");
	}
	printf("A\n");
	printf("----------------------\n");
}
int main(){

	char* str = "5+5*(6))+9/3*1)-(1+3(";

	//��ʼ��ջ
	SeqStack stack = Init_SeqStack();
	//�����ַ���
	char* p = str;
	int pos = 0;
	while (*p != '\0'){
		
		//��������ջ
		if (IsLeft(*p)){	
			Push_SeqStack(stack, CreateInfo(p, pos));
		}
		
		//����ջ������
		if (IsRight(*p)){
			if (Size_SeqStack(stack) == 0){
				printError(str,pos,"�����Ų�ƥ��!");
			}
			else{
				Info* info = (Info*)Top_SeqStack(stack);	
				if (IsLeft(*info->p)){
					Pop_SeqStack(stack);
					free(info);
				}
			}
		}
		p++;
		pos++;
	}

	while (Size_SeqStack(stack) > 0){
		Info* info = (Info*)Top_SeqStack(stack);
		Pop_SeqStack(stack);
		printError(str,info->pos,"�����Ų�ƥ��!");
		free(info);
	}


	system("pause");
	return EXIT_SUCCESS;
}