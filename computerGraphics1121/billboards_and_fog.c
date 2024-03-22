/*-----------------------------------------------------------
 *This program allows the user to control lighting and shading of the scene.
 *The camera can also be translated or rotated by the user.
 *Author: Wei Doong Wong
 *        Dept. of Computer Science and Engineering
 *        National Taiwan Ocean Uni.
 *Date  : 14th January 2024
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <GL/glut.h>

#define  PI   3.141592653
#define Step  0.5
#define TSIZE 64

/*Create image space for textures.*/
unsigned char checkboard[TSIZE][TSIZE][4];
unsigned char brick[TSIZE][TSIZE][4];
unsigned char circle[TSIZE][TSIZE][4];
unsigned char land[TSIZE][TSIZE][4];
unsigned int textName[10];

float ds = 0.0, dt = 0.0;
float textAng = 0.0;
float normal[][4] = { { 0.0, 0.0, -1.0 }, { 0.0, -1.0, 0.0 }, { 1.0, 0.0, 0.0 },
                      { 0.0, 0.0, 1.0 },  { 0.0, 1.0, 0.0 },  { -1.0, 0.0, 0.0 } };

float mtx[16];

/*the axes of a billboard.*/
float a[3], b[3];

 /*Define a unit box*/
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


/*-Declare helicopter and car position, orientation--*/
float  self_ang = -90.0, glob_ang = 0.0, car_ang = 0.0, car_dist = 0.0;
float  position[3] = { 8.0, 4.0, 0.0 };
float carpos[3] = { -14,0,20 };

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

/*Drawing stye 0:4-windows, 1:x direction, 2:y direction, 3:z-dirtection, 4:perspective*/
int style = 0;

void draw_scene(void);
float norm2(float v[]);

/*For pop up menu.*/
int top_m, lightColor_m, lightOn_m, heliLight_m, truckLight_m, specials_m, sp1 = 0, sp2 = 0, i = 0, j = 0;

/*----Define material properties for cube -----*/
//float mat_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
//float mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
//float mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
float mat_shininess = 64.0;

float flr_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
float flr_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
float flr_specular[] = { 0.0, 0.0, 0.0, 1.0 };

/*----Define light source properties -------*/
float lit_position[] = { 0.0, 30.0, 0.0, 1.0 };
float lit_direction[] = { 0.0, -1.0, 0.0, 0.0 };
float lit_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
float lit_specular[] = { 1.0, 1.0, 1.0, 1.0 };
float lit_cutoff = 120.0;
float lit_exponent = 8.0;

float lit1_position[] = { 0.0,0,0.0,1.0 };
float lit1_specular[] = { 0.0,1.0,1.0,1.0 };
float lit1_direction[] = { 0.0, -10.0, 0.0, 1.0 };
float lit1_diffuse[] = { 0.0, 1.0, 1.0, 1.0 };
float lit1_cutoff = 180.0;
float lit1_exponent = 20.0;

float lit4_position[] = { 0.0,0,0.0,1.0 };
float lit4_specular[] = { 0.0,0,1.0,1.0 };
float lit4_direction[] = { 0.0, -1.0, 0.0, 1.0 };
float lit4_diffuse[] = { 0.0, 0, 1.0, 1.0 };
float lit4_cutoff = 100.0;
float lit4_exponent = 10.0;

float carlit_position[] = { 0,0,0, 1.0};
float carlit_direction[] = {5,0,0, 1.0 };
float carlit_diffuse[] = { 0.9,0.9,0.0,1.0 };
float carlit_specular[] = { 0.9,0.9,0.9,1.0 };
float carlit_cutoff = 180.0;
float carlit_exponent = 20.0;

float global_ambient[] = { 1.0, 1.0, 1.0, 1.0 };

