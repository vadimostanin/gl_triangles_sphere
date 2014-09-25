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
#include <string.h>
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

float rotate_x = 90.0;
float rotate_y = 0.0;
float rotate_z = 0.0;
float scale_z = 1.0;

bool mouse_button_pressed = false;
int mouse_pointer_x = 0;
int mouse_pointer_y = 0;
int start_mouse_point_x = 0;
int start_mouse_point_y = 0;

const int vertex_per_triangles_count = 3;

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

template<typename T>inline T ABS(T x)
{
	return (x<0)?(-1)*x:x;
}

static void __printf_vertex_array(float * array, unsigned int size, const char * name, int line_number)
{
    cout<<"-----"<<name<<"-"<<line_number<<"-----"<<endl;
    for( unsigned int i = 0 ; i < size ; i++ )
    {
        cout<<"["<<i<<"]:x="<<array[i*vertex_per_triangles_count]<<"; y="<<array[i*vertex_per_triangles_count + 1]<<"; z="<<array[i*vertex_per_triangles_count + 2]<<endl;
    }
    cout<<"---------------------"<<endl;
}

void DrawAQuad()
{
    cout<<"-------DRAW START-------"<<endl<<endl<<endl;
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10., 10., -10., 10., 1, 30.);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0., 0., 10., 0., 0., 0., 0,1,0);
    
    glRotatef(rotate_x, 0, 1, 0);//x like y
    glRotatef(rotate_y, 1, 0, 0);//y like x
    glRotatef(rotate_z, 0, 0, 1);
    
    glScalef(scale_z, scale_z, scale_z);
    
    const float center_x = 0;
    const float center_y = 0;
    
    const int parts_count = 100;
    
    
    const int vertex_array_size = parts_count*vertex_per_triangles_count;
    float last_triangle_layer_vertex_array[vertex_array_size] = {0.0};
    float temp_last_triangle_layer_vertex_array[vertex_array_size] = {0.0};
    memset(&last_triangle_layer_vertex_array, 0.0, vertex_array_size*sizeof(float));
    memset(&temp_last_triangle_layer_vertex_array, 0.0, vertex_array_size*sizeof(float));
    
    float layer_period = ((M_PI*2)/parts_count);
    float inter_layer_period = ((M_PI)/parts_count);
    
    srand(time(0));
    
    const float amplitude_max = 10;
    
