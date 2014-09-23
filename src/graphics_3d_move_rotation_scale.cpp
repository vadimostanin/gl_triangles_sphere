//============================================================================
// Name        : graphics_3d_move_rotation_scale.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <unistd.h>
#include <GL/glut.h>
#include <iostream>
using namespace std;

class Person
{
public:
	class _Coord
	{
	public:
		double x,z;
	};
	class _Rotation
	{
	public:
		double y,x;
	};

	_Coord Coord;
	_Rotation Rotation;
};
//************************************************************************************
int MouseOld_x=0, MouseOld_y=0;
int WinWidth=640, WinHeight=480;
Person User;
//************************************************************************************
void Init(void);
void DrawEnvironment(void);
void Display(void);
void Keyboard(unsigned char key, int x, int y);
void Reshape(void);
void MouseMotion(int x, int y);
//************************************************************************************

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

void Init(void)
{
	//glEnable(GL_DEPTH_TEST);
	User.Coord.x=0;
	User.Coord.z=0;
	User.Rotation.x=0;
	User.Rotation.y=0;
}

void DrawEnvironment(void)
{
    glColor3f(1,1,1);
    for (int i = -100;i<100;i++)
    {
        glBegin(GL_LINES);
            glVertex3f(-100,-1,i);
            glVertex3f(-0,-1,i);
            glVertex3f(i,-1,100);
            glVertex3f(i,-1,-100);
        glEnd();
//        glutSwapBuffers();
//                    usleep(300);
    }
}

void Display(void)
{
	glClearColor(0.5,0.5,0.5,0.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	  glRotatef(User.Rotation.y, 0,1,0);
	  
	  float m[16] = {0};
      glGetFloatv(GL_MODELVIEW_MATRIX, m);
      _print_matrix(m);
      
	  glRotatef(User.Rotation.x, 1,0,0);
	  glGetFloatv(GL_MODELVIEW_MATRIX, m);
	  _print_matrix(m);

      glTranslatef(User.Coord.x,0,User.Coord.z);
      glGetFloatv(GL_MODELVIEW_MATRIX, m);
      _print_matrix(m);




	  DrawEnvironment();
	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
	if (key=='w') {User.Coord.z+=0.1;}
	if (key=='s') {User.Coord.z-=0.1;}
    if (key=='a') {User.Coord.x+=0.1;}
    if (key=='d') {User.Coord.x-=0.1;}
	if (key=='x') {User.Rotation.x++;}
	if (key=='y') {User.Rotation.y++;}
	glutPostRedisplay();
};
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!?  СОБСТВЕННО РЕАЛИЗАЦИЯ ПОВРОТА:
void MouseMotion(int x, int y)
{
	User.Rotation.x+=(y-MouseOld_y)/4;
	User.Rotation.y-=(x-MouseOld_x)/4;
	MouseOld_x=x;
	MouseOld_y=y;
	glutPostRedisplay();

};


void Reshape (int w, int h)
{
	WinWidth=w;
	WinHeight=h;
	glViewport(0,0,WinWidth,WinHeight);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120,WinWidth/WinHeight, 0.001, 80.0);//???

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutPostRedisplay();
}



int main(int argc, char** argv)
{
	// Initialization GLUT
	glutInit (&argc,argv);
	glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize (WinWidth,WinHeight);
    glutInitWindowPosition (100,100);
    glutCreateWindow ("Type Two Engine");
    // -------------------
    Init();
	// -------------------
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(MouseMotion);
	glutMainLoop();
	//--------------------
	return 0;
}