/*Procedure to change the lights.*/
void blinding_lights(int value)
{
    if (value == 1)
    {
        lit_diffuse[0] = lit_specular[0] = 1.0;
        lit_diffuse[1] = lit_diffuse[2] =lit_specular[1] = lit_specular[2] = 0.0;
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lit_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lit_specular);
    }
    else if (value == 2)
    {
        lit_diffuse[1] = lit_specular[1] = 1.0;
        lit_diffuse[0] = lit_diffuse[2] = lit_specular[0] = lit_specular[2] = 0.0;
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lit_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lit_specular);
    }
    else if (value == 3)
    {
        lit_diffuse[2] = lit_specular[2] = 1.0;
        lit_diffuse[1] = lit_diffuse[0] = lit_specular[1] = lit_specular[0] = 0.0;
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lit_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lit_specular);
    }
    else if (value == 4)
    {
        lit_diffuse[0] = lit_diffuse[1] = lit_diffuse[2] = 1.0;
        lit_specular[0] = lit_specular[1] = lit_specular[2] = 1.0;
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lit_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lit_specular);
    }
    else if (value == 11)
    {
        glEnable(GL_LIGHT0);
    }
    else if (value == 12)
    {
        glDisable(GL_LIGHT0);
    }
    else if (value == 13)
    {
        lit_diffuse[0] -= 0.5; lit_specular[0] -= 0.5;
        lit_diffuse[1] -= 0.5; lit_diffuse[2] -= 0.5; lit_specular[1] -= 0.5; lit_specular[2] -= 0.5;
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lit_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lit_specular);
    }
    else if (value == 14)
    {
        lit_diffuse[0] += 0.5; lit_specular[0] += 0.5;
        lit_diffuse[1] += 0.5; lit_diffuse[2] += 0.5; lit_specular[1] += 0.5; lit_specular[2] += 0.5;
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lit_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lit_specular);
    }
    else if (value == 21)
    {
        glEnable(GL_LIGHT2);
    }
    else if (value == 22)
    {
        glDisable(GL_LIGHT2);
    }
    else if (value == 23)
    {
        lit1_diffuse[0] -= 0.5; lit1_specular[0] -= 0.5;
        lit1_diffuse[1] -= 0.5; lit1_diffuse[2] -= 0.5; lit1_specular[1] -= 0.5; lit1_specular[2] -= 0.5;
        glLightfv(GL_LIGHT2, GL_DIFFUSE, lit1_diffuse);
        glLightfv(GL_LIGHT2, GL_SPECULAR, lit1_specular);
    }
    else if (value == 24)
    {
        lit1_diffuse[0] += 0.5; lit1_specular[0] += 0.5;
        lit1_diffuse[1] += 0.5; lit1_diffuse[2] += 0.5; lit1_specular[1] += 0.5; lit1_specular[2] += 0.5;
        glLightfv(GL_LIGHT2, GL_DIFFUSE, lit1_diffuse);
        glLightfv(GL_LIGHT2, GL_SPECULAR, lit1_specular);
    }
    else if (value == 25)
    {
        lit1_cutoff = 30;
        glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, lit1_cutoff);
    }
    else if (value == 26)
    {
        lit1_cutoff = 300;
        glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, lit1_cutoff);
        glEnable(GL_LIGHT2);
    }
    else if (value == 31)
    {
        glEnable(GL_LIGHT1);
    }
    else if (value == 32)
    {
        glDisable(GL_LIGHT1);
    }
    else if (value == 33)
    {
        carlit_diffuse[0] -= 0.5; carlit_specular[0] -= 0.5;
        carlit_diffuse[1] -= 0.5; carlit_diffuse[2] -= 0.5; carlit_specular[1] -= 0.5; carlit_specular[2] -= 0.5;
        glLightfv(GL_LIGHT1, GL_DIFFUSE, carlit_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, carlit_specular);
    }
    else if (value == 34)
    {
        carlit_diffuse[0] += 0.5; carlit_specular[0] += 0.5;
        carlit_diffuse[1] += 0.5; carlit_diffuse[2] += 0.5; carlit_specular[1] += 0.5; carlit_specular[2] += 0.5;
        glLightfv(GL_LIGHT0, GL_DIFFUSE, carlit_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, carlit_specular);
    }
    else if (value == 98)
    {
    sp2 = !sp2; fprintf(stderr, "%d ", sp2);
    }
    else if (value == 99)
    {
        sp1 = !sp1; fprintf(stderr, "%d ", sp1);
    }
}

/*Procedure to draw the blades of the helicopter.*/
void draw_blade()
{
    float mat_ambientb[] = { 0.5, 0.35, 0.35, 1.0 };
    float mat_diffuseb[] = { 1.0, 0.7, 0.7, 1.0 };
    float mat_specularb[] = { 0.4, 0.4, 0.4, 1.0 };
    mat_shininess = 64.0;
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientb);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specularb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseb);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_POLYGON);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 4.0, 0.0);
    glVertex3f(1.0, 8.0, 0.0);
    glVertex3f(-1.0, 8.0, 0.0);
    glVertex3f(-1.0, 4.0, 0.0);
    glEnd();
}

/*Procedure to draw a 1x1x1 cube. The cube is within (-0.5,-0.5,-0.5) ~ (0.5,0.5,0.5)*/
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

/*Procedure to draw a textured cube.*/
void texture_cube()
{
    int i;
    float range;

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glRotatef(textAng, 0, 0, 1);
    // glTranslatef(ds, dt, 0.0);
    glScalef(1.0, 3.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    float mat_ambientb[] = { 0.5, 0.5, 0.5, 1.0 };
    float mat_diffuseb[] = { 1.0, 1.0, 1.0, 1.0 };
    float mat_specularb[] = { 0.4, 0.4, 0.4, 1.0 };
    mat_shininess = 64.0;
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientb);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specularb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseb);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    for (i = 0; i < 6; i++) { /* draw the six faces one by one */
        range = 1.0;
        glNormal3fv(normal[i]);
        glBegin(GL_POLYGON); /* Draw the face */
        glTexCoord2f(0.0, 0.0);
        glVertex3fv(points[face[i][0]]);
        glTexCoord2f(0.0, range);
        glVertex3fv(points[face[i][1]]);
        glTexCoord2f(range, range);
        glVertex3fv(points[face[i][2]]);
        glTexCoord2f(range, 0.0);
        glVertex3fv(points[face[i][3]]);
        glEnd();
    }
}

void draw_triangle()
{

        glBegin(GL_POLYGON);
        glVertex3f(0.0,0.0,0.0);
        glVertex3f(1.0,0.0,0.0);
        glVertex3f(0.5,1.0,0.0);
        glEnd();

}

/*Procedure to draw a 1x1x1 cube,that can take three float variables.*/
void draw_colored_cube(float r, float g, float b)
{
    int    i;

    for (i = 0; i < 6; i++) {
        glColor3f(r,g,b);  /* Set color */
        float mat_ambientb[] = { r/2, g/2, b/2, 1.0 };
        float mat_diffuseb[] = { r, g, b, 1.0 };
        float mat_specularb[] = { 0.4, 0.4, 0.4, 1.0 };
        mat_shininess = 64.0;
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientb);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specularb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseb);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

        glBegin(GL_POLYGON);  /* Draw the face */
        glVertex3fv(points[face[i][0]]);
        glVertex3fv(points[face[i][1]]);
        glVertex3fv(points[face[i][2]]);
        glVertex3fv(points[face[i][3]]);
        glEnd();
    }
}

