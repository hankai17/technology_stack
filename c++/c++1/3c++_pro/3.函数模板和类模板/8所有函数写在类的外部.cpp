#include <iostream>
using namespace std;
//���� ��Ԫ����ֻ��������<< >>ʱ���� ����ʱ��Ҫ����
template <typename T>
class Complex
{
	friend Complex MySub(Complex &c1, Complex &c2);
	

	friend ostream & operator << <T>(ostream &out, Complex &c3);//���������bug��������
	
public:
	Complex(T a, T b);
	void printCom();
	Complex operator +(Complex &c2);
	Complex operator -(Complex &c2);
protected:
private:
	T a;
	T b;

};
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
	Complex <T>tmp(a + c2.a , b + c2.b); //����ɼӿɲ���
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

 template <typename T>//???????????????????????????????????????
 Complex<T> MySub(Complex <T> &c1, Complex <T> &c2)
 {
	 Complex tmp(c1.a - c2.a, c1.b - c2.b);
	 return tmp;
 }


void main()
{
	Complex<int> c1(1, 2);
	Complex<int> c2(3, 4);

	Complex<int>c3 = c1 + c2;



	system("pause");
	return;
}



