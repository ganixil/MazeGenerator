
#include "Point.h"
#include <cmath>

CPoint::CPoint()
{}

CPoint::CPoint(float x, float y)
{
	this->x = x;
	this->y = y;
}

CPoint::~CPoint()
{}

CPoint& CPoint::operator+(CPoint &r)
{
	CPoint result;
	result.x = x + r.x;
	result.y = x + r.y;
	return result;
}

CPoint& CPoint::operator-(CPoint &r)
{
	CPoint result;
	result.x = x - r.x;
	result.y = y - r.y;
	return result;
}

float CPoint::norm()
{
	return sqrt(x*x + y*y);
}