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



int winWidth = 800, winHeight = 600;
static int win(0);
char const * lTheOpenFileName;
char const * lFilterPatterns[2] = { "*.bmp","*.bitmap"};
FILE* image;


// left/right mouse button flag
int lmbDown = 0;
int rmbDown = 0;

float oldPos[2], newPos[2];

// vertex and line list
vector<CPoint> vPts;
vector<CLine> vLines;
CPoint tmpPts[2];

// keep track of the active anchor
int hover = -1, dragging = -1;
char keypress = NULL;


/* ************************************************ */
void init( void )
{
    // set background to white
    glClearColor(1.0, 1.0, 1.0, 0.0);

    glMatrixMode (GL_PROJECTION);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode (GL_MODELVIEW);
}

// windows position -> world coordinate conversion
void win2world(float* worldpos, int x, int y)
{
    worldpos[0] = (float)x / winWidth * 2 - 1.0;
    worldpos[1] =1.0 - (float)y / winHeight * 2;
}

// Main drawing routine. Called repeatedly by GLUT's main loop
void display( void )
{
    //Clear the screen and set our initial view matrix
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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
    glPointSize(7.0);
    for (int i=0; i<vPts.size(); i++)
    {
        // set anchor color according to its state
        if (i == dragging)
        {
            glColor3f(1.0, 0.0, 0.0);
        }
        else if (i == hover)
        {
            glColor3f(0.6, 0.6, 0.0);
        }
        else
        {
            glColor3f(0.0, 1.0, 0.0);
        }
        glBegin(GL_POINTS);
        glVertex2f(vPts[i].x, vPts[i].y);
        glEnd();
    }

    for (int i=0; i<vLines.size(); i++)
    {
        // draw lines
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex2f(vPts[ vLines[i].pts[0] ].x, vPts[ vLines[i].pts[0] ].y);
        glVertex2f(vPts[ vLines[i].pts[1] ].x, vPts[ vLines[i].pts[1] ].y);
        glEnd();
    }

    // if in drawing state, draw the current line too
    if (lmbDown == 1)
    {
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex2f(tmpPts[0].x, tmpPts[0].y);
        glVertex2f(tmpPts[1].x, tmpPts[1].y);
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
        win2world(oldPos, x, y);
        tmpPts[0].x = oldPos[0];
        tmpPts[0].y = oldPos[1];
        tmpPts[1].x = oldPos[0];
        tmpPts[1].y = oldPos[1];
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        // store the new line
        lmbDown = 0;
        win2world(newPos, x, y);
        vPts.push_back( tmpPts[0] );
        vPts.push_back( tmpPts[1] );
        vLines.push_back( CLine(vPts.size()-2, vPts.size()-1) );
        glutPostRedisplay();
    }
        // right button down - moving mode
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        rmbDown = 1;
        win2world(oldPos, x, y);
        // set the vertex to edit
        dragging = hover;
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        rmbDown = 0;
        win2world(newPos, x, y);
        // end dragging
        dragging = -1;
    }
}

// mouse move event handler
void motion(int x, int y)
{
    float tempPos[2];
    win2world(tempPos, x, y);


    if (lmbDown == 1)
    {
        // left mouse button down - drawing mode
        tmpPts[1].x = tempPos[0];
        tmpPts[1].y = tempPos[1];
    }
    else if (rmbDown == 1)
    {
        // right mouse button down - moving mode
        if (dragging != -1)
        {
            vPts[dragging].x = tempPos[0];
            vPts[dragging].y = tempPos[1];
        }
    }

    glutPostRedisplay();
}

void pmotion( int x, int y )
{
    float tempPos[2];
    bool bFound = false;

    win2world(tempPos, x, y);
    for (int i=0; i<vPts.size(); i++)
    {

        if ( (tempPos[0]-vPts[i].x<3./winWidth
              && tempPos[0]-vPts[i].x>-3./winWidth)
             && (tempPos[1]-vPts[i].y<3./winHeight
                 && tempPos[1]-vPts[i].y>-3./winHeight) )
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