#include"Circle.h"

//����Բ��
void Circle::setPoint(int x, int y){
	mPoint.setX(x);
	mPoint.setY(y);
}
//���ð뾶
void Circle::setR(int r){
	mR = r;
}
//���ذ뾶
int Circle::getR(){ return mR; }
//����Բ��
Point Circle::getPoint(){ return mPoint; }

void Circle::IsInCircle(Point& point){

	int distance = (getPoint().getX() - point.getX()) * (getPoint().getX() - point.getX()) + (getPoint().getY() - point.getY()) * (getPoint().getY() - point.getY());
	int r = getR() * getR();
	if (distance == r){
		std::cout << "Point(" << point.getX() << "," << point.getY() << ")��Բ��!" << std::endl;
	}
	else if (distance > r){
		std::cout << "Point(" << point.getX() << "," << point.getY() << ")��Բ��!" << std::endl;
	}
	else{
		std::cout << "Point(" << point.getX() << "," << point.getY() << ")��Բ��!" << std::endl;
	}

}