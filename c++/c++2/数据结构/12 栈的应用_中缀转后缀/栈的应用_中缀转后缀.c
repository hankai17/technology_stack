#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"SeqStack.h"


int IsNumber(char ch){
	return ch >= '0' && ch <= '9';
}
int IsLeft(char ch){
	return ch == '(';
}
int IsRight(char ch){
	return ch == ')';
}
int IsOperator(char ch){
	return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}
int GetPriotity(char ch){
	if (ch == '*' || ch == '/'){
		return 2;
	}

	if(ch == '+' || ch == '-'){
		return 1;
	}

	return 0;
}


int main(){

	char* str = "8+(3-1)*5";
	char* p = str;

	//��ʼ��ջ
	SeqStack stack = Init_SeqStack();

	while(*p != '\0'){
		
		//����ֱ�����
		if (IsNumber(*p)){
			printf("%c",*p);
		}
		//����
		else{
		
			//�����Ž�ջ
			if (IsLeft(*p)){
				Push_SeqStack(stack, p);
			}

			//�������
			if (IsOperator(*p)){
			
				while (Size_SeqStack(stack) > 0 &&  GetPriotity(*(char*)Top_SeqStack(stack)) >= GetPriotity(*p) ){
					//���ջ������
					printf("%c", *(char*)Top_SeqStack(stack));
					Pop_SeqStack(stack);
				}
				//��ǰ������ջ
				Push_SeqStack(stack, p);
			}

			//������ ��ջ��ƥ��������
			if (IsRight(*p)){
				
				while (Size_SeqStack(stack) > 0 && !IsLeft(*(char*)Top_SeqStack(stack))){
					printf("%c",*(char*)Top_SeqStack(stack));
					Pop_SeqStack(stack);
				}
				//����������
				Pop_SeqStack(stack);
			}

		}
		p++;
	}

	//����ջ��Ԫ��
	while (Size_SeqStack(stack) > 0){
		printf("%c", *(char*)Top_SeqStack(stack));
		Pop_SeqStack(stack);
	}

	system("pause");
	return EXIT_SUCCESS;
}