#pragma once

#include<stdlib.h>

//��������
typedef int(*EncData)(unsigned char* inBuf, int inBufLen, unsigned char* outBuf, int* outBufLen);
typedef int(*DecData)(unsigned char* inBuf, int inBufLen, unsigned char* outBuf, int* outBufLen);

typedef struct _EncDec{
	//���ܺ���
	EncData enc;
	//���ܺ���
	DecData dec;
}EncDec;

int Init_EncDec(EncDec* ed, EncData enc, DecData dec);
