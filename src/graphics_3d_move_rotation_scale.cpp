// -- Written in C -- //

#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>
#include <math.h>
#include <time.h>
#include <iostream>
using namespace std;

Display                 *dpy;
Window                  root;
GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
Window                  win;
GLXContext              glc;
XWindowAttributes       gwa;
XEvent                  xev;

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

int window_width = WINDOW_WIDTH;
int window_height = WINDOW_HEIGHT;

float rotate_x = 0.0;
float rotate_y = 0.0;
float rotate_z = 0.0;
float scale_z = 1.0;

void InvalidateWindow()
{
    XExposeEvent event = {0};
//    memset(&event, 0x00, sizeof(event));

    event.type = Expose;
    event.send_event = true;
    event.display = dpy;
    event.window = win;
    event.width = WINDOW_WIDTH;
    event.height = WINDOW_HEIGHT;

    XSendEvent(dpy, win, False, ExposureMask, (XEvent *) &event);
}

void DrawAQuad()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10., 10., -10., 10., 1, 30.);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0., 0., 10., 0., 0., 0., 0,1,0);
    
    glRotatef(rotate_x, 0, 1, 0);
    glRotatef(rotate_y, 1, 0, 0);
    glRotatef(rotate_z, 0, 0, 1);
    
    glScalef(scale_z, scale_z, scale_z);
    
    const float center_x = 0;
    const float center_y = 0;
    
    const int parts_count = 250;
    
    float period = (M_PI/parts_count);
    
    srand(time(0));
    
    const float amplitude_max = 10;
    
#define COLOR_RANDOM (0.1*(rand()%10) + 0.01*(rand()%10) + 0.001*(rand()%10))
    
    
    for(float j = 0.0 ; j <= M_PI ; j += period )
    {
        float last_color_r = COLOR_RANDOM;
        float last_color_g = COLOR_RANDOM;
        float last_color_b = COLOR_RANDOM;
        
        const float vertex_z = (amplitude_max-amplitude_max*(M_PI-j))*abs(sin(j));
        cout<<"vertex_z="<<vertex_z<<endl;
        
        float amplitude_cur = amplitude_max*sin(j);
        glBegin(GL_TRIANGLE_FAN);
        glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(center_x, center_y, vertex_z);
        glColor3f(last_color_r, last_color_g, last_color_b); glVertex3f(amplitude_cur*cos(0), amplitude_cur*sin(0), vertex_z);
        for(float i = period ; i <= M_PI*2 ; i += period )
        {
            glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(amplitude_cur*cos(i), amplitude_cur*sin(i), vertex_z);
        }
        glColor3f(last_color_r, last_color_g, last_color_b); glVertex3f(amplitude_cur*cos(0), amplitude_cur*sin(0), vertex_z);
        glEnd();
    }
}

int main(int argc, char *argv[])
{
    dpy = XOpenDisplay(NULL);
    
    if(dpy == NULL)
    {
        printf("\n\tcannot connect to X server\n\n");
        exit(0);
    }
    
    root = DefaultRootWindow(dpy);
    
    vi = glXChooseVisual(dpy, 0, att);
    
    if(vi == NULL)
    {
        printf("\n\tno appropriate visual found\n\n");
        exit(0);
    }
    else
    {
        printf("\n\tvisual %p selected\n", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */
    }
    
    
    cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask;
    
    win = XCreateWindow(dpy, root, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "VERY SIMPLE APPLICATION");
    
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
    
    glEnable(GL_DEPTH_TEST); 
    
    while(1)
    {
        XNextEvent(dpy, &xev);
        
        if(xev.type == Expose)
        {
            XGetWindowAttributes(dpy, win, &gwa);
            glViewport(0, 0, gwa.width, gwa.height);
            window_width = gwa.width;
            window_height = gwa.height;
            DrawAQuad(); 
            glXSwapBuffers(dpy, win);
        }
    
        else if(xev.type == KeyPress)
        {
            cout<<xev.xbutton.button<<endl;
            switch(xev.xbutton.button)
            {
                case 113://LEFT
                        rotate_x += 1.1;
                    break;
                case 114://RIGHT
                        rotate_x -= 1.1;
                    break;
                case 111://UP
                        rotate_y += 1.1;
                    break;
                case 116://DOWN
                        rotate_y -= 1.1;
                    break;
                case 52://Z
                        rotate_z += 1.1;
                    break;
                case 53://X
                        rotate_z -= 1.1;
                    break;
                case 54://X
                    scale_z -= 0.1;
                break;
            }
            InvalidateWindow();
            
            cout<<"rotate_x="<<rotate_x<<endl;
            cout<<"rotate_y="<<rotate_y<<endl;
            cout<<"rotate_z="<<rotate_z<<endl;
                
//            glXMakeCurrent(dpy, None, NULL);
//            glXDestroyContext(dpy, glc);
//            XDestroyWindow(dpy, win);
//            XCloseDisplay(dpy);
//            exit(0);
        }
    } /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */