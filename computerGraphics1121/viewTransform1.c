/*-----------------------------------------------------------
 *This program shows a helicopter, the user can switch between the 3 axis views, and the perspective view.
 *The camera can also be translated or rotated by the user.
 *Author: Wei Doong Wong
 *        Dept. of Computer Science and Engineering
 *        National Taiwan Ocean Uni.
 *Date  : 27th November 2023
 *
 */
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <GL/glut.h>

#define  PI   3.141592653

#define Step  0.5

 /*-----Define a unit box--------*/
 /* Vertices of the box */
float  points[][3] = { {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5},
                      {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5},
                      {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5},
                      {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5} };
/* face of box, each face composing of 4 vertices */
int    face[][4] = { {0, 3, 2, 1}, {0, 1, 5, 4}, {1, 2, 6, 5},
                    {4, 5, 6, 7}, {2, 3, 7, 6}, {0, 4, 7, 3} };
float  colors[6][3] = { {0.5,0.5,0.5}, {0.7,0.7,0.7}, {0.7,0.5,0.5},
                     {0.5,0.5,0.5}, {0.5,0.7,0.5}, {0.5,0.5,0.7} };
/* indices of the box faces */
int    cube[6] = { 0, 1, 2, 3, 4, 5 };

/*-Declare GLU quadric objects, sphere, cylinder, and disk --*/
GLUquadricObj* sphere = NULL, * cylind = NULL, * disk;


/*-Declare car position, orientation--*/
float  self_ang = -90.0, glob_ang = 0.0;
float  position[3] = { 8.0, 4.0, 0.0 };

float blade_angle = 0;
bool depart = false;
int direction = 0; /*0 = neutral, 1 = front pitch, 2 = back pitch, 3 = left roll, 4 = right roll.*/
int color = 5;     /*5 = pink themed heli, 6 = blue themed heli.*/
int rounds = 0;

/*-----Define window size----*/
int width = 1200, height = 1200;


/*-----Translation and rotations of eye coordinate system---*/
float   eyeDx = 0.0, eyeDy = 0.0, eyeDz = 0.0;
float   eyeAngx = 0.0, eyeAngy = 0.0, eyeAngz = 0.0;
double  Eye[3] = { 0.0, 0.0, 30.0 }, Focus[3] = { 0.0, 0.0, 0.0 },
Vup[3] = { 0.0, 1.0, 0.0 };

float   u[3][3] = { {1.0,0.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0} };
float   eye[3];
float   cv, sv; /* cos(5.0) and sin(5.0) */

/*-----Drawing stye
 0:4-windows, 1:x direction, 2:y direction, 3:z-dirtection, 4:perspective
 */
int style = 0;

void draw_scene(void);
float norm2(float v[]);

/*----------------------------------------------------------
 * Procedure to initialize the working environment.
 */
void  myinit()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);      /*set the background color BLACK */
                     /*Clear the Depth & Color Buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    /*---Create quadratic objects---*/
    if (sphere == NULL) {
        sphere = gluNewQuadric();
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
    }
    if (cylind == NULL) {
        cylind = gluNewQuadric();
        gluQuadricDrawStyle(cylind, GLU_FILL);
        gluQuadricNormals(cylind, GLU_SMOOTH);
    }
    if (disk == NULL) {
        disk = gluNewQuadric();
        gluQuadricDrawStyle(disk, GLU_FILL);
        gluQuadricNormals(disk, GLU_SMOOTH);
    }

    /*---- Compute cos(5.0) and sin(5.0) ----*/
    cv = cos(5.0 * PI / 180.0);
    sv = sin(5.0 * PI / 180.0);
    /*---- Copy eye position ---*/
    eye[0] = Eye[0];
    eye[1] = Eye[1];
    eye[2] = Eye[2];
}


/*Procedure to draw the blades of the helicopter.*/
void draw_blade()
{
    if(color == 5 || rounds == 0) glColor3f(1.0, 0.5, 0.5);
    if (color == 6 || rounds == 1) glColor3f(0.25, 0.41, 0.88);
    if (color == 7 || rounds == 2) glColor3f(0.37, 0.43, 0.32);

    glBegin(GL_POLYGON);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 4.0, 0.0);
    glVertex3f(1.0, 8.0, 0.0);
    glVertex3f(-1.0, 8.0, 0.0);
    glVertex3f(-1.0, 4.0, 0.0);
    glEnd();
}

