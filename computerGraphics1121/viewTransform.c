/*
This program will show a flying helicopter. The user can change the viewing angles between the three axes, and rotate/transform the camera.

Author: WEI DOONG WONG
		Dept. of Computer Science and Engineering
		National Taiwan Ocean University
Date:	22nd November 2023
*/

#include<stdio.h>
#include<math.h>
#include<GL/glut.h>
#include<stdbool.h>

#define PI 3.1415927

/*Define polygon drawing models, filled or outlined.*/
#define FILL 1
#define LINE 2

/*Procedure to initialize the working environment.*/
void my_init();

/*Procedure to draw the blades of the helicopter.*/
void draw_blade();

/*Procedure to draw the cube. Geometric data has been provided above.*/
void draw_cube();

/*Procedure to allow specified color cubes to be drawn.*/
void colored_cube(float r, float g, float b);

/*Procedure to draw the floor like a checkerboard.*/
void draw_floor();

/*Procedure to draw a cylinder.*/
void draw_cylinder();

/*Procedure to draw a sphere.*/
void draw_sphere();

/*Procedure to draw the city.*/
void draw_city();

/*Procedure to draw the three axes and the origin.*/
void draw_axes();

/*Display function.*/
void display();

/*Idle function that will increase the rotation angle of the blades over time.*/
void rotation_func();

/*Windows reshape event callback function.*/
void my_reshape(int w, int h);

/*Callback function for keyboard events.*/
void my_quit(unsigned char key, int x, int y);

/*For a cube.*/
/*Coordinates of the 8 corners.*/
float points[][3] = { {0.0,0.0,0.0},{1.0,0.0,0.0},
					{1.0,0.0,1.0},{0.0,0.0,1.0},
					{0.0,1.0,0.0},{1.0,1.0,0.0},
					{1.0,1.0,1.0},{0.0,1.0,1.0} };

/*Define the 6 faces of a cube using the 8 vertices.*/
int face[][4] = { {0,1,2,3},{7,6,5,4},{0,4,5,1},
				{1,5,6,2},{3,2,6,7},{0,3,7,4} };

/*Define the cube using the 6 faces.*/
int cube[6] = { 0,1,2,3,4,5 };

/*Define colors.All of them are pink.*/
float  color[][3] = { {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0},
					{1.0, 1.0, 0.0},{1.0, 0.0, 1.0},{0.0, 1.0, 1.0} };


/*Window shape.*/
int width = 1000, height = 1000;

/*Define a base position in the z-x plane.*/
float pos[3] = { 0.0,0.0,0.0 }, act_pos[3] = { 0.0, 0.0, 0.0 };

/*Declare the rotational angle.*/
float angle = 0.0, tilt = 0.0, yaw = 0.0;

/*Variables for recording rotation angles of the blades.*/
float rear_blade = 90.0, top_blade = 45.0;

/*Define the translations and rotation of the eye coordinate system.*/
float eyeDx = 0.0, eyeDy = 0.0, eyeDz = 0.0;
float eyeAngx = 0.0, eyeAngy = 0.0, eyeAngz = 0.0;
double Eye[3] = { 0.0, 0.0, 30.0 }, Focus[3] = { 0.0,0.0,0.0 }, Vup[3] = { 0.0,1.0,0.0 };

float u[3][3] = { {1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0} };
float eye[3];
float cv, sv; /*cos(5.0) and sin(5.0)*/

/*Drawing style:
0 = 4 windows.
1 = x-axis.
2 = y-axis.
3 = z-axis.
4 = perspective.
*/
int style = 0;
//void draw_scene(void);
//float norm2(float v[]);

/*To draw objects in the world coordinate system.*/
/*First, define some GLU quadric objects, like a sphere and a cylinder.*/
GLUquadricObj* sphere = NULL, * cylind = NULL, * disk = NULL;
int polygonMode = FILL;

/*Global booleans to detect flight and tilt.*/
bool start = false, forward_tilt = false, left_tilt = false, right_tilt = false, backward_tilt = false;

void my_init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);						/*Sets clear color to BLACK.*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		/*Clears the Depth and Color buffers.*/
	glViewport(0, 0, 999, 999);

	/*Set a parallel projection mode.*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-8.0, 8.0, -8.0, 8.0, 0.0, 20.0);
	glEnable(GL_DEPTH_TEST);								/*Enables depth buffer for shading computing.*/
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



	glFlush();												/*Enfore window system to display the results.*/
}

