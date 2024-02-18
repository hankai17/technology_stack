#include"Framework.h"

int framework(CSocketProtocol* socketProtocol,
	unsigned char* sendBuf, int sendBufLen,
	unsigned char* recvBuf, int* recvBufLen){

	if (socketProtocol == NULL){
		return -1;
	}

	if (sendBuf == NULL){
		return -2;
	}

	if (recvBuf == NULL){
		return -3;
	}

	if (recvBufLen == NULL){
		return -4;
	}

	//�������
	void* handle = NULL;
	int ret = 0;
	//��ʼ��
	if ((ret = socketProtocol->init(&handle)) != 0){
		printf("socketProtocol->init error: %d\n", ret);
		goto End;
	}

	//��������
	if ((ret = socketProtocol->send(handle, sendBuf, sendBufLen)) != 0){
		printf("socketProtocol->send error: %d\n", ret);
		goto End;
	}

	//��������
	if ((ret = socketProtocol->recv(handle, recvBuf, recvBufLen)) != 0){
		printf("socketProtocol->recv error: %d\n", ret);
		goto End;
	}

End:
	if ((ret = socketProtocol->destroy(&handle)) != 0){
		printf("socketProtocol->destroy error: %d\n", ret);
		return ret;
	}
	handle = NULL;

	return 0;
}


int frameworkPro(CSocketProtocol* socketProtocol,
	CEncDesProtocol* cEncDec,
	unsigned char* sendBuf, int sendBufLen,
	unsigned char* recvBuf, int* recvBufLen){

	if (socketProtocol == NULL){
		return -1;
	}

	if (sendBuf == NULL){
		return -2;
	}

	if (recvBuf == NULL){
		return -3;
	}

	if (recvBufLen == NULL){
		return -4;
	}

	//�������
	void* handle = NULL;
	int ret = 0;
	//��ʼ��
	if ((ret = socketProtocol->init(&handle)) != 0){
		printf("socketProtocol->init error: %d\n", ret);
		goto End;
	}

	unsigned char tempBuf[1024] = { 0 };
	int tempBufLen = 0;
	cEncDec->enc(sendBuf, sendBufLen, tempBuf, &tempBufLen);
	printf("����ǰ������:%s \n", sendBuf);
	printf("���ܺ������:%s \n", tempBuf);

	//��������
	if ((ret = socketProtocol->send(handle, tempBuf, tempBufLen)) != 0){
		printf("socketProtocol->send error: %d\n", ret);
		goto End;
	}

	memset(tempBuf, 0, 1024);
	tempBufLen = 0;

	//��������
	if ((ret = socketProtocol->recv(handle, tempBuf, &tempBufLen)) != 0){
		printf("socketProtocol->recv error: %d\n", ret);
		goto End;
	}

	//���ݽ���
	cEncDec->dec(tempBuf, tempBufLen, recvBuf, recvBufLen);

End:
	if ((ret = socketProtocol->destroy(&handle)) != 0){
		printf("socketProtocol->destroy error: %d\n", ret);
		return ret;
	}
	handle = NULL;

	return 0;
}