/*--------------------------------------------------------
 * Procedure to draw a 1x1x1 cube. The cube is within
 * (-0.5,-0.5,-0.5) ~ (0.5,0.5,0.5)
 */
void draw_cube()
{
    int    i;

    for (i = 0; i < 6; i++) {
        //glColor3fv(colors[i]);  /* Set color */
        glBegin(GL_POLYGON);  /* Draw the face */
        glVertex3fv(points[face[i][0]]);
        glVertex3fv(points[face[i][1]]);
        glVertex3fv(points[face[i][2]]);
        glVertex3fv(points[face[i][3]]);
        glEnd();
    }
}


/*---------------------------------------------------------
 * Procedure to draw the floor.
 */
void draw_floor()
{
    int  i, j;

    for (i = 0; i < 10; i++)
        for (j = 0; j < 10; j++) {
            if ((i + j) % 2 == 0) glColor3f(0.8, 0.8, 0.8);
            else glColor3f(0.2, 0.2, 0.4);
            glBegin(GL_POLYGON);
            glVertex3f((i - 5.0) * 10.0, -2.2, (j - 5.0) * 10.0);
            glVertex3f((i - 5.0) * 10.0, -2.2, (j - 4.0) * 10.0);
            glVertex3f((i - 4.0) * 10.0, -2.2, (j - 4.0) * 10.0);
            glVertex3f((i - 4.0) * 10.0, -2.2, (j - 5.0) * 10.0);
            glEnd();
        }
}

/*-------------------------------------------------------
 * Procedure to draw three axes and the orign
 */
void draw_axes()
{

    /*----Draw a white sphere to represent the original-----*/
    glColor3f(0.9, 0.9, 0.9);

    gluSphere(sphere, 2.0,   /* radius=2.0 */
        12,            /* composing of 12 slices*/
        12);           /* composing of 8 stacks */

  /*----Draw three axes in colors, yellow, meginta, and cyan--*/
  /* Draw Z axis  */
    glColor3f(0.0, 0.95, 0.95);
    gluCylinder(cylind, 0.5, 0.5, /* radius of top and bottom circle */
        10.0,              /* height of the cylinder */
        12,               /* use 12-side polygon approximating circle*/
        3);               /* Divide it into 3 sections */

/* Draw Y axis */
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);  /*Rotate about x by -90', z becomes y */
    glColor3f(0.95, 0.0, 0.95);
    gluCylinder(cylind, 0.5, 0.5, /* radius of top and bottom circle */
        10.0,             /* height of the cylinder */
        12,               /* use 12-side polygon approximating circle*/
        3);               /* Divide it into 3 sections */
    glPopMatrix();

    /* Draw X axis */
    glColor3f(0.95, 0.95, 0.0);
    glPushMatrix();
    glRotatef(90.0, 0.0, 1.0, 0.0);  /*Rotate about y  by 90', x becomes z */
    gluCylinder(cylind, 0.5, 0.5,   /* radius of top and bottom circle */
        10.0,             /* height of the cylinder */
        12,               /* use 12-side polygon approximating circle*/
        3);               /* Divide it into 3 sections */
    glPopMatrix();
    /*-- Restore the original modelview matrix --*/
    glPopMatrix();
}

/*-------------------------------------------------------
 * Make viewing matrix
 */
