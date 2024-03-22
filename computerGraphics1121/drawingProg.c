/*************************************************************************************
This program is a drawing program,by using the OpenGL library it should be able to
provide a GUI and let the user to draw primitives in a window,
different colours and thickness of the brush will be provided,
pop up menu, resizable window and keyboard/mouse triggered events will be available.
Copyright:	W. D. WONG
			Dept. of Computer Science and Software Engineering, National Taiwan Ocean Uni.
			1st Oct 2023.


**************************************************************************************/

#include <stdio.h>			
#include <stdlib.h>
#include <GL/freeglut.h>	/*glut.h*/
#include <math.h>

/*Definition for window size, function call references, colour and shape.*/

#define WINWIDTH 800
#define WINHEIGHT 800

#define QUIT_PRG 1
#define CLR_WIN 2
#define SV_IMG 3
#define BLND_IMG 4
#define LD_IMG 5
#define BONUS 6

#define WHITE 1
#define RED 2
#define GREEN 3
#define BLUE 4

#define POINT 1
#define LINE 2
#define POLYGON 3
#define CIRCLE 4
#define CURVE 5

typedef int menu_t;
menu_t top_m, color_m, file_m, type_m;

int height = 512, width = 512;
unsigned char image[WINWIDTH * WINHEIGHT][4];

int		pos_x = -1, pos_y = -1;
float	myColor[3] = { 0.0, 0.0, 0.0 };
int		obj_type = -1;
int		first = 0;						/*flag of initial points for lines ,curve and other primitives.*/
int		vertex[128][2];					/*Coordinates of vertices.*/
int		side = 0;						/*Number of sides for a polygon.*/
float	pnt_size = 1.0;



/*Width and height of the window*/
int width, height;

/*Draw a polygon.*/
void polygon_drawing();

/*Draw a circle.*/
void circle_drawing();

/*Callback function of display event, this procedure is called when the window shows up.*/
void window_display();

/*Callback function of reshape event. w and h are the new width and height respectively.*/
void window_reshape(int w, int h);

/*Callback function for keyboard events*/
void keyboard_events(unsigned char key, int x, int y);

/*Callback function for mouse-motion event. (x,y) is the current cursor position in the window.*/
void motion_mouse(int x, int y);

/*Callback function for cleaning the window.*/
void init_window();

/*Callback function to initialize data alignment and miscellaneous functions.*/
void init_function();

/*Callback function for colours.*/
void switch_color(int value);

/*Callback function for top menu.*/
void popUp_menu(int value);

/*Callback function to change the pen size.*/
void brush_size(int value);

/*Callback function for the top menu. Does nothing.*/
void top_menu_func();

/*Callback function for type_m, type_m defines the drawing object.*/
void primitive_selection(int value);

/*Callback function for mouse events.
	buttons : GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON and GLUT_MIDDLE_BUTTON
	status  : GLUT_DOWN, GLUT_UP
	(x,y)   : cursor position.
*/
void mouse_clicks(int button, int state, int x, int y);

/*Callback function for bonus.*/
void bonus();

