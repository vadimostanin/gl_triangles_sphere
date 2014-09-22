//============================================================================
// Name        : graphics_3d_move_rotation_scale.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <GL/glut.h>
#include <iostream>
using namespace std;
        
int x = 0, 
    y = 0, 
    z = 0;

static void _print_matrix(float *m)
{
    for(int i = 0 ; i < 4 ; i ++)
    {
        cout<<"|";
        for(int j = 0 ; j < 4 ; j++)
        {
            cout<<" "<<m[i*4 + j]<<" ";
        }
        cout<<"|"<<endl;
    }
}

static void reshape(int width, int height)
{
    cout << "reshape enter" << endl;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    gluOrtho2D(0, width, 0, height);
    glOrtho(0, width, height, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
    cout << "reshape leave" << endl;
}

static void display()
{
    cout << "display enter" << endl;
    
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glLineWidth(100);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2i(250, 450);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2i(250, 150);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2i(550, 150);
    glColor3f(0.0, 0.0, 1.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2i(550, 150);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2i(550, 450);
    glColor3f(1.0, 0.0, 1.0);
    glVertex2i(800, 450);
    
    float m[16] = {0};
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    _print_matrix(m);
    glTranslatef(x, y, z);
    
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    _print_matrix(m);
    
    glEnd();
    glFlush();
    glutSwapBuffers();
    cout << "display enter" << endl;
}

void keyboard(unsigned char key, int a, int b)
{
    cout<<"key="<<key<<"; a="<<a<<"; b="<<b<<flush;
}

void special_keyboard(int a, int b, int c)
{
    cout<<"a="<<a<<"; b="<<b<<"; c="<<c<<endl;
    
    switch(a)
    {
        case 100://LEFT
//            glTranslatef(1, 0, 0);
        {
            x++;
        }
        break;
        case 101://UP
        break;
        case 102://RIGHT
        break;
        case 103://DOWN
        break;
    }
}

int main(int argc, char **argv) {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Sample");
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_keyboard);
	glClearColor(0.5, 0.5, 0.5, 0);
	glutDisplayFunc(display);
	
	glutMainLoop();
	
	return 0;
}