void draw_blade()
{
	glColor3f(1.0, 0.5, 0.5);

	glBegin(GL_POLYGON);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0, 4.0, 0.0);
	glVertex3f(1.0, 8.0, 0.0);
	glVertex3f(-1.0, 8.0, 0.0);
	glVertex3f(-1.0, 4.0, 0.0);
	glEnd();
}

void draw_cube()
{
	int i;

	glPolygonMode(GL_FRONT, GL_FILL);
	for (i = 0; i < 6; i++)				/*Draw the 6 sides of the cube.*/
	{
		glColor3f(1.0, 0.7, 0.8);			/*Sets the current color.*/

		glBegin(GL_POLYGON);			/*Draws the faces.*/
		glVertex3fv(points[face[i][0]]);
		glVertex3fv(points[face[i][1]]);
		glVertex3fv(points[face[i][2]]);
		glVertex3fv(points[face[i][3]]);
		glEnd();
	}
}

void colored_cube(float r, float g, float b)
{
	int i;

	glPolygonMode(GL_FRONT, GL_FILL);
	for (i = 0; i < 6; i++)				/*Draw the 6 sides of the cube.*/
	{
		glColor3f(r, g, b);			/*Sets the current color.*/

		glBegin(GL_POLYGON);			/*Draws the faces.*/
		glVertex3fv(points[face[i][0]]);
		glVertex3fv(points[face[i][1]]);
		glVertex3fv(points[face[i][2]]);
		glVertex3fv(points[face[i][3]]);
		glEnd();
	}
}

void draw_floor()
{
	int i, j;
	if (polygonMode == LINE) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (i = 0; i < 10; i++)
		for (j = 0; j < 10; j++)
		{
			if ((i + j) % 2 == 0) glColor3f(0.9, 0.9, 0.9);
			else glColor3f(0.2, 0.2, 0.2);

			glBegin(GL_POLYGON);
			glVertex3f(i, 0.0, j);
			glVertex3f(i, 0.0, j + 1);
			glVertex3f(i + 1, 0.0, j + 1);
			glVertex3f(i + 1, 0.0, j);
			glEnd();
		}
}

void draw_cylinder()
{
	if (cylind == NULL)
	{
		cylind = gluNewQuadric();
	}
	/*Draw a cylinder.*/
	glColor3f(1.0, 0.8, 0.7);
	gluCylinder(cylind, 0.3, 0.3, 4.0, 12, 4);
	/*radius of top and bottom circle, height of the cylinder, use 12-side polygon apporximating circle, divide it into 4 sections.*/

}

void draw_sphere()
{
	if (sphere == NULL)
	{
		sphere = gluNewQuadric();
		gluQuadricDrawStyle(sphere, GLU_FILL);
	}
	glColor3f(1.0, 0.4, 0.4);
	gluSphere(sphere, 1.0, 12, 12);/*redius,composing of 12 slices, composing of 12 stacks.*/
}

void draw_city()
{
	glPushMatrix();

	glTranslatef(0.0, 0.0, 3.0);
	glScalef(1.0, 3.0, 1.0);
	colored_cube(1.0, 0.2, 0.2);

	glTranslatef(0.0, 0.0, 6.0);
	glScalef(1.0, 3.0, 1.0);
	colored_cube(0.9, 0.7, 0.5);

	glTranslatef(3.0, 0.0, -4.0);
	glScalef(1.0, 0.5, 1.0);
	colored_cube(0.1, 0.7, 0.5);

	glTranslatef(0.0, 0.0, -5.0);
	glScalef(1.0, 0.25, 3.0);
	colored_cube(0.79, 0.95, 0.88);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(7.5, 0.0, 7.5);
	glScalef(0.25, 3.0, 0.25);
	colored_cube(0.15, 0.1, 0.1);

	glTranslatef(0.0, 1.0, 0.0);
	glScalef(4, 0.5, 4);
	colored_cube(0.7, 0.95, 0.7);

	glTranslatef(0.0, -2.15, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glScalef(0.5, 0.5, 0.5);
	glColor3f(0.4, 0.9, 0.4);
	glutSolidTorus(1.0, 2.0, 24, 12);

	glPopMatrix();


	glPushMatrix();
	glTranslatef(8.0, 1.0, 1.0);
	glColor3f(1.0, 0.8, 0.0);
	glRotatef(rear_blade, 0.0, 1.0, 0.0);
	glScalef(0.1, 0.1, 0.1);
	glutSolidTorus(1.0, 2.0, 24, 12);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3, 8, 9);
	glRotatef(rear_blade, 0.0, 1.0, 0.0);
	glScalef(0.1, 0.1, 0.1);
	glutSolidTorus(1.0, 2.0, 24, 12);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2, 1, 2);
	glRotatef(rear_blade, 0.0, 1.0, 0.0);
	glScalef(0.1, 0.1, 0.1);
	glutSolidTorus(1.0, 2.0, 24, 12);

	glPopMatrix();

}

