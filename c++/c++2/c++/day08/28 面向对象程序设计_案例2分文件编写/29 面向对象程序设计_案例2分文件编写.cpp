#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
#include"Circle.h"

int main(){

	//ʵ����Բ����
	Circle circle;
	circle.setP(20, 20);
	circle.setR(5);
	//ʵ���������
	Point point;
	point.setX(25);
	point.setY(20);

	circle.IsPointInCircle(point);

	system("pause");
	return EXIT_SUCCESS;
}