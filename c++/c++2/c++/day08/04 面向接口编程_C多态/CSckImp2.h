#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

//��ʼ������
int Init_Socket2(void** handle);
//���ͺ���
int Send_Socket2(void* handle, unsigned char* buf, int bufLen);
//���պ���
int Recv_Socket2(void* handle, unsigned char* buf, int* bufLen);
//���ٺ���
int Destroy_Socket2(void** handle);