void draw_axes()
{
	glPushMatrix();
	glScalef(0.5, 0.5, 0.5);
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

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);/*Clear previous frame and the depth buffer.*/

	glMatrixMode(GL_MODELVIEW);							/*Define the current eye positon and the eye- coordinate system.*/
	glLoadIdentity();
	gluLookAt(12.0, 12.0, 12.0, 5.0, 0.0, 5.0, 0.0, 1.0, 0.0);
	draw_axes();
	draw_floor();										/*Draw the floor.*/
	draw_city();


	glTranslatef(pos[0], pos[1], pos[2]);
	glPushMatrix();

	glRotatef(tilt, 1.0, 0.0, 0.0);
	glRotatef(yaw, 0.0, 0.0, 1.0);
	glRotatef(angle, 0.0, 1.0, 0.0);

	glTranslatef(0.25, 0.25, 0.25);				/*Move to the base position.*/
	/*Draw the first cube.*/
	glPushMatrix();
	glScalef(0.75, 0.75, 0.75);
	draw_cube();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, -0.25, 0.0);
	/*Draw the skids.*/
	glScalef(0.25, 0.25, 0.25);
	draw_cylinder();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.5, 0.0, 0.0);
	glScalef(0.25, 0.25, 0.25);
	draw_cylinder();
	glPopMatrix();

	/*Draw the rear.*/
	glPushMatrix();
	glTranslatef(0.5, 0, -1);
	glScalef(0.25, 0.25, 0.25);
	draw_cylinder();
	glPopMatrix();

	/*Draw the ball bearing for the rear blades.*/
	glPushMatrix();
	glTranslatef(0.5, 0, -1);
	glScalef(0.05, 0.1, 0.05);
	draw_sphere();
	glPopMatrix();

	/*Draw the blades for the rear motor.*/
	glPushMatrix();
	glTranslatef(0.5, 0, -1);
	glRotatef(90, 0.0, 1.0, 0.0);
	glRotatef(rear_blade, 0.0, 0.0, 1.0);
	glScalef(0.05, 0.05, 0.05);
	draw_blade();

	glRotatef(90, 0.0, 0.0, 1.0);
	draw_blade();

	glRotatef(90, 0.0, 0.0, 1.0);
	draw_blade();

	glRotatef(90, 0.0, 0.0, 1.0);
	draw_blade();
	glPopMatrix();

	/*Draw the top motor.*/
	glPushMatrix();
	glTranslatef(0.5, 0.5, 0.5);
	glRotatef(90, -1.0, 0.0, 0.0);
	glScalef(0.25, 0.1, 0.25);
	draw_cylinder();
	glPopMatrix();

	/*Draw the ball bearing for the top blades.*/
	glPushMatrix();
	glTranslatef(0.5, 1.5, 0.5);
	glScalef(0.05, 0.1, 0.05);
	draw_sphere();
	glPopMatrix();

	/*Draw the top rotor blades.*/
	glPushMatrix();
	glTranslatef(0.5, 1.5, 0.5);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(top_blade, 0.0, 0.0, 1.0);
	glScalef(0.1, 0.2, 0.1);
	draw_blade();

	glRotatef(180, 0.0, 0.0, 1.0);
	draw_blade();

	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.5, 1.5, 0.5);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(top_blade + 90, 0.0, 0.0, 1.0);
	glScalef(0.1, 0.2, 0.1);
	draw_blade();

	glRotatef(180, 0.0, 0.0, 1.0);
	draw_blade();

	glPopMatrix();
	glPopMatrix();

	glFlush();
	glutSwapBuffers();


}

