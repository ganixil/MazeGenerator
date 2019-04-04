
// class for a 2D point (x, y)

#ifndef __POINT_H__
#define __POINT_H__

class CPoint
{
public:
	int x, y;
	int visited;
	CPoint();
	CPoint(int x, int y);
	CPoint(int x, int y, int visited);
	~CPoint();
	CPoint& operator+(CPoint &);
	CPoint& operator-(CPoint &);
	float norm();
};

#endif