void make_view(int x)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (x) {
    case 4:       /* Perspective */

      /* In this sample program, eye position and Xe, Ye, Ze are computed
         by ourselves. Therefore, use them directly; no trabsform is
         applied upon eye coordinate system
         */
        gluLookAt(eye[0], eye[1], eye[2],
            eye[0] - u[2][0], eye[1] - u[2][1], eye[2] - u[2][2],
            u[1][0], u[1][1], u[1][2]);
        break;

    case 1:       /* X direction parallel viewing */
        gluLookAt(30.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        break;
    case 2:       /* Y direction parallel viewing */
        gluLookAt(0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
        break;
    case 3:
        gluLookAt(0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        break;
    }
}

/*------------------------------------------------------
 * Procedure to make projection matrix
 */
void make_projection(int x)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (x == 4) {
        gluPerspective(90.0, (double)width / (double)height, 1.5, 50.0);
    }
    else {
        if (width > height)
            glOrtho(-40.0, 40.0, -40.0 * (float)height / (float)width,
                40.0 * (float)height / (float)width,
                -0.0, 100.0);
        else
            glOrtho(-40.0 * (float)width / (float)height,
                40.0 * (float)width / (float)height, -40.0, 40.0,
                -0.0, 100.0);
    }
    glMatrixMode(GL_MODELVIEW);
}


/*---------------------------------------------------------
 * Procedure to draw view volume, eye position, focus ,...
 * for perspective projection
 */
void draw_view()
{
    int    i;

    glMatrixMode(GL_MODELVIEW);

    /*----Draw Eye position-----*/
    glPushMatrix();
    glTranslatef(eye[0], eye[1], eye[2]);
    glColor3f(0.0, 1.0, 0.0);
    glutWireSphere(1.0, 10, 10);
    glPopMatrix();

    /*----Draw eye coord. axes -----*/
    glColor3f(1.0, 1.0, 0.0);           /* Draw Xe */
    glBegin(GL_LINES);
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] + 20.0 * u[0][0], eye[1] + 20.0 * u[0][1], eye[2] + 20.0 * u[0][2]);
    glEnd();

    glColor3f(1.0, 0.0, 1.0);          /* Draw Ye */
    glBegin(GL_LINES);
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] + 20.0 * u[1][0], eye[1] + 20.0 * u[1][1], eye[2] + 20.0 * u[1][2]);
    glEnd();

    glColor3f(0.0, 1.0, 1.0);          /* Draw Ze */
    glBegin(GL_LINES);
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] + 20.0 * u[2][0], eye[1] + 20.0 * u[2][1], eye[2] + 20.0 * u[2][2]);
    glEnd();
}


/*--------------------------------------------------------
 * Display callback procedure to draw the scene
 */
void display()
{
    /*Clear previous frame and the depth buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    switch (style) {
    case 0:
        make_view(4);
        make_projection(4);
        glViewport(width / 2, 0, width / 2, height / 2);
        draw_scene();

        make_view(1);
        make_projection(1);
        glViewport(0, height / 2, width / 2, height / 2);
        draw_scene();
        make_view(1);
        draw_view();

        make_view(2);
        glViewport(width / 2, height / 2, width / 2, height / 2);
        draw_scene();
        make_view(2);
        draw_view();

        make_view(3);
        glViewport(0, 0, width / 2, height / 2);
        draw_scene();
        make_view(3);
        draw_view();
        break;

    case 1:
        glViewport(0, 0, width, height);
        make_view(1);
        make_projection(1);
        draw_scene();
        break;
    case 2:
        glViewport(0, 0, width, height);
        make_view(2);
        make_projection(2);
        draw_scene();
        break;
    case 3:
        glViewport(0, 0, width, height);
        make_view(3);
        make_projection(3);
        draw_scene();
        break;
    case 4:
        glViewport(0, 0, width, height);
        make_view(4);
        make_projection(4);
        draw_scene();
        break;
    }
    /*-------Swap the back buffer to the front --------*/
    glutSwapBuffers();
    return;

}

/*-------------------------------------------------------
 * This procedure draw the car
 */
