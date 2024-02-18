#ifndef CIRCLE_H
#define CIRCLE_H

#include"Point.h"
#include<iostream>

class Circle{
public:
	//����Բ��
	void setPoint(int x, int y);
	//���ð뾶
	void setR(int r);
	//���ذ뾶
	int getR();
	//����Բ��
	Point getPoint();
	void IsInCircle(Point& point);

private:
	Point mPoint; //Բ��
	int mR; //�뾶
};

#endif