/*Procedure to make a land texture.*/
void Create_Texture_Land() {
    int x, y, i;

    for (y = 0; y < 32; y++)
        for (x = 0; x < 32; x++) {
            land[x][y][0] = 240;
            land[x][y][1] = 240;
            land[x][y][2] = 240;
            land[x][y][3] = 255;
        }

    for (x = 24; x < 32; x++) {
        land[x][0][0] = 255;
        land[x][0][1] = 200;
        land[x][0][2] = 200;
        land[x][0][3] = 255;

        land[0][x][0] = 255;
        land[0][x][1] = 200;
        land[0][x][2] = 200;
        land[0][x][3] = 255;
    }
    for (x = 26; x < 32; x++) {
        land[x][1][0] = 255;
        land[x][1][1] = 200;
        land[x][1][2] = 200;
        land[x][1][3] = 255;

        land[1][x][0] = 255;
        land[1][x][1] = 200;
        land[1][x][2] = 200;
        land[1][x][3] = 255;
    }
    for (x = 27; x < 32; x++) {
        for (y = 2; y < 4; y++) {
            land[x][y][0] = 255;
            land[x][y][1] = 200;
            land[x][y][2] = 200;
            land[x][y][3] = 255;

            land[y][x][0] = 255;
            land[y][x][1] = 200;
            land[y][x][2] = 200;
            land[y][x][3] = 255;
        }
    }
    for (x = 28; x < 32; x++) {
        for (y = 4; y < 7; y++) {
            land[x][y][0] = 255;
            land[x][y][1] = 200;
            land[x][y][2] = 200;
            land[x][y][3] = 255;

            land[y][x][0] = 255;
            land[y][x][1] = 200;
            land[y][x][2] = 200;
            land[y][x][3] = 255;
        }
    }
    for (x = 29; x < 32; x++) {
        for (y = 7; y < 9; y++) {
            land[x][y][0] = 255;
            land[x][y][1] = 200;
            land[x][y][2] = 200;
            land[x][y][3] = 255;

            land[y][x][0] = 255;
            land[y][x][1] = 200;
            land[y][x][2] = 200;
            land[y][x][3] = 255;
        }
    }
    for (x = 30; x < 32; x++) {
        for (y = 9; y < 12; y++) {
            land[x][y][0] = 255;
            land[x][y][1] = 200;
            land[x][y][2] = 200;
            land[x][y][3] = 255;

            land[y][x][0] = 255;
            land[y][x][1] = 200;
            land[y][x][2] = 200;
            land[y][x][3] = 255;
        }
    }
    for (x = 31; x < 32; x++) {
        for (y = 12; y < 16; y++) {
            land[x][y][0] = 255;
            land[x][y][1] = 200;
            land[x][y][2] = 200;
            land[x][y][3] = 255;

            land[y][x][0] = 255;
            land[y][x][1] = 200;
            land[y][x][2] = 200;
            land[y][x][3] = 255;
        }
    }

    for (y = 16; y < 32; y++)
        for (x = 16; x < 32; x++) {
            land[x][y][0] = 200;
            land[x][y][1] = 160;
            land[x][y][2] = 255;
            land[x][y][3] = 255;
        }
    for (y = 20; y < 32; y++)
        for (x = 20; x < 32; x++) {
            land[x][y][0] = 255;
            land[x][y][1] = 200;
            land[x][y][2] = 255;
            land[x][y][3] = 255;
        }

    for (x = 63; x > 31; x--) {
        for (y = 0; y < 32; y++)
            for (i = 0; i < 4; i++)
                land[x][y][i] = land[63 - x][y][i];
    }

    /* Create the rest half part */
    for (y = 32; y < 64; y++)
        for (x = 0; x < 64; x++)
            for (i = 0; i < 4; i++)
                land[x][y][i] = land[x][63 - y][i];
}

/*Procedure to draw the floor.*/
void draw_floor()
{
    int  i, j;
    float range = 1.0;
    mat_shininess = 0.0;
    glMaterialfv(GL_FRONT, GL_AMBIENT, flr_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, flr_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, flr_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    glNormal3f(0.0, 1.0, 0.0);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, textName[0]);
    glEnable(GL_TEXTURE_2D);


    for (i = 0; i < 10; i++)
        for (j = 0; j < 10; j++) {
            if ((i + j) % 2 == 0) glColor3f(0.8, 0.8, 0.8);
            else glColor3f(0.2, 0.2, 0.4);
            glBegin(GL_POLYGON);
            glTexCoord2f(0.0, 0.0);
            glVertex3f((i - 5.0) * 10.0, -2.2, (j - 5.0) * 10.0);
            glTexCoord2f(0.0, range);
            glVertex3f((i - 5.0) * 10.0, -2.2, (j - 4.0) * 10.0);
            glTexCoord2f(range,range);
            glVertex3f((i - 4.0) * 10.0, -2.2, (j - 4.0) * 10.0);
            glTexCoord2f(range, 0.0);
            glVertex3f((i - 4.0) * 10.0, -2.2, (j - 5.0) * 10.0);
            glEnd();
        }

    glDisable(GL_TEXTURE_2D);
}

/*Procedure to draw a checkerboard texture.*/
void make_check()
{
    int i, j, c;

    for(i = 0;i < TSIZE;i++)
        for (j = 0; j < TSIZE; j++)
        {
            if (j > i / 2 && j <= 64 - i / 2)
                c = 255;
            else
                c = 0;
            checkboard[i][j][0] = c / 8;
            checkboard[i][j][1] = c / 2;
            checkboard[i][j][2] = c / 4;
            if (c == 255)
                checkboard[i][j][3] = 255;
            else 
                checkboard[i][j][3] = 0;
        }
    /*Generate trunk*/
    for (i = 0; i < TSIZE; i++)
    {
        for (j = 0; j < TSIZE; j++)
            checkboard[i][j][3] = 0;
        for (j = TSIZE / 2 + 4; j < TSIZE; j++)
            checkboard[i][j][3] = 0;
    }
}