void draw_heli()
{
    if (color == 5 || rounds == 0) glColor3f(1.0, 0.8, 0.8);
    if (color == 6 || rounds == 1) glColor3f(0.65, 0.8, 0.91);
    if (color == 7 || rounds == 2) glColor3f(0.66, 0.70, 0.53);

    /*-------Draw the car body which is a cube----*/
    glTranslatef(position[0], position[1], position[2]);
    glRotatef(self_ang, 0.0, 1.0, 0.0);

    switch (direction)
    {
    case 1:
        glRotatef(45, 0.0, 0.0, 1.0);
        break;

    case 2:
        glRotatef(-45, 0.0, 0.0, 1.0);
        break;

    case 3:
        glRotatef(45, 1.0, 0.0, 0.0);
        break;
    case 4:
        glRotatef(-45, 1.0, 0.0, 0.0);
        break;

    default:break;
    }

    glPushMatrix();              /* Save M1 coord. sys */
    glScalef(10.0, 10.0, 10.0);    /* Scale up the axes */
    draw_cube();
    glPopMatrix();               /* Get M1 back */

    /*-------Draw the skids -----*/

    if (color == 5 || rounds == 0) glColor3f(1.0, 0.5, 0.5);
    if (color == 6 || rounds == 1) glColor3f(0.25, 0.41, 0.88);
    if (color == 7 || rounds == 2) glColor3f(0.37, 0.43, 0.32);
    //glColor3f(1.0, 0.80, 0.8);
    glPushMatrix();              /* Save M1 coord. sys */
    glTranslatef(-5.0, -5.5, -3.0); /* Go to left skid position */
    glRotatef(90.0, 0.0, 1.0, 0.0);  /*Rotate about x by -90', z becomes y */
    //glColor3f(0.95, 0.0, 0.95);
    gluCylinder(cylind, 0.5, 0.5, /* radius of top and bottom circle */
        10.0,             /* height of the cylinder */
        12,               /* use 12-side polygon approximating circle*/
        3);               /* Divide it into 3 sections */
    glPopMatrix();

    glPushMatrix();              /* Save M1 coord. sys */
    glTranslatef(-5.0, -5.5, 3.0); /* Go to right wheel position */
    glRotatef(90.0, 0.0, 1.0, 0.0);  /*Rotate about x by -90', z becomes y */
    //glColor3f(0.95, 0.0, 0.95);
    gluCylinder(cylind, 0.5, 0.5, 10.0,12,3);              
    glPopMatrix();

    glPushMatrix();              /* Save M1 coord. sys */
    glTranslatef(2.5, 0.75, 0.5);
    glRotatef(90.0, 0.0, 1.0, 0.0);  /*Rotate about x by -90', z becomes y */
    //glColor3f(0.95, 0.0, 0.95);
    gluCylinder(cylind, 0.5, 0.5, 10.0, 12, 3);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(12, 0.75, 0.5);
    gluSphere(sphere, 1.0,12,12);           
    glPopMatrix();

    glPushMatrix();
    glTranslatef(12, 0.75, 0.5);
    glScalef(0.5, 0.5, 0.5);
    glRotatef(blade_angle, 0.0, 0.0, 1.0);
    draw_blade();
    glRotatef(90, 0.0, 0.0, 1.0);
    draw_blade();
    glRotatef(90, 0.0, 0.0, 1.0);
    draw_blade();
    glRotatef(90, 0.0, 0.0, 1.0);
    draw_blade();
    glPopMatrix();

    if (color == 5 || rounds == 0) glColor3f(1.0, 0.8, 0.8);
    if (color == 6 || rounds == 1) glColor3f(0.65, 0.8, 0.91);
    if (color == 7 || rounds == 2) glColor3f(0.66, 0.70, 0.53);

    glPushMatrix();
    glTranslatef(0, 10, 0);
    glRotatef(90, 1.0, 0.0, 0.0);
    gluCylinder(cylind, 0.5, 0.5, 10.0, 12, 3);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 10, 0);
    gluSphere(sphere, 1.0, 12, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 10, 0);
    glRotatef(blade_angle, 0.0, 1.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    draw_blade();
    glRotatef(90, 0.0, 0.0, 1.0);
    draw_blade();
    glRotatef(90, 0.0, 0.0, 1.0);
    draw_blade();
    glRotatef(90, 0.0, 0.0, 1.0);
    draw_blade();
    glPopMatrix();


}

void draw_scene()
{

    draw_floor();

    draw_axes();

    draw_heli();

    //fancy idea options: 1 change helicopter color 2 alternate between car and helicopter control. 3 alternate display of car or helicopter.

}

/*--------------------------------------------------
 * Reshape callback function which defines the size
 * of the main window when a reshape event occurrs.
 */
void my_reshape(int w, int h)
{

    width = w;
    height = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (double)w / (double)h, 1.5, 50.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*Callback function for mouse event, in charges of raising/decreasing the altitude of the helicopter.*/
void mouse_event_mine(int button, int state)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        position[1] += 1;
        if (position[1] > 4.0) depart = true;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        if (position[1] >= 4.0)position[1] -= 1;
        if (position[1] <= 4.0) { depart = false; position[1] = 4.0; }
    }
}

/*--------------------------------------------------
 * Keyboard callback func. When a 'q' key is pressed,
 * the program is aborted.
 */
