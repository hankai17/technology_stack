#include"Framework.h"


//���ͺͽ�������
void FrameWork(CSocketProtocol* csp, unsigned char* sendBuf, int sendBufLen, unsigned char* recvBuf, int* recvBufLen){

	void* handle = NULL;
	int ret = 0;
	//ͨ�ų�ʼ��
	if ((ret = csp->init(&handle)) != 0){
		printf("csp->init error: %d\n", ret);
		return;
	}

	//��������
	if ((ret = csp->send(handle, sendBuf, sendBufLen) != 0)){
		printf("csp->send error: %d\n", ret);
		csp->destroy(&handle);
		return;
	}

	//��������
	if ((ret = csp->recv(handle, recvBuf, recvBufLen)) != 0){
		printf("csp->recv error: %d\n", ret);
		csp->destroy(&handle);
		return;
	}

	//��������
	if ((ret = csp->destroy(&handle)) != 0){
		printf("csp->destroy error: %d\n", ret);
	}
}

//���ͺͽ�������
void FrameWorkPro(CSocketProtocol* csp,
	EncDec* ed,
	unsigned char* sendBuf, int sendBufLen,
	unsigned char* recvBuf, int* recvBufLen){

	void* handle = NULL;
	int ret = 0;
	//ͨ�ų�ʼ��
	if ((ret = csp->init(&handle)) != 0){
		printf("csp->init error: %d\n", ret);
		return;
	}

	//������ܺ������
	unsigned char encData[1024] = { 0 };
	int encLen = 0;
	//����
	ed->enc(sendBuf, sendBufLen, encData, &encLen);

	//��������
	if ((ret = csp->send(handle, encData, encLen) != 0)){
		printf("csp->send error: %d\n", ret);
		csp->destroy(&handle);
		return;
	}


	unsigned char decData[1024] = { 0 };
	int decLen = 0;
	//��������
	if ((ret = csp->recv(handle, decData, &decLen)) != 0){
		printf("csp->recv error: %d\n", ret);
		csp->destroy(&handle);
		return;
	}
	//���ݽ���
	ed->dec(decData, decLen, recvBuf, recvBufLen);

	printf("��������:%s\n", decData);

	//��������
	if ((ret = csp->destroy(&handle)) != 0){
		printf("csp->destroy error: %d\n", ret);
	}
}