/*Procedure to make a brick pattern.*/
void brick_pattern()
{
    int y, x, i;

    /*Create the first row of bricks.*/
    for (y = 0; y < 12; y++)
    {
        for (x = 0; x < 2; x++)
        {
            brick[y][x][0] = 140;
            brick[y][x][1] = 140;
            brick[y][x][2] = 140;
brick[y][x][3] = 255;
        }

        for (x = 2; x < 30; x++)
        {
            brick[y][x][0] = 200;
            brick[y][x][1] = 0;
            brick[y][x][2] = 0;
            brick[y][x][3] = 255;
        }

        for (x = 30; x < 34; x++)
        {
            brick[y][x][0] = 140;
            brick[y][x][1] = 140;
            brick[y][x][2] = 140;
            brick[y][x][3] = 255;
        }

        for (x = 34; x < 62; x++)
        {
            brick[y][x][0] = 200;
            brick[y][x][1] = 0;
            brick[y][x][2] = 0;
            brick[y][x][3] = 255;
        }

        for (x = 62; x < 64; x++)
        {
            brick[y][x][0] = 140;
            brick[y][x][1] = 140;
            brick[y][x][2] = 140;
            brick[y][x][3] = 255;
        }
    }

    /*Create the cement between layers.*/
    for (y = 12; y < 16; y++)
        for (x = 0; x < 64; x++)
        {
            brick[y][x][0] = 140;
            brick[y][x][1] = 140;
            brick[y][x][2] = 140;
            brick[y][x][3] = 255;
        }

    /*Create the second row of bricks.*/
    for (y = 16; y < 28; y++) {
        for (x = 0; x < 14; x++) {
            brick[y][x][0] = 200;
            brick[y][x][1] = 0;
            brick[y][x][2] = 0;
            brick[y][x][3] = 255;
        }

        for (x = 14; x < 18; x++) {
            brick[y][x][0] = 140;
            brick[y][x][1] = 140;
            brick[y][x][2] = 140;
            brick[y][x][3] = 255;
        }

        for (x = 18; x < 46; x++) {
            brick[y][x][0] = 200;
            brick[y][x][1] = 0;
            brick[y][x][2] = 0;
            brick[y][x][3] = 255;
        }

        for (x = 46; x < 50; x++) {
            brick[y][x][0] = 140;
            brick[y][x][1] = 140;
            brick[y][x][2] = 140;
            brick[y][x][3] = 255;
        }

        for (x = 50; x < 64; x++) {
            brick[y][x][0] = 200;
            brick[y][x][1] = 0;
            brick[y][x][2] = 0;
            brick[y][x][3] = 255;
        }
    }

    /*Add another layer of cement*/
    for (y = 28; y < 32; y++)
        for (x = 0; x < 64; x++) {
            brick[y][x][0] = 140;
            brick[y][x][1] = 140;
            brick[y][x][2] = 140;
            brick[y][x][3] = 255;
        }

    for (y = 32; y < 64; y++)
        for (x = 0; x < 64; x++)
            for (i = 0; i < 4; i++)
                brick[y][x][i] = brick[y - 32][x][i];
}

/*Procedure to make circle texture.*/
void Create_Texture_Circle() {
    int y, x, i, temp;
    for (y = 0; y < 64; y++) {
        for (x = 0; x < 64; x++) {
            circle[y][x][0] = 100;
            circle[y][x][1] = 100;
            circle[y][x][2] = 100;
            circle[y][x][3] = 255;
        }
    }
    for (y = 16, x = 0; y < 48; y++) {
        if (y <= 20 && y >= 17)
            x = x + 2;
        else if (y == 22 || y == 25 || y == 31)
            x = x + 1;
        else if (y == 42 || y == 39 || y == 33)
            x = x - 1;
        else if (y <= 47 && y >= 44)
            x = x - 2;
        temp = x;
        for (x = 0; x < temp; x++) {
            circle[y][x][0] = 150;
            circle[y][x][1] = 150;
            circle[y][x][2] = 150;
            circle[y][x][3] = 255;
        }
        for (x = 64 - temp; x < 64; x++) {
            circle[y][x][0] = 150;
            circle[y][x][1] = 150;
            circle[y][x][2] = 150;
            circle[y][x][3] = 255;
        }
        x = temp;
    }

    for (x = 16, y = 0; x < 48; x++) {
        if (x <= 20 && x >= 17)
            y = y + 2;
        else if (x == 22 || x == 25 || x == 31)
            y = y + 1;
        else if (x == 42 || x == 39 || x == 33)
            y = y - 1;
        else if (x <= 47 && x >= 44)
            y = y - 2;
        temp = y;
        for (y = 0; y < temp; y++) {
            circle[y][x][0] = 200;
            circle[y][x][1] = 200;
            circle[y][x][2] = 200;
            circle[y][x][3] = 255;
        }
        for (y = 64 - temp; y < 64; y++) {
            circle[y][x][0] = 200;
            circle[y][x][1] = 200;
            circle[y][x][2] = 200;
            circle[y][x][3] = 255;
        }
        y = temp;
    }
    for (i = 0; i < 61; i++) {
        for (y = i; y < 4 + i; y++) {
            for (x = i; x < 4 + i; x++) {
                circle[y][x][0] = 255;
                circle[y][x][1] = 255;
                circle[y][x][2] = 255;
                circle[y][x][3] = 255;
            }
        }
    }
    for (i = 0; i < 61; i++) {
        for (y = i; y < 4 + i; y++) {
            for (x = 60 - i; x < 64 - i; x++) {
                circle[y][x][0] = 255;
                circle[y][x][1] = 255;
                circle[y][x][2] = 255;
                circle[y][x][3] = 255;
            }
        }
    }
}

/*Procedure to calculate the a[] and b[] axes of a billboard, after specifying the eye parameters.*/
void compute_ab_axes(void)
{
    float w0, w2;
    double len;

    w0 = mtx[2];
    w2 = mtx[10];

    len = sqrt(w0 * w0 + w2 * w2);
    b[0] = 0.0;
    b[1] = 1.0;
    b[2] = 0.0;
    a[0] = w2 / len;
    a[1] = 0.0;
    a[2] = -w0 / len;
}

