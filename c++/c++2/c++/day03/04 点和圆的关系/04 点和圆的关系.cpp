#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//����
class Point{
public:
	void setX(int x){ mX = x; }
	void setY(int y){ mY = y; }
	int getX(){ return mX; }
	int getY(){ return mY; }
private:
	int mX; //x����
	int mY; //y����
};


//Բ��
class Circle{
public:
	//����Բ��
	void setPoint(int x,int y){
		mPoint.setX(x);
		mPoint.setY(y);
	}
	//���ð뾶
	void setR(int r){
		mR = r;
	}
	//���ذ뾶
	int getR(){ return mR; }
	//����Բ��
	Point getPoint(){ return mPoint; }

	void IsInCircle(Point& point){
		int distance = (getPoint().getX() - point.getX()) * (getPoint().getX() - point.getX()) + (getPoint().getY() - point.getY()) * (getPoint().getY() - point.getY());
		int r = getR() * getR();
		if (distance == r){
			cout << "Point(" << point.getX() << "," << point.getY() << ")��Բ��!" << endl;
		}
		else if (distance > r){
			cout << "Point(" << point.getX() << "," << point.getY() << ")��Բ��!" << endl;
		}
		else{
			cout << "Point(" << point.getX() << "," << point.getY() << ")��Բ��!" << endl;
		}

	}

private:
	Point mPoint; //Բ��
	int mR; //�뾶
};


void IsInCircle(Circle& circle,Point& point){
	int distance = (circle.getPoint().getX() - point.getX()) * (circle.getPoint().getX() - point.getX()) + (circle.getPoint().getY() - point.getY()) * (circle.getPoint().getY() - point.getY());
	int r = circle.getR() * circle.getR();
	if (distance == r){
		cout << "Point(" << point.getX() << "," << point.getY() << ")��Բ��!" << endl;
	}
	else if (distance > r){
		cout << "Point(" << point.getX() << "," << point.getY() << ")��Բ��!" << endl;
	}
	else{
		cout << "Point(" << point.getX() << "," << point.getY() << ")��Բ��!" << endl;
	}

}

int main(){


	//����һ����
	Point point;
	point.setX(10);
	point.setY(15);

	//����һ��Բ
	Circle circle;
	circle.setPoint(10,10);
	circle.setR(5);

	//����ȫ�ֺ���
	//IsInCircle(circle, point);
	//���ó�Ա����
	circle.IsInCircle(point);


	system("pause");
	return EXIT_SUCCESS;
}