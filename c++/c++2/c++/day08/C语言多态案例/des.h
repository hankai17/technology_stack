/*********************************************************
 *  des.h
 *  �û�ʹ��des�㷨ͷ�ļ�
 *	
 *********************************************************/
#ifndef _OPENDESS_H_
#define _OPENDESS_H_

#ifdef __cplusplus
extern "C" {
#endif

//�������ݣ�encrypt�����ܣ�
//pInData����������
//nInDataLen���������ݳ���
//pOutData�����ܺ������
//pOutDataLen���������ݵĳ���
int DesEnc(
		unsigned char *pInData,
		int            nInDataLen,
		unsigned char *pOutData,
		int           *pOutDataLen);


//��������(decrypt������)
//pInData����������
//nInDataLen���������ݳ���
//pOutData�����ܺ������
//pOutDataLen���������ݵĳ���
int DesDec(
	   unsigned char *pInData,
	   int            nInDataLen,
	   unsigned char *pOutData,
	   int           *pOutDataLen);

#ifdef __cplusplus
}
#endif

#endif