/*Procedure to draw the city.*/
void draw_city()
{
    /*The fixed light source is in the draw axes part, cuz I need it on one of the poles.*/

    /*Draw the tall building.*/
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, textName[1]);
    glPushMatrix();
    glTranslatef(45 ,22, 45);
    glScalef(10, 50, 10);
    texture_cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /*Draw the long building.*/
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, textName[2]);
    glPushMatrix();
    glTranslatef(-30, 2.5, -45);
    glScalef(40, 10, 10);
    texture_cube();
    //draw_colored_cube(0.5, 1.0, 0.8);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /*Draw the ball on top of the building.*/
    float mat_ambientb[] = {0.5, 0.5, 1.0, 1.0};
    float mat_diffuseb[] = {0.2, 0.2, 0.8, 1.0};
    float mat_specularb[] = {0.4, 0.4, 0.4, 1.0};
    mat_shininess = 100.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientb);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_diffuseb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_specularb);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    glPushMatrix(); 
    glTranslatef(-30, 15, -45);
    gluSphere(sphere, 10.0,12,12);           
    glPopMatrix();

    /*Draw the Christmas Tree.*/

    glPushMatrix();
    float mat_ambientt1[] = { 0.2, 0.1, 0.1, 1.0 };
    float mat_diffuset1[] = { 0.4, 0.1, 0.1, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientt1);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_diffuset1);

    glTranslatef(25,5,-45);
    glRotatef(90, 1.0, 0.0, 0.0);
    glScalef(2.0, 2.0, 2.0);
    glColor3f(0.5,0.25,0);
    gluCylinder(cylind, 2.0, 2.0, 4.0, 16, 16);

    glPushMatrix();
    float mat_ambientt[] = { 0.1, 0.2, 0.1, 1.0 };
    float mat_diffuset[] = { 0.1, 0.4, 0.1, 1.0 };
    float mat_speculart[] = { 0.4, 0.4, 0.4, 1.0 };
    mat_shininess = 20.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientt);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_diffuset);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_speculart);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);



    glTranslatef(-4, 0, 0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glScalef(8, 10, 1);
    glColor3f(0, 0.2, 0.1);
    draw_triangle();
    glPopMatrix();

    glTranslatef(-0.4, -4.3, 0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glRotatef(-90, 0.0, 1.0, 0.0);
    glScalef(8, 10, 1);
    draw_triangle();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(20, 10, -45);
    gluSphere(sphere, 0.1, 12, 12);
    glLightfv(GL_LIGHT4, GL_POSITION, lit4_position);
    glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, lit4_direction);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, lit4_diffuse);
    glLightfv(GL_LIGHT4, GL_SPECULAR, lit4_specular);
    glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, lit4_cutoff);
    glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, lit4_exponent);

    glTranslatef(7,10,0);
    gluSphere(sphere, 0.1, 12, 12);
    lit4_diffuse[0] = lit4_diffuse[2] = 0; lit4_diffuse[1] = 1;
    lit4_specular[0] = lit4_specular[2] = 0; lit4_specular[1] = 1;
    glLightfv(GL_LIGHT5, GL_POSITION, lit4_position);
    glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, lit4_direction);
    glLightfv(GL_LIGHT5, GL_DIFFUSE, lit4_diffuse);
    glLightfv(GL_LIGHT5, GL_SPECULAR, lit4_specular);
    glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, lit4_cutoff);
    glLightf(GL_LIGHT5, GL_SPOT_EXPONENT, lit4_exponent);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(25, 25, -45);
    gluSphere(sphere, 2, 12, 12);
    lit4_diffuse[0] = lit4_diffuse[1] = 1; lit4_diffuse[2] = 0;
    lit4_specular[0] = lit4_specular[1] = 1; lit4_specular[2] = 0;
    glLightfv(GL_LIGHT6, GL_POSITION, lit4_position);
    glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, lit4_direction);
    glLightfv(GL_LIGHT6, GL_DIFFUSE, lit4_diffuse);
    glLightfv(GL_LIGHT6, GL_SPECULAR, lit4_specular);
    glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, 10);
    glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, 2);
    glPopMatrix();
    /*Draw a statue.*/

    mat_shininess = 100.0;
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    glPushMatrix();
    glTranslatef(-30, -2, 30);
    glScalef(10, 1, 10);
    draw_colored_cube(0.8, 0.50, 0.52);

    glPushMatrix();
    glTranslatef(0, 1, 0);
    draw_colored_cube(1.0, 0.5, 0.6);
    glTranslatef(0, 1, 0);
    draw_colored_cube(0.5, 1.0, 0.6);
    glTranslatef(0, 1, 0);
    draw_colored_cube(0.5, 1.0, 1.0);
    glTranslatef(0, 1, 0);
    draw_colored_cube(0.5, 0.5, 1.0);
    glTranslatef(0, 1, 0);
    draw_colored_cube(0.2, 1.0, 0.6);
    glTranslatef(0, 1, 0);
    draw_colored_cube(0.5, 0.8, 0.2);
    glTranslatef(0, 1, 0);
    draw_colored_cube(0.1, 1.0, 0.6);
    glTranslatef(0, 1, 0);
    draw_colored_cube(0.5, 1.0, 0.2);
    glPopMatrix();
    glPopMatrix();

    /*Draw a road pavement*/
    glPushMatrix();
    glScalef(10, 1, 50);
    glTranslatef(2, -2.5, 0);
    draw_colored_cube(0.1, 0.1, 0.1);
    glPopMatrix();

    glPushMatrix();
    glScalef(10, 1, 50);
    glTranslatef(-2, -2.5, 0);
    draw_colored_cube(0.1,0.1,0.1);
    glPopMatrix();

    glPushMatrix();
    glScalef(50, 1, 10);
    glRotatef(90, 0.0, 1.0, 0.0);
    glTranslatef(-2, -2.5, 0);
    draw_colored_cube(0.1,0.1,0.1);
    glPopMatrix();

    glPushMatrix();
    glScalef(50, 1, 10);
    glRotatef(90, 0.0, 1.0, 0.0);
    glTranslatef(2, -2.5, 0);
    draw_colored_cube(0.1, 0.1, 0.1);
    glPopMatrix();

}

