#include"Circle.h"

void Circle::setP(int x, int y){
	mP.setX(x);
	mP.setY(y);
}
void Circle::setR(int r){
	mR = r;
}
Point& Circle::getP(){
	return mP;
}
int Circle::getR(){
	return mR;
}
//�жϵ��Բ�Ĺ�ϵ
void Circle::IsPointInCircle(Point& point){
	int distance = (point.getX() - mP.getX()) * (point.getX() - mP.getX()) + (point.getY() - mP.getY()) * (point.getY() - mP.getY());
	int radius = mR * mR;
	if (distance < radius){
		cout << "Point(" << point.getX() << "," << point.getY() << ")��Բ��!" << endl;
	}
	else if (distance > radius){
		cout << "Point(" << point.getX() << "," << point.getY() << ")��Բ��!" << endl;
	}
	else{
		cout << "Point(" << point.getX() << "," << point.getY() << ")��Բ��!" << endl;
	}
}
