#pragma once

//��ʼ��
typedef int(*Init_CSocket)(void** handle);
//����
typedef int(*Send_CSocket)(void* handle,unsigned char* buf,int bufLen);
//����
typedef int(*Recv_CSocket)(void* handle, unsigned char* buf, int* bufLen);
//����
typedef int(*Destroy_CSocket)(void** handle);


typedef struct _CSocketProtocol{
	//��ʼ��
	Init_CSocket init;
	//����
	Send_CSocket send;
	//����
	Recv_CSocket recv;
	//����
	Destroy_CSocket destroy;
}CSocketProtocol;

//��ʼ��CSocketProtocol
void Init_CSocketProtocol(CSocketProtocol* sp, Init_CSocket init, Send_CSocket send, Recv_CSocket recv, Destroy_CSocket destroy);