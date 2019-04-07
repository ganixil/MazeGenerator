#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;
#include <algorithm>
#include <vector>
#include <array>
#include <math.h>
#include <iostream>
#include "tinyfiledialogs.h"
#include "Point.h"
#include "Line.h"
#include "Cell.h"



int winWidth = 800, winHeight = 600;
static int win(0);
char const * lTheOpenFileName;
char const * lFilterPatterns[2] = { "*.bmp","*.bitmap"};
FILE* image;


// left/right mouse button flag
int lmbDown = 0;
int rmbDown = 0;


// vertex and line list
vector<CPoint> vPts;
vector<CLine> vLines;
vector<CPoint> polygonPt;
vector<vector<CPoint>> polygon;
CPoint tmpPts[2];


vector<CPoint> ptstack;
vector<CLine> mazeRoute;
// keep track of the active anchor
int hover = -1, dragging = -1;
char keypress = NULL;


/* ************************************************ */

void init( void )
{
    // set background to white
    glClearColor(1.0, 1.0, 1.0, 0.0);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,winWidth, 0, winHeight);
    glMatrixMode (GL_MODELVIEW);
}

void getPixel(int x, int y, GLfloat *color){
    glReadPixels(x,y,1,1,GL_RGB,GL_FLOAT,color);
}
vector<int> check_surrending(int x, int y){
    vector<int> unvisited;
    int up=1, down=2, left=3, right=4;
    GLfloat rgb[3];
    //up
    getPixel(x,y+8,rgb);
    if(rgb[0] == 0.0 && rgb[1] == 0.0 && rgb[2] == 0.0){
        unvisited.push_back(up);
    }
    //down
    getPixel(x,y-8,rgb);
    if(rgb[0] == 0.0 && rgb[1] == 0.0 && rgb[2] == 0.0){
        unvisited.push_back(down);
    }
    //left
    getPixel(x-8,y,rgb);
    if(rgb[0] == 0.0 && rgb[1] == 0.0 && rgb[2] == 0.0){
        unvisited.push_back(left);
    }
    //right
    getPixel(x+8,y,rgb);
    if(rgb[0] == 0.0 && rgb[1] == 0.0 && rgb[2] == 0.0){
        unvisited.push_back(right);
    }
}
void drawMaze(CPoint start, vector<CPoint> cells, int offset){
    for(auto &p: cells){
        if(p.x == start.x && p.y == start.y){
            p.visited = 1;
            start = p;
        }
    }
    CPoint top;
    CPoint down;
    CPoint left;
    CPoint right;
    vector<CPoint> fourdir;
    for(auto &p: cells){
        //top
        if(p.x == start.x && p.y == (start.y) + offset){
            top = p;
            fourdir.push_back(top);
        }
        //down
        if(p.x == start.x && p.y == (start.y) - offset){
            down = p;
            fourdir.push_back(down);
        }
        //left
        if(p.x == (start.x)-offset && p.y == start.y){
            left = p;
            fourdir.push_back(left);
        }
        //right
        if(p.x == (start.x)+offset && p.y == start.y){
            right = p;

            fourdir.push_back(right);
        }
    }

    vector<CPoint> notvisited;
    for(auto &pt: fourdir){
        if(pt.visited == 0)
            notvisited.push_back(pt);
    }

    if(!notvisited.empty()){
        int dir = (int)(notvisited.size() * rand() / RAND_MAX);
        ptstack.push_back(start);
        mazeRoute.push_back(CLine(start,notvisited[dir]));
        drawMaze(notvisited[dir],cells,offset);
    } else if(!ptstack.empty()){
        CPoint pt = ptstack.back();
        ptstack.pop_back();
        drawMaze(pt,cells,offset);
    }

}
vector<CPoint> initCells(vector<CPoint> polygonShape, int offset){
    vector<CPoint> cells;
    //find bound box for polygons, currently only works for convex
    int l = winWidth ,r =0 ,t= 0,d = winHeight;
    for(auto &pt: polygonShape){
        if(pt.x >= r){
            r = pt.x;
        }
        if(pt.y >= t){
            t = pt.y;
        }
    }
    for(auto &pt: polygonShape){
        if(pt.x < l){
            l = pt.x;
        }
        if(pt.y < d){
            d = pt.y;
        }
    }

    int m,n;
    for(int j = d; j<= t;j+=offset){
        for(int i= l;i<= r;i+=offset){
            for(m=0,n=(int)polygonShape.size()-1;m<polygonShape.size();n=m++)
            {
                if((polygonShape[m].y > j) != (polygonShape[n].y>j) && ( i < (polygonShape[n].x - polygonShape[m].x)*(j - polygonShape[m].y)/(polygonShape[n].y - polygonShape[m].y)+polygonShape[m].x)){
                    cells.push_back(CPoint(i,j,0));
                }
            }
        }
    }
    return cells;
}