/*Main procedure sets up the program and window environment.*/
int main(int argc, char **argv)
{
	int size_menu;

	/*Initialize mode and opens a window in the upper left corner of screen.*/
	glutInit(&argc,argv);							/*Establish a connection to the window system.*/
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);	/*Set display mode.*/

	glutCreateWindow("DrawingProg");				/*Create the window.*/

	/*Associate callback function with events.*/
	glutInitWindowPosition(0, 0);		/*Specify window position.*/
	glutInitWindowSize(width, height);	/*window size.*/
	init_function();
	glutDisplayFunc(window_display);			/*diplay event.*/
	glutReshapeFunc(window_reshape);		/*reshape event.*/
	glutKeyboardFunc(keyboard_events);		/*keyboard event.*/
	glutMouseFunc(mouse_clicks);			/*mouse click event.*/
	glutMotionFunc(motion_mouse);			/*mouse motion event.*/

	color_m = glutCreateMenu(switch_color); /*Creates the color menu.*/
		glutAddMenuEntry("white", WHITE);
		glutAddMenuEntry("red", RED);
		glutAddMenuEntry("green", GREEN);
		glutAddMenuEntry("blue", BLUE);

	file_m = glutCreateMenu(popUp_menu);/*Creates another menu, file-menu.*/
		glutAddMenuEntry("save", SV_IMG);
		glutAddMenuEntry("load", LD_IMG);
		glutAddMenuEntry("blend", BLND_IMG);
		glutAddMenuEntry("clear", CLR_WIN);
		glutAddMenuEntry("quit", QUIT_PRG);
		glutAddMenuEntry("bonus", BONUS);

	type_m = glutCreateMenu(primitive_selection); /*Creates draw-type menu.*/
		glutAddMenuEntry("point", POINT);
		glutAddMenuEntry("line", LINE);
		glutAddMenuEntry("polygon", POLYGON);
		glutAddMenuEntry("curve", CURVE);
		glutAddMenuEntry("circle", CIRCLE);

	size_menu = glutCreateMenu(brush_size);/*Creates a menu for changing the pen size.*/
		glutAddMenuEntry("bigger", 1);
		glutAddMenuEntry("smaller", 2);

	top_m = glutCreateMenu(top_menu_func);/*Creates the top menu.*/
		glutAddSubMenu("colors", color_m);
		glutAddSubMenu("type", type_m);
		glutAddSubMenu("size", size_menu);
		glutAddSubMenu("file", file_m);
		glutAttachMenu(GLUT_RIGHT_BUTTON);

	/*Check availability for overlay.*/
		if (glutLayerGet(GLUT_OVERLAY_POSSIBLE))
		{
			fprintf(stderr,"Overlay is available\n");
		}
		else
		{
			fprintf(stderr, "Overlay is not available, may encounter problems for the menu.\n");
		}

	/*Enter the arbiter, infinite loop.*/
	glutMainLoop();

	return 0;
}

void polygon_drawing()
{
	int i;
	
	glBegin(GL_POLYGON);
	for (i = 0; i < side; i++)
		glVertex2f(vertex[i][0], height - vertex[i][1]);
	glEnd();
	glFlush();
	side = 0;												/*set side = 0 for next polygon*/
}

void circle_drawing()
{
	static GLUquadricObj* mycircle = NULL;

	if (mycircle == NULL) 
	{
		mycircle = gluNewQuadric();
		gluQuadricDrawStyle(mycircle, GLU_FILL);
	}
	glPushMatrix();
	glTranslatef(pos_x, height - pos_y, 0.0);
	gluDisk(mycircle, 0.0, 10.0, 16, 3);			/*gludisk(GLUquadricObj*, innerRadius, outerRadius, 16-sidepolygon)*/
	glPopMatrix();

}

void window_display()
{
	/*Set clear colour to black and clear window*/
	//glClearColor(0.0, 0.0, 0.0, 0.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	glFlush();

	fprintf(stderr, "display() is called\n");
}

void window_reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/*Record the dimension of the window*/
	width = w; height = h;

	glOrtho(0.0, w, 0.0, h, -1.0, 1.0);

	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay(); /*triggers a display event*/
	fprintf(stderr, "my_reshape() is called\n");
}

void keyboard_events(unsigned char key, int x, int y)
{
	fprintf(stderr, "%c key is pressed, cursor is at <%d,%d>\n", key, x, y);
	if (key == 'Q' || key == 'q') exit(0);
}

void motion_mouse(int x, int y)
{
	if (obj_type != CURVE) return;
	if (first == 0)
	{
		first = 1;
		pos_x = x; pos_y = y;
	}
	else
	{
		glBegin(GL_LINES);
		glVertex3f(pos_x, height - pos_y, 0.0);
		glVertex3f(x, height - y, 0.0);
		glEnd();

		pos_x = x; pos_y = y;
	}
	glFinish();
	fprintf(stderr, "cursor has moved to  <%d,%d>\n", x, y);
}

