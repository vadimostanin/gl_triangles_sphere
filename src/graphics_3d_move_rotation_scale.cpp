// -- Written in C -- //

#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#define GL_GLEXT_PROTOTYPES 1
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include <math.h>
#include <time.h>
#include <iostream>
#include <string.h>
#include <GL/freeglut_std.h>
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

bool mouse_button_pressed = false;
float mouse_pointer_x = 0;
float mouse_pointer_y = 0;
int start_mouse_point_x = 0;
int start_mouse_point_y = 0;

float camera_look_from_z = 4.0;

typedef struct
{
    void *data;
    unsigned int width;
    unsigned int height;
}RGBImageRec;

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

void SetUpLights()
{
    float pos[] = {10, 10, 10, 0};
    float ambient[] = {0.3, 0.3, 0.3, 1};
    float white[] = {1, 1, 1, 1};
    
    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    
    glEnable(GL_LIGHT0);
}

static bool loadBMP_custom(const char * imagepath, RGBImageRec &result)
{
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char * data;
    
    // Open the file
    FILE * file = fopen(imagepath,"rb");
    if (!file)
    {
        printf("Image could not be opened\n");
        return false;
    }
    if ( fread(header, 1, 54, file)!=54 )
    { // If not 54 bytes read : problem
        printf("Not a correct BMP file\n");
        return false;
    }
    if ( header[0]!='B' || header[1]!='M' )
    {
        printf("Not a correct BMP file\n");
        return false;
    }
    // Read ints from the byte array
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);
    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)
    {
        imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    }
    if (dataPos==0)
    {
        dataPos=54; // The BMP header is done that way
    }
    // Create a buffer
    data = new unsigned char [imageSize];
     
    // Read the actual data from the file into the buffer
    fread(data,1,imageSize,file);
     
    //Everything is in memory now, the file can be closed
    fclose(file);
    
    result.data = data;
    result.width = width;
    result.height = height;
    
//    // Create one OpenGL texture
//    GLuint textureID;
//    glGenTextures(1, &textureID);
//     
//    // "Bind" the newly created texture : all future texture functions will modify this texture
//    glBindTexture(GL_TEXTURE_2D, textureID);
//     
//    // Give the image to OpenGL
//    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
//     
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return true;
}

RGBImageRec image;
void DrawAQuad()
{
//    cout<<"-------DRAW START-------"<<endl<<endl<<endl;
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-5., 5., -5., 5., -10, 10.);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(mouse_pointer_x, mouse_pointer_y, camera_look_from_z, -0.0, -0.0, 0., 0,1,0);
//    glTranslatef(-mouse_pointer_x, mouse_pointer_y, 0.0);
    
    glRotatef(rotate_x, 0, 1, 0);//x like y
    glRotatef(rotate_y, 1, 0, 0);//y like x
    glRotatef(rotate_z, 0, 0, 1);
    
    glScalef(scale_z, scale_z, scale_z);
    
    
    srand(time(0));
    
#define COLOR_RANDOM 1.0//(0.1*(rand()%10) + 0.01*(rand()%10) + 0.001*(rand()%10))

    glEnable(GL_TEXTURE_2D);        // Разрешение наложение текстуры
    GLuint  texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, image.width, image.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image.data);
    printf("image.data=%p\n", image.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBegin(GL_QUADS);
    // Передняя грань
//glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);  // Низ право
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, 1.0f);  // Верх право
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, 1.0f);  // Верх лево
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 1.0f);  // Низ лево

                    // Задняя грань
    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM);
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f(-1.0f, -1.0f, -1.0f);  // Низ право
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f(-1.0f,  1.0f, -1.0f);  // Верх право
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f( 1.0f,  1.0f, -1.0f);  // Верх лево
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f( 1.0f, -1.0f, -1.0f);  // Низ лево
    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM);
                    // Верхняя грань
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f(-1.0f,  1.0f, -1.0f);  // Верх лево
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f(-1.0f,  1.0f,  1.0f);  // Низ лево
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f( 1.0f,  1.0f,  1.0f);  // Низ право
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f( 1.0f,  1.0f, -1.0f);  // Верх право
    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM);
                    // Нижняя грань
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f(-1.0f, -1.0f, -1.0f);  // Верх право
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f( 1.0f, -1.0f, -1.0f);  // Верх лево
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f( 1.0f, -1.0f,  1.0f);  // Низ лево
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f(-1.0f, -1.0f,  1.0f);  // Низ право
    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM);
                    // Правая грань
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f( 1.0f, -1.0f, -1.0f);  // Низ право
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f( 1.0f,  1.0f, -1.0f);  // Верх право
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f( 1.0f,  1.0f,  1.0f);  // Верх лево
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f( 1.0f, -1.0f,  1.0f);  // Низ лево
    glColor3f(COLOR_RANDOM, COLOR_RANDOM, COLOR_RANDOM);
                    // Левая грань
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f(-1.0f, -1.0f, -1.0f);  // Низ лево
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f(-1.0f, -1.0f,  1.0f);  // Низ право
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f(-1.0f,  1.0f,  1.0f);  // Верх право
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f(-1.0f,  1.0f, -1.0f);  // Верх лево

    glEnd();
    
    SetUpLights();
}

//TRIANGLE_STRIP = {before last vertex, last vertex and new vertex}
//TRIANGLE_FAN = first vertex, last vertex and new vertex}

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
    
    glutInit(&argc, argv);

    if( loadBMP_custom("./res/allfons.ru-12581.bmp", image) == false )
            return false;
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
                case 54://C
                    scale_z -= 0.1;
                break;
                case 55://V
                    scale_z += 0.1;
                break;
                case 56://B
                    camera_look_from_z -= 0.1;
                break;
                case 57://N
                    camera_look_from_z += 0.1;
                break;
            }
            InvalidateWindow();
            
            cout<<"rotate_x="<<rotate_x<<endl;
            cout<<"rotate_y="<<rotate_y<<endl;
            cout<<"rotate_z="<<rotate_z<<endl;
            cout<<"camera_look_from_z="<<camera_look_from_z<<endl;
                
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
            mouse_pointer_x = (start_mouse_point_x - xev.xmotion.x)*0.1;
            mouse_pointer_y = (start_mouse_point_y - xev.xmotion.y)*0.1;
            
            cout<<"mouse_pointer_x="<<mouse_pointer_x<<", mouse_pointer_y="<<mouse_pointer_y<<endl;
            InvalidateWindow();
        }
    } /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */
