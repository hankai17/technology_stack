#pragma once

#include<stdlib.h>

//��ʼ������
typedef int(*Init_Socket)(void** handle);
//���ͺ���
typedef int(*Send_Socket)(void* handle,unsigned char* buf,int bufLen);
//���պ���
typedef int(*Recv_Socket)(void* handle,unsigned char* buf,int* bufLen);
//���ٺ���
typedef int(*Destroy_Socket)(void** handle);


//ͨ��ģ��
typedef struct _CSocketProtocol{
	//��ʼ��
	Init_Socket init;
	//����
	Send_Socket send;
	//����
	Recv_Socket recv;
	//����
	Destroy_Socket destroy;
}CSocketProtocol;


//ͨ��ģ���ʼ��
int Init_CSocketProtocol(CSocketProtocol* csp, 
	Init_Socket init,
	Send_Socket send,
	Recv_Socket recv,
	Destroy_Socket destroy
	);