/*Procedure to draw three axes and the orign.*/
void draw_axes()
{
    /*Draw the fixed light on here.*/
    glPushMatrix();
    glTranslatef(0, 30, 0);
    
    float mat_ambient[] = { 0.0215, 0.1745, 0.0215	, 1.0 };
    float mat_diffuse[] = { 0.07568,0.61424 ,0.07568 , 1.0 };
    float mat_specular[] = { 0.633, 0.7278110, 0.633, 1.0 };
    mat_shininess = 0.6;
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    gluSphere(sphere, 1.0, 12, 12);
    glLightfv(GL_LIGHT0, GL_POSITION, lit_position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lit_direction);
    //glEnable(GL_LIGHT0);
    glPopMatrix();

    /*Draw a white sphere to represent the original*/
    glColor3f(0.9, 0.9, 0.9);

    gluSphere(sphere, 2.0,   /* radius=2.0 */
        12,            /* composing of 12 slices*/
        12);           /* composing of 8 stacks */

    /*Draw three axes in colors, yellow, meginta, and cyan*/
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
        30.0,             /* height of the cylinder */
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
    /* Restore the original modelview matrix */
    glPopMatrix();
}

/*Make viewing matrix.*/
void make_view(int x)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (x) {
    case 4:       /* Perspective */

      /* In this sample program, eye position and Xe, Ye, Ze are computed
         by ourselves. Therefore, use them directly; no transform is
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

/*Procedure to make projection matrix.*/
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

/*Procedure to draw view volume, eye position, focus ,...for perspective projection.*/
void draw_view()
{
    int    i;

    glMatrixMode(GL_MODELVIEW);

    /*Draw Eye position.*/
    glPushMatrix();
    glTranslatef(eye[0], eye[1], eye[2]);
    glColor3f(0.0, 1.0, 0.0);
    glutWireSphere(1.0, 10, 10);
    glPopMatrix();

    /*Draw eye coord. axes.*/
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

/*Display callback procedure to draw the scene.*/
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
    /*Swap the back buffer to the front.*/
    glutSwapBuffers();
    glFlush();
    return;

}

/*This procedure draw the heli. */
void draw_heli()
{
    mat_shininess = 0.21794872;
    float mat_ambient[] = { 0.329412, 0.223529, 0.027451, 1.0 };
    float mat_specular[] = { 0.780392, 0.568627, 0.113725, 1.0 };
    float mat_diffuse[] = { 0.992157, 0.941176, 0.807843, 1.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


    /*Draw the heli body which is a cube.*/
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
    glPushMatrix();
    glTranslatef(0, -0.5, 0);
    glColor3f(0.0, 1.0, 0.0);
    gluSphere(sphere, 0.1, 12, 12);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lit1_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lit1_specular);
    glLightfv(GL_LIGHT2, GL_POSITION, lit1_position);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, lit1_direction);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, lit1_cutoff);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, lit1_exponent);

    //glEnable(GL_LIGHT2);
    glPopMatrix();
    glPopMatrix();               /* Get M1 back */

    /*Draw the skids.*/
    glPushMatrix();              /* Save M1 coord. sys */
    glTranslatef(-5.0, -5.5, -3.0); /* Go to left skid position */
    glRotatef(90.0, 0.0, 1.0, 0.0);  /*Rotate about x by -90', z becomes y */
    gluCylinder(cylind, 0.5, 0.5, /* radius of top and bottom circle */
        10.0,             /* height of the cylinder */
        12,               /* use 12-side polygon approximating circle*/
        3);               /* Divide it into 3 sections */
    glPopMatrix();

    glPushMatrix();              /* Save M1 coord. sys */
    glTranslatef(-5.0, -5.5, 3.0); /* Go to right wheel position */
    glRotatef(90.0, 0.0, 1.0, 0.0);  /*Rotate about x by -90', z becomes y */
    gluCylinder(cylind, 0.5, 0.5, 10.0, 12, 3);
    glPopMatrix();

    glPushMatrix();              /* Save M1 coord. sys */
    glTranslatef(2.5, 0.75, 0.5);
    glRotatef(90.0, 0.0, 1.0, 0.0);  /*Rotate about x by -90', z becomes y */
    gluCylinder(cylind, 0.5, 0.5, 10.0, 12, 3);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(12, 0.75, 0.5);
    gluSphere(sphere, 1.0, 12, 12);
    glPopMatrix();

    glPushMatrix();

    float lit3_diffuse[] = {1.0,0.0,0.0,1.0};
    float lit3_specular[] = { 1.0,0.0,0.0,1.0 };
    glLightfv(GL_LIGHT3, GL_DIFFUSE, lit3_diffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, lit3_specular);
    glLightfv(GL_LIGHT3, GL_POSITION, lit1_position);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, lit1_direction);
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, lit1_cutoff);
    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, lit1_exponent);
    //glEnable(GL_LIGHT3);
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