void my_quit(unsigned char key, int ix, int iy)
{
    int    i;
    float  x[3], y[3], z[3];

    /*--- Controls the helicopter. ----*/
    if ((int)key == 27) exit(0);
    if (!depart);
    else if (key == 's') {                              /* move car back*/
        if (direction != 2) { direction = 2; debugg(2); return; };
        position[0] += Step * cos(self_ang * PI / 180.0);
        position[2] -= Step * sin(self_ang * PI / 180.0);
    }
    else if (key == 'w') {                        /* move car forward*/
        if (direction != 1) { direction = 1; debugg(1); return; };
        position[0] -= Step * cos(self_ang * PI / 180.0);
        position[2] += Step * sin(self_ang * PI / 180.0);
    }
    else if (key == 'a') {                        /* make a left turn */
        self_ang += 10.0;
        if (self_ang > 360.0) self_ang -= 360.0;
    }
    else if (key == 'd') {                        /* make a right turn */
        self_ang += -10.0;
        if (self_ang < 0.0) self_ang += 360.0;    
    } 
    else if (key == 'q')                          /*Strafe left.*/
    {
        if (direction != 3) { direction = 3; debugg(3); return; };
        position[2] -= Step * cos(self_ang * PI / 180.0);
        position[0] += Step * sin(self_ang * PI / 180.0);
    }
    else if (key == 'e')                          /*Strafe right.*/
    {
        if (direction != 4) { direction = 4; debugg(4); return; };
        position[2] += Step * cos(self_ang * PI / 180.0);
        position[0] -= Step * sin(self_ang * PI / 180.0);
    }

    /*mouse event determines the rise or landing of the helicopter.*/

    /*------transform the EYE coordinate system ------*/
    if (key == 'i') {
        eyeDy += 0.5;       /* move up */
        for (i = 0; i < 3; i++) eye[i] -= 0.5 * u[1][i];
    }
    else if (key == 'k') {
        eyeDy += -0.5;       /* move down */
        for (i = 0; i < 3; i++) eye[i] += 0.5 * u[1][i];
    }
    else if (key == 'j') {
        eyeDx += -0.5;       /* move left */
        for (i = 0; i < 3; i++) eye[i] += 0.5 * u[0][i];
    }
    else if (key == 'l') {
        eyeDx += 0.5;        /* move right */
        for (i = 0; i < 3; i++) eye[i] -= 0.5 * u[0][i];
    }
    else if (key == 'u') {
        eyeDz += 0.5;        /* zoom in */
        for (i = 0; i < 3; i++) eye[i] -= 0.5 * u[2][i];
    }
    else if (key == 'o') {
        eyeDz += -0.5;       /* zoom out */
        for (i = 0; i < 3; i++) eye[i] += 0.5 * u[2][i];
    }
    else if (key == 'm') {             /* pitching */
        eyeAngx += 5.0;
        if (eyeAngx > 360.0) eyeAngx -= 360.0;
        y[0] = u[1][0] * cv - u[2][0] * sv;
        y[1] = u[1][1] * cv - u[2][1] * sv;
        y[2] = u[1][2] * cv - u[2][2] * sv;

        z[0] = u[2][0] * cv + u[1][0] * sv;
        z[1] = u[2][1] * cv + u[1][1] * sv;
        z[2] = u[2][2] * cv + u[1][2] * sv;

        for (i = 0; i < 3; i++) {
            u[1][i] = y[i];
            u[2][i] = z[i];
        }
    }
    else if (key == 'M') {
        eyeAngx += -5.0;
        if (eyeAngx < 0.0) eyeAngx += 360.0;
        y[0] = u[1][0] * cv + u[2][0] * sv;
        y[1] = u[1][1] * cv + u[2][1] * sv;
        y[2] = u[1][2] * cv + u[2][2] * sv;

        z[0] = u[2][0] * cv - u[1][0] * sv;
        z[1] = u[2][1] * cv - u[1][1] * sv;
        z[2] = u[2][2] * cv - u[1][2] * sv;

        for (i = 0; i < 3; i++) {
            u[1][i] = y[i];
            u[2][i] = z[i];
        }
    }
    else if (key == ',') {            /* heading */
        eyeAngy += 5.0;
        if (eyeAngy > 360.0) eyeAngy -= 360.0;
        for (i = 0; i < 3; i++) {
            x[i] = cv * u[0][i] - sv * u[2][i];
            z[i] = sv * u[0][i] + cv * u[2][i];
        }
        for (i = 0; i < 3; i++) {
            u[0][i] = x[i];
            u[2][i] = z[i];
        }
    }
    else if (key == '<') {
        eyeAngy += -5.0;
        if (eyeAngy < 0.0) eyeAngy += 360.0;
        for (i = 0; i < 3; i++) {
            x[i] = cv * u[0][i] + sv * u[2][i];
            z[i] = -sv * u[0][i] + cv * u[2][i];
        }
        for (i = 0; i < 3; i++) {
            u[0][i] = x[i];
            u[2][i] = z[i];
        }
    }
    else if (key == '.') {            /* rolling */
        eyeAngz += 5.0;
        if (eyeAngz > 360.0) eyeAngz -= 360.0;
        for (i = 0; i < 3; i++) {
            x[i] = cv * u[0][i] - sv * u[1][i];
            y[i] = sv * u[0][i] + cv * u[1][i];
        }
        for (i = 0; i < 3; i++) {
            u[0][i] = x[i];
            u[1][i] = y[i];
        }
    }
    else if (key == '>') {
        eyeAngz += -5.0;
        if (eyeAngz < 0.0) eyeAngz += 360.0;
        for (i = 0; i < 3; i++) {
            x[i] = cv * u[0][i] + sv * u[1][i];
            y[i] = -sv * u[0][i] + cv * u[1][i];
        }
        for (i = 0; i < 3; i++) {
            u[0][i] = x[i];
            u[1][i] = y[i];
        }
    }
    else if (key == '0') {
        debugg(5);
        style = 0;
    }
    else if (key == '1') {
        debugg(7);
        style = 1;
    }
    else if (key == '2') {
        debugg(8);
        style = 2;
    }
    else if (key == '3') {
        debugg(9);
        style = 3;
    }
    else if (key == '4') {
        debugg(6);
        style = 4;
    }
    else if (key == '5')
    {
        debugg(10);
        color = 5;
    }
    else if (key == '6')
    {
        debugg(11);
        color = 6;
    }
    else if (key == '7')
    {
    debugg(12);
    color = 7;
    }
    else if (key == '8')
    {
    debugg(13);
    color = 8;
    }

    display();
}