#define COLOR_RANDOM (0.1*(rand()%10) + 0.01*(rand()%10) + 0.001*(rand()%10))
    
    
    float amplitude_cur = amplitude_max;
    
    float last_color_r = COLOR_RANDOM;
    float last_color_g = COLOR_RANDOM;
    float last_color_b = COLOR_RANDOM;
    
    float offset_z = 0;

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(center_x, center_y, offset_z);

    amplitude_cur = amplitude_max*ABS(sin(inter_layer_period));
    offset_z += amplitude_cur*sin(inter_layer_period);//plus to one over z coordinate
    glColor3f(last_color_r, last_color_g, last_color_b); glVertex3f(amplitude_cur*cos(0), amplitude_cur*sin(0), offset_z);
    cout<<"------FIRST FAN-------"<<endl;
    
    
    for(int i = 0 ; i < parts_count ; i++ )
    {
        cout<<"ABS(sin(i*period))="<<ABS(sin(i*layer_period))<<", amplitude_cur="<<amplitude_cur<<endl;
        last_triangle_layer_vertex_array[i*vertex_per_triangles_count] = amplitude_cur*cos(i*layer_period);
        last_triangle_layer_vertex_array[i*vertex_per_triangles_count+1] = amplitude_cur*sin(i*layer_period);
        last_triangle_layer_vertex_array[i*vertex_per_triangles_count+2] = offset_z;
//        cout<<"x="<<last_triangle_layer_vertex_array[i*vertex_per_triangles_count]<<
//              ", y="<<last_triangle_layer_vertex_array[i*vertex_per_triangles_count + 1]<<
//              ", z="<<last_triangle_layer_vertex_array[i*vertex_per_triangles_count + 2]<<endl;
        glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); 
        glVertex3f(last_triangle_layer_vertex_array[i*vertex_per_triangles_count], 
                   last_triangle_layer_vertex_array[i*vertex_per_triangles_count+1], 
                   last_triangle_layer_vertex_array[i*vertex_per_triangles_count + 2]);
    }

    glColor3f(last_color_r, last_color_g, last_color_b); glVertex3f(amplitude_cur*cos(0), amplitude_cur*sin(0), offset_z);
    glEnd();

    for(int j = 1 ; j < parts_count ; j++)
    {
        amplitude_cur = amplitude_max*ABS(sin((j+1)*inter_layer_period));
        offset_z += amplitude_cur*sin(inter_layer_period);
        memcpy(temp_last_triangle_layer_vertex_array, last_triangle_layer_vertex_array, vertex_array_size*sizeof(float));
        glBegin(GL_TRIANGLE_STRIP);
        
        for(int i = 0 ; i < parts_count ; i++ )
        {
            const int current_index = i;
            const int next_index = (i == (parts_count - 1) ? 0 : (i + 1));
//            
//            cout<<"arr_x="<<temp_last_triangle_layer_vertex_array[current_index*vertex_per_triangles_count]<<
//                  ", arr_y="<<temp_last_triangle_layer_vertex_array[current_index*vertex_per_triangles_count + 1]<<
//                  ", arr_z="<<temp_last_triangle_layer_vertex_array[current_index*vertex_per_triangles_count + 2]<<endl;
//            cout<<"new_x="<<amplitude_cur*cos(current_index*period)<<", new_y="<<amplitude_cur*sin(current_index*period)<<", new_z="<<offset_z<<endl;
//            cout<<"arr_x="<<temp_last_triangle_layer_vertex_array[next_index*vertex_per_triangles_count]<<
//                          ", arr_y="<<temp_last_triangle_layer_vertex_array[next_index*vertex_per_triangles_count + 1]<<
//                          ", arr_z="<<temp_last_triangle_layer_vertex_array[next_index*vertex_per_triangles_count + 2]<<endl;
//            cout<<"new_x="<<amplitude_cur*cos(next_index*period)<<", new_y="<<amplitude_cur*sin(next_index*period)<<", new_z="<<offset_z<<endl;
//            
            
            
            glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); 
            glVertex3f(temp_last_triangle_layer_vertex_array[current_index*vertex_per_triangles_count], 
                       temp_last_triangle_layer_vertex_array[current_index*vertex_per_triangles_count + 1], 
                       temp_last_triangle_layer_vertex_array[current_index*vertex_per_triangles_count + 2]);
            
            float next_layer_near_vertex_x = amplitude_cur*cos(current_index*layer_period);
            float next_layer_near_vertex_y = amplitude_cur*sin(current_index*layer_period);
            float next_layer_near_vertex_z = offset_z;
            
            glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); 
            glVertex3f(next_layer_near_vertex_x, next_layer_near_vertex_y, next_layer_near_vertex_z);
            
            glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); 
            glVertex3f(temp_last_triangle_layer_vertex_array[next_index*vertex_per_triangles_count], 
                       temp_last_triangle_layer_vertex_array[next_index*vertex_per_triangles_count + 1], 
                       temp_last_triangle_layer_vertex_array[next_index*vertex_per_triangles_count + 2]);
            
            float next_layer_far_vertex_x = amplitude_cur*cos(next_index*layer_period);
            float next_layer_far_vertex_y = amplitude_cur*sin(next_index*layer_period);
            float next_layer_far_vertex_z = offset_z;
                        
            glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); 
            glVertex3f(amplitude_cur*cos(next_index*layer_period), amplitude_cur*sin(next_index*layer_period), offset_z);
            
            
            last_triangle_layer_vertex_array[current_index*vertex_per_triangles_count] = next_layer_near_vertex_x;
            last_triangle_layer_vertex_array[current_index*vertex_per_triangles_count+1] = next_layer_near_vertex_y;
            last_triangle_layer_vertex_array[current_index*vertex_per_triangles_count+2] = next_layer_near_vertex_z;
            
            last_triangle_layer_vertex_array[next_index*vertex_per_triangles_count] = next_layer_far_vertex_x;
            last_triangle_layer_vertex_array[next_index*vertex_per_triangles_count+1] = next_layer_far_vertex_y;
            last_triangle_layer_vertex_array[next_index*vertex_per_triangles_count+2] = next_layer_far_vertex_z;
        }
        
        glEnd();
    }
    
    offset_z += amplitude_cur*sin(inter_layer_period);//plus to one over z coordinate
    
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(center_x, center_y, offset_z);

    cout<<"------LAST FAN-------"<<endl;
    for(int i = 0 ; i < parts_count ; i++ )
    {
//        cout<<"x="<<last_triangle_layer_vertex_array[i*vertex_per_triangles_count]<<
//              ", y="<<last_triangle_layer_vertex_array[i*vertex_per_triangles_count + 1]<<
//              ", z="<<last_triangle_layer_vertex_array[i*vertex_per_triangles_count + 2]<<endl;
        glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); 
        glVertex3f(last_triangle_layer_vertex_array[i*vertex_per_triangles_count], 
                   last_triangle_layer_vertex_array[i*vertex_per_triangles_count+1], 
                   last_triangle_layer_vertex_array[i*vertex_per_triangles_count + 2]);
    }

    glColor3f(last_color_r, last_color_g, last_color_b); glVertex3f(amplitude_cur*cos(0), amplitude_cur*sin(0), last_triangle_layer_vertex_array[2]);
    glEnd();
}