/*This procedure draws a car.*/
void draw_car()
{
    mat_shininess = 0.25;
    float mat_ambient[] = { 0, 0, 0, 1.0 };
    float mat_specular[] = { 0.01, 0.01, 0.01, 1.0 };
    float mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    glPushMatrix();
    /*Draw the car body which is a cube*/
    glTranslatef(carpos[0], 0.9, carpos[2]);
    glRotatef(car_ang, 0.0, 1.0, 0.0);
        
    glPushMatrix();
    glTranslatef(9, 1.2,-2.5);
    glColor3f(0.0, 1.0, 0.0);
    gluSphere(sphere,1, 12, 12);
    glLightfv(GL_LIGHT1, GL_POSITION, carlit_position);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, carlit_direction);
    //glEnable(GL_LIGHT1);

    glTranslatef(0, 0, 5);
    glColor3f(0.0, 1.0, 0.0);
    gluSphere(sphere, 1, 12, 12);
    glPopMatrix();

    glPushMatrix();              /* Save M1 coord. sys */
    glScalef(17.0, 3.0, 6.0);    /* Scale up the axes */
    draw_colored_cube(0.75, 0.75, 0.75);
    glPopMatrix();               /* Get M1 back */

    glPushMatrix();
    glTranslatef(4.5, 5.5, 0);
    glScalef(8, 8, 6);
    draw_colored_cube(0.6, 0.6, 0.6);
    glPopMatrix();

    /*Draw the front wheels*/
    mat_shininess = 0.0;
    float mat_ambientw[] = { 0.0, 0.0, 0.0, 1.0 };
    float mat_specularw[] = { 0.0, 0.0, 0.0, 1.0 };
    float mat_diffusew[] = { 0.2, 0.2, 0.2, 1.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientw);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specularw);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffusew);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    glColor3f(0, 0, 0);
    glPushMatrix();              /* Save M1 coord. sys */
    glTranslatef(-6.0, 0.0, 4.0); /* Go to left wheel position */
    glutSolidTorus(1.0,  /* inner radius */
        2.0,  /* outer radius */
        24,   /* divided into 18 segments */
        12);  /* 12 rings */
    glPopMatrix();

    glPushMatrix();              /* Save M1 coord. sys */
    glTranslatef(-6.0, 0.0, -4.0);/* Go to right wheel position */
    glutSolidTorus(1.0,  /* inner radius */
        2.0,  /* outer radius */
        24,   /* divided into 18 segments */
        12);  /* 12 rings */
    glPopMatrix();

    /*Draw back wheels*/
    glColor3f(0, 0, 0);
    glPushMatrix();              /* Save M1 coord. sys */
    glTranslatef(6.0, 0.0, 4.0); /* Go to left wheel position */
    glutSolidTorus(1.0,  /* inner radius */
        2.0,  /* outer radius */
        24,   /* divided into 18 segments */
        12);  /* 12 rings */
    glPopMatrix();

    glPushMatrix();              /* Save M1 coord. sys */
    glTranslatef(6.0, 0.0, -4.0);/* Go to right wheel position */
    glutSolidTorus(1.0,  /* inner radius */
        2.0,  /* outer radius */
        24,   /* divided into 18 segments */
        12);  /* 12 rings */
    glPopMatrix();
    glPopMatrix();
}

/*Procedure to draw fixed light.*/
void draw_lights()
{
    ;
}

/*Procedure to draw a billboard, center =[x,z], width = w, height = h.*/
void draw_billboard(float x, float z, float w, float h)
{
    float v0[3], v1[3], v2[3], v3[3];

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    /*Compute the 4 vertices of the the billboard.*/
    v0[0] = x - (w / 2) * a[0];
    v0[1] = 0.0;
    v0[2] = z - (w / 2) * a[2];
    v1[0] = x + (w / 2) * a[0];
    v1[1] = 0.0;
    v1[2] = z + (w / 2) * a[2];
    v2[0] = x + (w / 2) * a[0];
    v2[1] = h;
    v2[2] = z + (w / 2) * a[2];
    v3[0] = x - (w / 2) * a[0];
    v3[1] = h;
    v3[2] = z - (w / 2) * a[2];

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3fv(v0);
    glTexCoord2f(1.0, 0.0);
    glVertex3fv(v1);
    glTexCoord2f(1.0, 1.0);
    glVertex3fv(v2);
    glTexCoord2f(0.0, 1.0);
    glVertex3fv(v3);
    glEnd();
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
}

void draw_scene()
{    
    draw_lights();
    
    draw_floor();

    draw_city();

    draw_axes();

    draw_car();
    
    draw_heli();

}

/*Reshape callback function which defines the size of the main window when a reshape event occurrs.*/
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

/*Callback function for top menu. Do nothing.*/
void top_menu(int value)
{
    ;
}

