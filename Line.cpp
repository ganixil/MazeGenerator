
#include "Line.h"
#include "Point.h"
CLine::CLine()
{}

CLine::CLine(int pt1, int pt2)
{
	pts[0] = pt1;
	pts[1] = pt2;
}
CLine::CLine(CPoint pt1, CPoint pt2) {
	this->pt1 = pt1;
	this->pt2 = pt2;
}
CLine::~CLine()
{}