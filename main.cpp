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

float oldPos[2], newPos[2];
float worldPos[2];
// vertex and line list
vector<CPoint> vPts;
vector<CLine> vLines;
CPoint tmpPts[2];
int winpoints[2];

vector<CPoint> visited;
vector<CPoint> ptstack;

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

    glColor3f(0.0,0.0,0.0);
    glPointSize(8.0);
    glBegin(GL_POINTS);
    glVertex2f(start.x,start.y);
    glEnd();

    for(auto p: cells){
        if(p.x == start.x && p.y == start.y){
            p.visited = 1;
        }
    }
    CPoint top;
    CPoint down;
    CPoint left;
    CPoint right;
    for(auto p: cells){
        //top
        if(p.x == start.x && p.y == (start.y) + offset){
            top = p;
        }
        //down
        if(p.x == start.x && p.y == (start.y) - offset){
            down = p;
        }
        //left
        if(p.x == (start.x)-offset && p.y == start.y){
            left = p;
        }
        //right
        if(p.x == (start.x)+offset && p.y == start.y){
            right = p;
        }
    }
    vector<CPoint> fourdir;
    fourdir.push_back(top);
    fourdir.push_back(down);
    fourdir.push_back(left);
    fourdir.push_back(right);

    vector<CPoint> notvisited;
    for(auto pt: fourdir){
        if(pt.visited == 0)
            notvisited.push_back(pt);
    }
    //random number
    if(!notvisited.empty()){
        int dir = rand() % (int)(notvisited.size());
    } else if(!ptstack.empty()){

    }








}
// Main drawing routine. Called repeatedly by GLUT's main loop
void display( void )
{
    //Clear the screen and set our initial view matrix
    glClear(GL_COLOR_BUFFER_BIT);


    switch(keypress){
        case 'n':
            lTheOpenFileName = tinyfd_openFileDialog(
                    "Please choose a BMP file",
                    "",
                    2,
                    lFilterPatterns,
                    NULL,
                    0);
            keypress = NULL;
    }
    for (int i=0; i<vLines.size(); i++)
    {
        // draw lines
        glColor3f(0.0, 0.0, 0.0);
        glLineWidth(8.0);
        glBegin(GL_LINES);
        glVertex2i(vPts[ vLines[i].pts[0] ].x, vPts[ vLines[i].pts[0] ].y);
        glVertex2i(vPts[ vLines[i].pts[1] ].x, vPts[ vLines[i].pts[1] ].y);
        glEnd();
    }

    // if in drawing state, draw the current line too
    if (lmbDown == 1) {
        glColor3f(0.0, 0.0, 0.0);
        glLineWidth(8.0);
        glBegin(GL_LINES);
        glVertex2i(tmpPts[0].x, tmpPts[0].y);
        glVertex2i(tmpPts[1].x, tmpPts[1].y);
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
        vLines.push_back( CLine(vPts.size()-2, vPts.size()-1) );
        glutPostRedisplay();
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
        keypress='n';
    }
    else if(key == 'f'){
        keypress = 'f';
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