void idle()
{
    if (!depart) {direction = 0; return;}
    if (color == 8) rounds++;
    if (rounds == 3) rounds = 0;

    blade_angle += 10;
    if (blade_angle >= 360) blade_angle -= 360;
    if (blade_angle <= 360) blade_angle += 360;
    display();
}

/*---------------------------------------------------
 * Main procedure which defines the graphics environment,
 * the operation style, and the callback func's.
 */
void main(int argc, char** argv)
{
    fprintf(stderr, "Controls:\n<esc> = quit\nw = forward\ns = backwards\na/d = rotate clockwise/CC wise.\n");
    fprintf(stderr, "----------Eye Coordinate System----------\ni/k = move up/down\nj/l = move left/right\nu/o = move near/far\n");
    fprintf(stderr, "M/m = pitching\n</, = heading\n>/. = rotate\n0 = all viewport view\n4 = perspective view\n");
    fprintf(stderr, "Debug Messages:\n");
    /*-----Initialize the GLUT environment-------*/
    glutInit(&argc, argv);

    /*-----Depth buffer is used, be careful !!!----*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(width, height);
    glutCreateWindow("helicopter helicopter");

    myinit();      /*---Initialize other state varibales----*/

    /*----Associate callback func's whith events------*/
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(my_reshape);
    glutKeyboardFunc(my_quit);
    glutMouseFunc(mouse_event_mine);

    glutMainLoop();
}

int debugg(int x)
{
    switch (x)
    {
    case 1: 
        fprintf(stderr, "helicopter pitched forward, press w again to move forward.\n");
        break;
    case 2: 
        fprintf(stderr, "helicopter pitched backward, press s again to move backward.\n");
        break;
    case 3: 
        fprintf(stderr, "helicopter rotated left, press q again to strafe left.\n");
        break;
    case 4: 
        fprintf(stderr, "helicopter rotated backward, press e again to strafe right.\n");
        break;
    case 5: 
        fprintf(stderr, "All viewports mode.\n");
        break;
    case 6: 
        fprintf(stderr, "Perspective ONLY mode.\n");
        break;
    case 7:
        fprintf(stderr, "From x-axis ONLY mode.\n");
        break;
    case 8:
        fprintf(stderr, "From y-axis ONLY mode.\n");
        break;
    case 9:
        fprintf(stderr, "From z-axis ONLY mode.\n");
        break;
    case 10:
        fprintf(stderr, "pink color helicopter.\n");
        break;
    case 11:
        fprintf(stderr, "blue color helicopter.\n");
        break;
    case 12:
        fprintf(stderr, "green color helicopter.\n");
        break;
    case 13:
        fprintf(stderr, "color show helicopter.\n");
        break;

    default:
        fprintf(stderr, "No message available. Add some more.\n");
        break;
    }
}

/*------------------------------------------
 * Procedure to compute norm of vector v[]
 */
float norm2(float v[])
{
    double  sum;
    float   temp;

    sum = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    temp = (float)sqrt(sum);
    return temp;
}
