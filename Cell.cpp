//
// Created by Xiang Li on 2019-04-03.
//
#include "Cell.h"

Cell::Cell()
{}

Cell::Cell(int x, int y,int visited)
{
    this->x = x;
    this->y = y;
    this->visited = visited;
}

Cell::~Cell()
{}
