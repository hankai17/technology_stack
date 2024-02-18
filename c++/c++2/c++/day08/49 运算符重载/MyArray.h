#ifndef MYARRAY_H
#define MYARRAY_H

class MyArray{

public:
	//�޲ι��캯�����û�û��ָ�����������ʼ��Ϊ100
	MyArray();
	//�вι��캯�����û�ָ��������ʼ��
	MyArray(int capacity);

	//�û������ӿ�

	//����λ�����Ԫ��
	void SetData(int pos, int val);
	//���ָ��λ������
	int GetData(int pos);
	//β�巨
	void PushBack(int val);
	//��ó���
	int GetLength();
	//�����������ͷ�����ռ�

	//-----------����[]������ start-------
	int& operator[](int index){
		return pAdress[index];
	}
	//-----------����[]������ end-------

	~MyArray();

private:
	int mCapacity; //����һ�������ɶ��ٸ�Ԫ��
	int mSize; //��ǰ�ж��ٸ�Ԫ��
	int* pAdress; //ָ��洢���ݵĿռ�

};


#endif