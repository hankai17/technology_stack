#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
#include"Circle.h"

int main(){


	//����һ����
	Point point;
	point.setX(10);
	point.setY(15);

	//����һ��Բ
	Circle circle;
	circle.setPoint(10, 10);
	circle.setR(5);

	//����ȫ�ֺ���
	//IsInCircle(circle, point);
	//���ó�Ա����
	circle.IsInCircle(point);

	system("pause");
	return EXIT_SUCCESS;
}