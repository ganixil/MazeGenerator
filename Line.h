
// class to describe a line segment P1P2

#ifndef __LINE_H__
#define __LINE_H__

#include "Point.h"

class CLine
{
public:
	int pts[2];
	CPoint pt1;
	CPoint pt2;
	CLine();
	CLine(int pt1, int pt2);
	CLine(CPoint pt1, CPoint pt2);
	~CLine();
};

#endif