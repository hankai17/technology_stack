#ifndef POINT_H
#define POINT_H

class Point{
public:
	void setX(int x);
	void setY(int y);
	int getX();
	int getY();
private:
	int mX; //x����
	int mY; //y����
};

#endif