//float amplitude_cur = 5;//amplitude_max*sin(j);
//
//        glBegin(GL_TRIANGLE_STRIP);
//        float offset_z = 0;
//        glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(center_x, center_y, offset_z);
//
//        offset_z = 1;
//        glColor3f(last_color_r, last_color_g, last_color_b); glVertex3f(amplitude_cur*cos(0), amplitude_cur*sin(0), offset_z);
//        for(float i = 0.0 ; i <= M_PI*2 ; i += period )
//        {
//            glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(amplitude_cur*cos(i), amplitude_cur*sin(i), offset_z);
//        }
//        glColor3f(last_color_r, last_color_g, last_color_b); glVertex3f(amplitude_cur*cos(0), amplitude_cur*sin(0), offset_z);
//        glEnd();
//TRIANGLE_STRIP = {before last vertex, last vertex and new vertex}
//TRIANGLE_FAN = first vertex, last vertex and new vertex}
//void DrawAQuad()
//{
//    glClearColor(1.0, 1.0, 1.0, 1.0);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(-10., 10., -10., 10., 1, 30.);
//    
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    gluLookAt(mouse_pointer_x, mouse_pointer_y, 20., 0.0, 0.0, 0.0, 0,1,0);
//    
//    glRotatef(rotate_x, 0, 1, 0);//x like y
//    glRotatef(rotate_y, 1, 0, 0);//y like x
//    glRotatef(rotate_z, 0, 0, 1);
//    
//    glScalef(scale_z, scale_z, scale_z);
//#define COLOR_RANDOM (0.1*(rand()%10) + 0.01*(rand()%10) + 0.001*(rand()%10))
//    glBegin(GL_TRIANGLE_FAN);
//    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(0.0, 10.0, 10.0);
//    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(5.0, 10.0, 10.0);
//    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(0.0, 15.0, 0.0);
//    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(0.0, 17.0, 0.0);
//    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(10.0, 20.0, 0.0);
//    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(10.0, 25.0, 10.0);
//    glEnd();
//    
//    glBegin(GL_TRIANGLE_STRIP);
//    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(0.0, 0.0, 10.0);
//    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(5.0, 0.0, 10.0);
//    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(0.0, 5.0, 0.0);
//    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(0.0, 7.0, 0.0);
//    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM); glVertex3f(10.0, 15.0, 10.0);
//    glEnd();
//}

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
    swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    
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
//            cout<<xev.xbutton.button<<endl;
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
        else if(xev.type == ButtonPress)
        {
            start_mouse_point_x = xev.xbutton.x;
            start_mouse_point_y = xev.xbutton.y;
            mouse_button_pressed = true;
        }
        else if(xev.type == ButtonRelease)
        {
            mouse_button_pressed = false;
        }
        else if(xev.type == MotionNotify && mouse_button_pressed)
        {
            mouse_pointer_x = (start_mouse_point_x - xev.xmotion.x);
            mouse_pointer_y = (start_mouse_point_y - xev.xmotion.y);
            
            cout<<"mouse_pointer_x="<<mouse_pointer_x<<", mouse_pointer_y="<<mouse_pointer_y<<endl;
            InvalidateWindow();
        }
    } /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */
