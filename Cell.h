//
// Created by Xiang Li on 2019-04-03.
//

#ifndef PROJECT_CELL_H
#define PROJECT_CELL_H

class Cell
{
public:
    int x, y;
    int visited;
    Cell();
    Cell(int x, int y,int visited);
    ~Cell();
};

#endif //PROJECT_CELL_H