void mouse_clicks(int button, int state, int x, int y)
{
	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
	
	switch (obj_type)
	{
	case POINT:
		glPointSize(pnt_size);
		glBegin(GL_POINTS);
			glVertex2f(x, height - y);
		glEnd();
		break;

	case LINE:
		if (first == 0)
		{
			first = 1;
			pos_x = x, pos_y = y;
			glPointSize(pnt_size);
			glBegin(GL_POINTS);				/*Draw the 1st point.*/
			glVertex3f(x, height - y, 0);
			glEnd();
		}
		else
		{
			first = 0;
			glLineWidth(pnt_size);				/*Define line width.*/
			glBegin(GL_LINES);					/*Draw the line.*/
			glVertex2f(pos_x, height - pos_y);
			glVertex2f(x, height - y);
			glEnd();
		}
		break;

	case POLYGON:
		if (side == 0)
		{
			vertex[side][0] = x; vertex[side][1] = y;
			side++;
		}
		else
		{
			if (fabs(vertex[side - 1][0] - x) + fabs(vertex[side - 1][1] - y) < 2)
				polygon_drawing();
			else
			{
				glBegin(GL_LINES);
				glVertex2f(vertex[side - 1][0], height - vertex[side - 1][1]);
				glVertex2f(x, height - y);
				glEnd();
				vertex[side][0] = x;
				vertex[side][1] = y;
				side++;
			}
		}
		break;

	case CIRCLE:
		pos_x = x; pos_y = y;
		circle_drawing();
		break;

	default: break;

		glFinish();
	}

	fprintf(stderr, "mouse is pressed\n");
}

void init_window()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (double)width, 0.0, (double)height);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();

}

void init_function()
{
	glReadBuffer(GL_FRONT);
	glDrawBuffer(GL_FRONT);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void switch_color(int value)
{
	switch (value) 
	{
	case WHITE:
		myColor[0] = myColor[1] = myColor[2] = 1.0;
		break;

	case RED:
		myColor[0] = 1.0;
		myColor[1] = myColor[2] = 0.0;
		break;

	case GREEN:
		myColor[1] = 1.0;
		myColor[0] = myColor[2] = 0.0;
		break;

	case BLUE:
		myColor[2] = 1.0;
		myColor[0] = myColor[1] = 0.0;
		break;

	default: break;
	}
	glColor3f(myColor[0], myColor[1], myColor[2]);
}

void popUp_menu(int value)
{
	int i, j;

	if (value == QUIT_PRG) exit(0);
	else if (value == CLR_WIN) init_window();
	else if (value == SV_IMG)
	{
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
		for (i = 0; i < width; i++)	/*Assign 0 opacity to black pixels*/
			for (j = 0; j < height; j++)
				if (image[i * width + j][0] == 0 && image[i * width + j][1] == 0 && image[i * width + j][2] == 0)
					image[i * width + j][3] = 0;
				else image[i * width + j][3] = 127; /*Other pixles have A =127.*/
	}
	else if (value == LD_IMG)
	{
		glRasterPos2i(0, 0);
		glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}
	else if (value == BLND_IMG)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glRasterPos2i(0, 0);
		glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glDisable(GL_BLEND);
	}
	else if (value == BONUS) bonus();
	glFlush();
}

void brush_size(int value)
{
	if (value == 1)
	{
		if (pnt_size < 10.0) pnt_size += 1.0;
	}
	else
	{
		if (pnt_size > 1.0) pnt_size -= 1.0;
	}		
}

void top_menu_func()
{

}

void primitive_selection(int value)
{
	obj_type = value;
	if (value == LINE || value == CURVE) first = 0;
	else if (value == POLYGON) side = 0;
}

void bonus()
{
	init_window();			/*clear the window.*/

	glBegin(GL_LINES);		/*draws a 3*3 grid.*/
	glVertex2f(100, 20);
	glVertex2f(100, 290);

	glVertex2f(190, 20);
	glVertex2f(190, 290);

	glVertex2f(10, 100);
	glVertex2f(280, 100);

	glVertex2f(10, 200);
	glVertex2f(280, 200);
	glEnd();
}