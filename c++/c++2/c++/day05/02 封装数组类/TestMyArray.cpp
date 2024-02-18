#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;
#include"MyArray.h"

void test01(){

	MyArray* mArray = new MyArray(30);
	//�����в���Ԫ��
	for (int i = 0; i < mArray->GetCapacity(); i++){
		mArray->SetData(i, i + 1);
	}
	//��ӡ������Ԫ��
	for (int i = 0; i < mArray->GetCapacity(); i++){
		cout << mArray->GetData(i) << " ";
	}
	cout << endl;

	cout << "Size:" << mArray->GetSize() << endl;
	cout << "Capacity:" << mArray->GetCapacity() << endl;

	delete mArray;
}

void test02(){
	
	MyArray* mArray = new MyArray(30);
	for (int i = 0; i < 10; i++){
		mArray->PushBack(i+1);
	}
	for (int i = 0; i < mArray->GetSize(); i++){
		cout << mArray->GetData(i) << " ";
	}
	cout << endl;
	cout << "Size:" << mArray->GetSize() << endl;
	cout << "Capacity:" << mArray->GetCapacity() << endl;

	//β��ɾ��
	mArray->PopBack();
	cout << "----------------" << endl;
	for (int i = 0; i < mArray->GetSize(); i++){
		cout << mArray->GetData(i) << " ";
	}
	cout << endl;
	cout << "Size:" << mArray->GetSize() << endl;
	cout << "Capacity:" << mArray->GetCapacity() << endl;

	//ָ��λ��ɾ��
	mArray->RemoveByPos(6);
	cout << "----------------" << endl;
	for (int i = 0; i < mArray->GetSize(); i++){
		cout << mArray->GetData(i) << " ";
	}
	cout << endl;
	cout << "Size:" << mArray->GetSize() << endl;
	cout << "Capacity:" << mArray->GetCapacity() << endl;

	delete mArray;
}

void test03(){

	MyArray mArray;
	for (int i = 0; i < 10; i++){
		//mArray.PushBack(i + 1);
		mArray[i] = 10 + i;
	}
	for (int i = 0; i < 10;i++){
		cout << mArray[i] << " ";
	}
	printf("\n");

}

int main(){

	//test02();
	test03();


	system("pause");
	return EXIT_SUCCESS;
}