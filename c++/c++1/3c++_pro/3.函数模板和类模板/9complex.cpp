#include <iostream>
using namespace std;
#include"9complex.h"



template <typename T>
Complex<T>::Complex(T a, T b)//���캯��д������ⲿ
//1��Ϊ��ģ�庯�� ��template
//2��Ϊ���ຯ�� �������������˲����б�<T>��
{
	this->a = a;
	this->b = b;
}

template <typename T>       //��ͨ��Ա����д������ⲿ
//1��Ϊ��ģ�庯�� ��template
//2��Ϊ���ຯ�� �������������˲����б�<T>��
void Complex<T>::printCom()
{
	cout << "a:" << a << "b:" << endl;
}

template <typename T>
Complex<T> Complex<T>:: operator+ (Complex<T> &c2)//����ֵҲҪ���廯 ����ҲҪ���廯
{
	Complex <T>tmp(a + c2.a, b + c2.b); //����ɼӿɲ���
	return tmp;
}

template <typename T>
Complex<T> Complex<T>:: operator- (Complex<T> &c2)//����ֵҲҪ���廯 ����ҲҪ���廯
{
	Complex <T> tmp(a - c2.a, b - c2.b); //����ɼӿɲ���
	return tmp;
}

template <typename T>//��Ԫ����Ҳ��ģ�庯��  ��Ԫ��������ȫ�ֺ���������������� ����Ҫ�������÷�
ostream & operator << (ostream &out, Complex<T> &c3)//��Ԫ����д������Ƚϸ���
{
	out << c3.a << " + " << c3.b << "i" << endl;
	return out;
}


