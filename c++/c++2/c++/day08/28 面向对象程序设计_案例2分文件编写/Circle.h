#pragma once

#include<iostream>
using namespace std;
#include"Point.h"

//Բ��
class Circle{
public:
	void setP(int x, int y);
	void setR(int r);
	Point& getP();
	int getR();
	//�жϵ��Բ�Ĺ�ϵ
	void IsPointInCircle(Point& point);
private:
	Point mP; //Բ��
	int mR; //�뾶
};
