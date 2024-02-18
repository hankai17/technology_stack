#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"CSocketProtocol.h"
#include"CSckImp1.h"
#include"CSckImp2.h"
#include"CEncDesProtocol.h"
#include"CHwImp.h"
#include"Framework.h"


void TestFramework01(){

	CSocketProtocol csp;

	//���ó���һ�Ĵ���:
	//Init_CSocketProtocol(&csp,Init_Socket1,Send_Socket1,Recv_Socket1,Destroy_Socket1);
	//���ó��̶��Ĵ���:
	Init_CSocketProtocol(&csp, Init_Socket2, Send_Socket2, Recv_Socket2, Destroy_Socket2);

	//��ʱ�˿�  csp�����еĺ���ָ�룬��ָ����庯��

	//��������
	unsigned char* sendBuf = "hello world";
	int sendBufLen = strlen(sendBuf) + 1;

	//��������
	unsigned char recvBuf[1024] = { 0 };
	int recvBufLen = 0;

	FrameWork(&csp, sendBuf, sendBufLen, recvBuf, &recvBufLen);

	printf("���յ������ݳ���:%d\n", recvBufLen);
	printf("���յ�������:%s\n", recvBuf);
}

void TestFramework02(){

	CSocketProtocol csp;

	//���ó���һ�Ĵ���:
	//Init_CSocketProtocol(&csp,Init_Socket1,Send_Socket1,Recv_Socket1,Destroy_Socket1);
	//���ó��̶��Ĵ���:
	Init_CSocketProtocol(&csp, Init_Socket2, Send_Socket2, Recv_Socket2, Destroy_Socket2);

	//���ܽӿ�
	EncDec ed;
	Init_EncDec(&ed, EncDataHw, DecDataHw);

	//��ʱ�˿�  csp�����еĺ���ָ�룬��ָ����庯��

	//��������
	unsigned char* sendBuf = "hello world";
	int sendBufLen = strlen(sendBuf) + 1;

	//��������
	unsigned char recvBuf[1024] = { 0 };
	int recvBufLen = 0;

	FrameWorkPro(&csp, &ed, sendBuf, sendBufLen, recvBuf, &recvBufLen);

	printf("���յ������ݳ���:%d\n", recvBufLen);
	printf("���յ�������:%s\n", recvBuf);
}

int main(){

	//TestFramework01();
	TestFramework02();

	system("pause");
	return EXIT_SUCCESS;
}