void rotation_func()
{
	if (start)
	{
		rear_blade += 100; top_blade += 100;
	}
	if (rear_blade >= 360) rear_blade -= 360;
	if (top_blade >= 360) top_blade -= 360;

	if (tilt > 360) tilt -= 360;
	if (tilt < -360) tilt += 360;
	if (yaw > 360) yaw -= 360;
	if (yaw < -360) yaw += 360;

	display();
}

void my_reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w > h)
	{
		glOrtho(-10, 10.0, -10.0 * (float)h / (float)w, 10.0 * (float)h / (float)w, 0.50, 30.0);
	}
	else
	{
		glOrtho(-10 * (float)w / (float)h, 10.0 * (float)h / (float)w, -10.0, 10.0, 0.50, 30.0);
	}
	width = w; height = h;
}

void my_quit(unsigned char key, int x, int y)
{
	if (key == 'Q' || key == 'q') { fprintf(stderr, "helicopter helicopter\n"); exit(0); }
	if (key == 'Z')
	{
		if (pos[2] < 9.0 && forward_tilt) { pos[2] += 1.0; fprintf(stderr, "Moved in postive z-axis\n"); }
	}
	else if (key == 'z')
	{
		if (pos[2] > 0.0 && backward_tilt) { pos[2] -= 1.0; fprintf(stderr, "Moved in negative z-axis\n"); }
	}
	if (key == 'x')
	{
		if (pos[0] > 0.0 && left_tilt) { pos[0] -= 1.0; }
	}
	else if (key == 'X')
	{
		if (pos[0] < 9.0 && right_tilt) { pos[0] += 1.0; }
	}
	if (key == 'u')
	{
		if (pos[1] > 0.0) { pos[1] -= 1.0; }
		if (pos[1] <= 0.0) { start = false;  fprintf(stderr, "Landed\n"); }
	}
	else if (key == 'U')
	{
		if (pos[1] < 9.0) { pos[1] += 1.0; start = true; fprintf(stderr, "Take off\n"); }

	}
	if (key == 'T')
	{
		tilt += 1;
		if (tilt >= 10) { forward_tilt = true; fprintf(stderr, "forward unlocked.\n"); }
		if (tilt > -10) {
			backward_tilt = false;  fprintf(stderr, "backward locked.\n");
		}
	}
	else if (key == 't')
	{
		tilt -= 1;
		if (tilt < 10) { forward_tilt = false;  fprintf(stderr, "forward locked.\n"); }
		if (tilt <= -10) { backward_tilt = true; fprintf(stderr, "backward unlocked.\n"); }
	}
	if (key == 'y')
	{
		yaw += 1;
		if (yaw >= 10) { left_tilt = true;  fprintf(stderr, "left unlocked.\n"); }
		if (yaw > -10) { right_tilt = false; fprintf(stderr, "right locked.\n"); }
	}
	else if (key == 'Y')
	{
		yaw -= 1;
		if (yaw < 10) { left_tilt = false;  fprintf(stderr, "left locked.\n"); }
		if (yaw <= -10) { right_tilt = true; fprintf(stderr, "right unlocked.\n"); }
	}


	if (key == 'r' || key == 'R')
	{
		angle += 3.0;
	}
	if (key == 'L' || key == 'l')
	{
		angle -= 3.0;
	}

	if (key == '1') { polygonMode = FILL; fprintf(stderr, "Fill mode to see tiles.\n"); }
	else if (key == '2') { polygonMode = LINE; fprintf(stderr, "Line mode to see tile outlines.\n"); }

	if (angle >= 360.0) angle -= 360.0;
	else if (angle < 0.0) angle += 360.0;

	display();
}

/*Main procedure which defines the graphics environment, the operation style and the callback functions.*/

int main(int argc, char** argv)
{
	fprintf(stderr, "Welcome!\nInstructions:\nZ = move in positive z-axis.\nz = move in negative z-axis.\nX = move in positive x-axis.\nx = move in negative x-axis.\n1/2 = change color fill mode(fill/line) \nQ/q = Quit!\n");
	fprintf(stderr, "Debug Console\n");
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);	/*Advise caution when using depth buffer.*/
	glutInitWindowSize(width, height);
	glutCreateWindow("Helicopter Helicopter");

	my_init();													/*Initiate other state variables.*/

	/*Associate callback functions with events.*/
	glutDisplayFunc(display);
	glutIdleFunc(rotation_func);
	glutReshapeFunc(my_reshape);
	glutKeyboardFunc(my_quit);
	glutMainLoop();
}