/*Callback function for top menu.*/
void file_func(int value)
{
    if (value == 1) fprintf(stderr, "this is 1\n");
    if (value == 2) fprintf(stderr, "this is 2\n");
    if (value == 3) fprintf(stderr, "this is 3\n");
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

/*Keyboard callback func. When the <esc> key is pressed, the program is aborted.*/
void my_quit(unsigned char key, int ix, int iy)
{
    int    i;
    float  x[3], y[3], z[3];

    /*Controls the helicopter.*/
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
        position[2] += Step * cos(self_ang * PI / 180.0);
        position[0] += Step * sin(self_ang * PI / 180.0);
    }
    else if (key == 'e')                          /*Strafe right.*/
    {
        if (direction != 4) { direction = 4; debugg(4); return; };
        position[2] -= Step * cos(self_ang * PI / 180.0);
        position[0] -= Step * sin(self_ang * PI / 180.0);
    }

    /*mouse event determines the rise or landing of the helicopter.*/

    /*transform the EYE coordinate system.*/
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

/*Idle function when there is no key nor mouse inputs.*/
void idle()
{
    /*Specials*/
    int specials = 103;
    if (sp2)
    {
        i++;
        if (i <= (int)(specials / 2))
        {
            glDisable(GL_LIGHT4);
            glDisable(GL_LIGHT5);
            glDisable(GL_LIGHT6);
        }
        else if (i < specials)
        {
            glEnable(GL_LIGHT4);
            glEnable(GL_LIGHT5);
            glEnable(GL_LIGHT6);
        }
        if (i == specials) i = 0;
    }
    if (sp1)
    {
        j++;
        if (j == specials)
        {
            j == 0;
            lit_diffuse[0] = lit_diffuse[1] = lit_diffuse[2] = (float)rand() / (float)RAND_MAX;
            glLightfv(GL_LIGHT0, GL_DIFFUSE, lit_diffuse);
            glEnable(GL_LIGHT0);
        }
    }

    /*Car part*/
    car_dist += 1;
    carpos[0] += Step * cos(car_ang * PI / 180.0);
    carpos[2] -= Step * sin(car_ang * PI / 180.0);
    if (car_dist == 75)
    {
        car_ang += 90;
        car_dist = 0;
    }
    if (car_ang >= 360) car_ang -= 360;

    display();

    /*Helicopter part*/
    if (!depart) { direction = 0; return; }
    int interval = 100;
    if (rounds < interval/2)
    {
        rounds++; glEnable(GL_LIGHT3);
    }
    if (rounds >= interval/2)
    {
        rounds++; glDisable(GL_LIGHT3);
    }
    if (rounds == interval)
    {
        rounds = 0;
    }

    blade_angle += 10;
    if (blade_angle >= 360) blade_angle -= 360;
    if (blade_angle <= 360) blade_angle += 360;

}

/*Procedure to initialize the working environment.*/
void myinit()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);      /*set the background color BLACK */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);/*Clear the Depth & Color Buffers */

    glEnable(GL_DEPTH_TEST);  /*Enable depth buffer for shading computing */
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);   /*Enable normalization  */

    glEnable(GL_LIGHTING);    /*Enable lighting effects */
    //glEnable(GL_LIGHT0);    /*Turn on light0 */
    glEnable(GL_LIGHT1);      /*Turn on light1 */
    glEnable(GL_LIGHT2);

    /*Define light0 - lamp post.*/
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, lit_cutoff);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lit_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lit_specular);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, lit_exponent);

    /*Define light1 - car*/
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, carlit_cutoff);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, carlit_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, carlit_specular);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, carlit_exponent);

    /*Define light2 - heli*/
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lit1_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lit1_specular);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, lit1_cutoff);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, lit1_exponent);

    glLightfv(GL_LIGHT3, GL_DIFFUSE, lit1_diffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, lit1_specular);
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, lit1_cutoff);
    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, lit1_exponent);

    /*Define light4 - christmas tree*/
    glLightfv(GL_LIGHT4, GL_DIFFUSE, lit4_diffuse);
    glLightfv(GL_LIGHT4, GL_SPECULAR, lit4_specular);
    glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, lit4_cutoff);
    glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, lit4_exponent);

    /*Define some global lighting status*/
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); /* local viewer */
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient); /*global ambient*/

    /*Enable face culling*/
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    /*Create quadratic objects*/
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

    /*Create Texture*/
    make_check();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(2, textName);
    glBindTexture(GL_TEXTURE_2D, textName[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TSIZE, TSIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkboard);

    /*Create another texture*/
    brick_pattern();
    glBindTexture(GL_TEXTURE_2D, textName[1]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TSIZE, TSIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, brick);

    Create_Texture_Circle();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textName[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, circle);

}

/*Main procedure which defines the graphics environment,the operation style, and the callback func's.*/
void main(int argc, char** argv)
{
    fprintf(stderr, "Controls:\n<esc> = quit\nw = forward\ns = backwards\na/d = rotate clockwise/CC wise.\n<space> = rotate light.\n");
    fprintf(stderr, "Mouse:\nLMB/RMB = lifting the helicopter\nMMB = popup menu\n");
    fprintf(stderr, "----------Eye Coordinate System----------\ni/k = move up/down\nj/l = move left/right\nu/o = move near/far\n");
    fprintf(stderr, "M/m = pitching\n</, = heading\n>/. = rotate\n0 = all viewport view\n4 = perspective view\n");
    fprintf(stderr, "Debug Messages:\n");
    /*Initialize the GLUT environment*/
    glutInit(&argc, argv);

    /*Depth buffer is used, be careful !!!*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(width, height);
    glutCreateWindow("helicopter helicopter");

    myinit();      /*Initialize other state varibales*/

    /*Associate callback func's whith events*/
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(my_reshape);
    glutKeyboardFunc(my_quit);
    glutMouseFunc(mouse_event_mine);

    lightColor_m = glutCreateMenu(blinding_lights);
    glutAddMenuEntry("red", 1);
    glutAddMenuEntry("green", 2);
    glutAddMenuEntry("blue", 3);
    glutAddMenuEntry("white", 4);

    lightOn_m = glutCreateMenu(blinding_lights);
    glutAddMenuEntry("On", 11);
    glutAddMenuEntry("Off", 12);
    glutAddMenuEntry("Low Intensity", 13);
    glutAddMenuEntry("HIGH INTENSITY", 14);

    heliLight_m = glutCreateMenu(blinding_lights);
    glutAddMenuEntry("On", 21);
    glutAddMenuEntry("Off", 22);
    glutAddMenuEntry("Low Intensity", 23);
    glutAddMenuEntry("HIGH INTENSITY", 24);
    glutAddMenuEntry("small cutoff", 25);
    glutAddMenuEntry("BIG CUTOFF", 26);

    truckLight_m = glutCreateMenu(blinding_lights);
    glutAddMenuEntry("On", 31);
    glutAddMenuEntry("Off", 32);
    glutAddMenuEntry("Low Intensity", 33);
    glutAddMenuEntry("HIGH INTENSITY", 34);

    specials_m = glutCreateMenu(blinding_lights);
    glutAddMenuEntry("?_?", 99);
    glutAddMenuEntry("*_*", 98);

    top_m = glutCreateMenu(top_menu);
    glutAddSubMenu("lightColors", lightColor_m);
    glutAddSubMenu("lightsOn?", lightOn_m);
    glutAddSubMenu("HeliLightsOn?", heliLight_m);
    glutAddSubMenu("TruckLightsOn?", truckLight_m);
    glutAddSubMenu("???", specials_m);
    glutAttachMenu(GLUT_MIDDLE_BUTTON);

    glutMainLoop();

    srand((unsigned int)time(NULL));
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
    case 14:
        fprintf(stderr, "Current time is %d:00.\n");
        break;
    default:
        fprintf(stderr, "No message available. Add some more.\n");
        break;
    }
}

/*Procedure to compute norm of vector v[].*/
float norm2(float v[])
{
    double  sum;
    float   temp;

    sum = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    temp = (float)sqrt(sum);
    return temp;
}
