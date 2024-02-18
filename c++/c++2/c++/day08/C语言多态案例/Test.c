#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"CSckImp1.h"
#include"CSckImp2.h"
#include"CSocketProtocol.h"
#include"CEncDesProtocol.h"
#include"CHwImp.h"
#include"Framework.h"

//����ͨ�ſ��
void TestSocketProtocol01(){
	
	CSocketProtocol csck;
	//ʹ�õ�һ�����̵�ͨ�Žӿ�
	//Init_CSocketProtocol(&csck, Init_SocketClinet1, Send_SocketClient1, 
	//	Recv_SocketClient1, Destroy_SocketClient1);

	//ʹ�õڶ������̵�ͨ�Žӿ�
	Init_CSocketProtocol(&csck, Init_SocketClinet2, Send_SocketClient2,
		Recv_SocketClient2, Destroy_SocketClient2);

	unsigned char* sendBuf = "hello wrold!";
	int sendBufLen = strlen(sendBuf) + 1;

	unsigned char recvBuf[1024] = { 0 };
	int recvBufLen = 0;

	int ret = framework(&csck, sendBuf, sendBufLen, recvBuf, &recvBufLen);
	if (ret != 0){
		printf("framework error:%d\n", ret);
	}

	printf("���յ�������:%s\n",recvBuf);
	printf("���յ������ݳ���:%d\n",recvBufLen);
}

//���Ӽ��ܹ���ͨ�ſ��
void TestSocketProtocol02(){

	CSocketProtocol csck;
	//ʹ�õ�һ�����̵�ͨ�Žӿ�
	//Init_CSocketProtocol(&csck, Init_SocketClinet1, Send_SocketClient1, 
	//	Recv_SocketClient1, Destroy_SocketClient1);

	//ʹ�õڶ������̵�ͨ�Žӿ�
	Init_CSocketProtocol(&csck, Init_SocketClinet2, Send_SocketClient2,
		Recv_SocketClient2, Destroy_SocketClient2);

	//ʹ�û�Ϊ���ܽӿ�
	CEncDesProtocol ced;
	Init_CEncDesProtocol(&ced, EncDataHw, DecDataHw);

	unsigned char* sendBuf = "hello wrold!";
	int sendBufLen = strlen(sendBuf) + 1;

	unsigned char recvBuf[1024] = { 0 };
	int recvBufLen = 0;

	unsigned char tempBuf[1024] = { 0 };

	int ret = frameworkPro(&csck, &ced, sendBuf, sendBufLen, recvBuf, &recvBufLen);
	if (ret != 0){
		printf("framework error:%d\n", ret);
	}

	printf("���յ�������:%s\n", recvBuf);
	printf("���յ������ݳ���:%d\n", recvBufLen);
}

int main(){

	//TestSocketProtocol01();
	TestSocketProtocol02();

	system("pause");
	return EXIT_SUCCESS;
}