// Main drawing routine. Called repeatedly by GLUT's main loop
void display( void )
{
    //Clear the screen and set our initial view matrix
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i=0; i<vLines.size(); i++)
    {
        // draw lines
        glColor3f(0.0, 0.0, 0.0);
        glLineWidth(1.0);
        glBegin(GL_LINES);
        glVertex2i(vPts[ vLines[i].pts[0] ].x, vPts[ vLines[i].pts[0] ].y);
        glVertex2i(vPts[ vLines[i].pts[1] ].x, vPts[ vLines[i].pts[1] ].y);
        glEnd();
    }

    glColor3f(0.0, 0.0, 0.0);
    for(int i=0;i<polygon.size();i++){
        glBegin(GL_POLYGON);
        for(int j=0;j<polygon[i].size();j++){
            glVertex2i(polygon[i][j].x,polygon[i][j].y);
        }
        glEnd();
    }

    // if in drawing state, draw the current line too
    if (lmbDown == 1) {
        glColor3f(0.0, 0.0, 0.0);
        glLineWidth(1.0);
        glBegin(GL_LINES);
        glVertex2i(tmpPts[0].x, tmpPts[0].y);
        glVertex2i(tmpPts[1].x, tmpPts[1].y);
        glEnd();
    }
    for(auto &l : mazeRoute){
        glColor3f(1.0,1.0,1.0);
        glLineWidth(1.0);
        glBegin(GL_LINES);
        glVertex2i(l.pt1.x,l.pt1.y);
        glVertex2i(l.pt2.x,l.pt2.y);
        glEnd();
    }
    glutSwapBuffers();
}

// mouse click event handler
void mouse(int button, int state, int x, int y)
{
    // set the mouse button flags
    // and store the mouse position to determine drag distance

    // left button down - drawing mode
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        lmbDown = 1;
        tmpPts[0].x = x;
        tmpPts[0].y = winHeight-y;
        tmpPts[1].x = x;
        tmpPts[1].y = winHeight-y;

    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        // store the new line
        lmbDown = 0;
        vPts.push_back( tmpPts[0] );
        vPts.push_back( tmpPts[1] );
        polygonPt.push_back(tmpPts[0]);
        vLines.push_back( CLine(vPts.size()-2, vPts.size()-1) );
        glutPostRedisplay();
    }
    else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
        polygonPt.push_back(tmpPts[1]);
        polygon.push_back(polygonPt);
        polygonPt.clear();
    }

}

// mouse move event handler
void motion(int x, int y)
{
    float tempPos[2];

    if (lmbDown == 1)
    {
        // left mouse button down - drawing mode
        tmpPts[1].x = x;
        tmpPts[1].y = winHeight-y;
    }

    glutPostRedisplay();
}

void pmotion( int x, int y )
{

    bool bFound = false;


    for (int i=0; i<vPts.size(); i++)
    {

        if ( (x-vPts[i].x<3./winWidth
              && x-vPts[i].x>-3./winWidth)
             && (y-vPts[i].y<3./winHeight
                 && y-vPts[i].y>-3./winHeight) )
        {
            hover = i;
            bFound = true;
            break;
        }
    }
    if (!bFound)
        hover = -1;

    glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y){
    if (key == 'n'){
        lTheOpenFileName = tinyfd_openFileDialog(
                "Please choose a BMP file",
                "",
                2,
                lFilterPatterns,
                NULL,
                0);
        keypress = NULL;
    }
    else if(key == 'f'){
        vector<CPoint> cells;
        if(!polygon.empty()) {
            cells = initCells(polygon[0], 8);
            int start = (int)(cells.size() * rand() / RAND_MAX);
            drawMaze(cells[start],cells,8);
        }
    }
    else if(key == 'p'){
        keypress = 'p';
    }
}
// Entry point - GLUT setup and initialization
int main( int argc, char** argv )
{

    glutInit( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (winWidth, winHeight);
    glutInitWindowPosition (100, 100);
    glutCreateWindow( "MazeGenerator" );
    glutDisplayFunc( display );

    // register mouse functions for mouse keys and
    // mouse moves with/without mouse clicks
    // and keyboard presses
    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    glutPassiveMotionFunc( pmotion );
    glutKeyboardFunc(keyboard);


    init();

    glutMainLoop();

    return 0;
}