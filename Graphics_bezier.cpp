// Bezier Surfaces Demo.

#include <stdio.h>
#include <stdlib.h>


#include <assert.h>
#include <math.h>
#include <string.h>
#include "graphics.h"
#include "vector.h"

#include <iostream>
#include "trackbal.h"
#include <vector>
#include <utility>
#include <time.h> 

#include "glut.h"




double curquat[4];
static int rotation_mouse=GL_FALSE;
static int rotation_mousex,rotation_mousey;
static double lastquat[4];
/*my stuff */
std::vector<Bezier> LINES;
std::pair <int, int> DRAG;
int last_selected_curve = -1;

Point2::Point2()
	: x(0), y(0) {
}

Point2::Point2(double xin, double yin)
	: x(xin), y(yin) {
}

Point2 Point2::operator*(const double &rhs) {
	return Point2(x * rhs, y * rhs);
}

Point2 Point2::operator+(const Point2 &rhs) {
	return Point2(x + rhs.getX(), y + rhs.getY());
}
void DrawCircle(int x1, int y1, int radius, double r, double g, double b)
{
	glBegin(GL_POLYGON);
	glColor3d(r, g, b);
	for (int i = 0; i<32; i++)
	{
		double theta = (double)i / 32.0 * 2.0 * 3.1415926;
		double x = x1 + radius * cos(theta);
		double y = y1 + radius * sin(theta);
		glVertex2f(x, y);
	}
	glEnd();
}
/*finds the interpolated point.*/


Point2 Bezier::Evaluate(float t)
{
	Point2 result;
	float n = 1 - t;
	//p= p0*(1-t)*(1-t)*(1-t) + 3*p1*(1-t)*(1-t)*t + 3*p2*(1-t)*t*t + p3*t*t*t;
	
	result.x = points[0].getX() * n*n*n + 3 * points[1].getX()*n*n*t + 3 * points[2].getX()*n*t*t + points[3].getX()*t*t*t;
	result.y = points[0].getY() * n*n*n + 3 * points[1].getY()*n*n*t + 3 * points[2].getY()*n*t*t + points[3].getY()*t*t*t;
	
	return result;
}
void DrawLine(double x1, double y1, double x2, double y2) {
	glBegin(GL_LINES);
	glVertex2d(x1, y1);
	glVertex2d(x2, y2);
	glEnd();
}
/*draws the curve by approximating it at about 20 evaluation points*/
/*
void Bezier::DrawCurve() {
	const float numPoints = 20;
	Point2 lastP = Evaluate(0 / numPoints);
	for (float i = 0; i < numPoints; i++) {
		Point2 newP = Evaluate((i + 1.0) / numPoints);

		glLineWidth(2);
		glBegin(GL_LINES);
		glColor3d(color[0], color[1], color[2]);
		glVertex2f(lastP.getX(), lastP.getY());
		glVertex2f(newP.getX(), newP.getY());
		glEnd();

		lastP = newP;
	}
}*/

void Bezier::DrawCurve() {
	int sections = 20;
	double inc = 1.0 / sections;
	for (double t = 0; t < 1; t += inc) {
		Point2 p1 = Evaluate(t);
		Point2 p2 = Evaluate(t + inc);

		glLineWidth(2);
		glBegin(GL_LINES);
		glColor3d(color[0], color[1], color[2]);
		glVertex2f(p1.getX(), p1.getY());
		glVertex2f(p2.getX(), p2.getY());
		
		glEnd();
		//DrawLine(p1.getX(), p1.getY(), p2.getX(), p2.getY());
	}
}
/* draws the 4 control points as circles.*/

void DrawCircle(int x1, int y1, int radius)
{
	glBegin(GL_POLYGON);
	for (int i = 0; i<32; i++)
	{
		double theta = (double)i / 32.0 * 2.0 * 3.1415926;
		double x = x1 + radius * cos(theta);
		double y = y1 + radius * sin(theta);
		glVertex2f(x, y);
	}
	glEnd();
}
void Bezier::DrawControlPoints() {
	/*for (int i = 0; i < 4; i++) {
		double x = points[i].getX();
		double y = points[i].getY();
		DrawCircle(x, y, radius);
	}*/
	for (int i = 0; i < 4; i++) {
		DrawCircle(points[i].getX(), points[i].getY(), radius, color[0], color[1], color[2]);
	}
}

// returns <n> if x,y is within <radius> of control point <n>.
// returns -1 if x,y is not close to any of the 4 control points.
int Bezier::IsPicked(float x, float y) {
	for (int i = 0; i < 4; i++) {
		double x2 = points[i].getX();
		double y2 = points[i].getY();
		double dist = (x2 - x)*(x2 - x) + (y2 - y)*(y2 - y);
		dist = sqrt(dist);
		if (dist < radius)
			return i;
	}
	return -1;
}

class BezierSurface
{
public:
	Point3 cp[4][4];

	BezierSurface();
	Point3 Evaluate(float s, float t);
	void DrawSurfaceWireframe();
	void DrawPoints();
};

float random_float(float low, float high)
{
	float x = (float)rand() / (float)RAND_MAX; // 0 to 1
	float scale = high-low;
	x *= scale; // 0 to scale
	x += low; // low to high;
	return x;
}

BezierSurface::BezierSurface()
{
	for(int r=0; r<4; r++)
	{
		for(int c=0; c<4; c++)
		{
			cp[r][c].p[0] = (c-1.5)/1.5;
			cp[r][c].p[1] = (r-1.5)/1.5;
			cp[r][c].p[2] = random_float(-1, 1);;
		}
	}
}

Point3 EvalCurve(float t, Point3 p0, Point3 p1, Point3 p2, Point3 p3)
{
	Point3 result;
	for(int i=0; i<3; i++)
	{
		result.p[i] = p0.p[i]*(1-t)*(1-t)*(1-t) + 3*p1.p[i]*(1-t)*(1-t)*t + 3*p2.p[i]*(1-t)*t*t + p3.p[i]*t*t*t;
	}
	return result;
}

Point3 BezierSurface::Evaluate(float s, float t)
{
	Point3 temp[4];
	for(int i=0; i<4; i++)
	{
		temp[i] = EvalCurve(s, cp[i][0], cp[i][1], cp[i][2], cp[i][3]);
	}

	Point3 result = EvalCurve(t, temp[0], temp[1], temp[2], temp[3]);
	return result;
}

void DrawSphere(float x, float y, float z, float radius)
{
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex3f(x,y,z);
	glEnd();

	/*
	glPushMatrix();
	glTranslatef(x,y,z);
	int slices = 40;
	int stacks = 40;
	glutSolidSphere(radius, slices, stacks);
	glPopMatrix();
	*/
}

void DrawAxes(float length)
{
	glLineWidth(3.0);
	glBegin(GL_LINES);

	// x axis in red
	glColor3f(1,0,0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(length, 0.0, 0.0);
	// y axis in green
	glColor3f(0,1,0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, length, 0.0);
	// z axis in blue
	glColor3f(0,0,1);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, length);

	glEnd();
	glLineWidth(1.0);
}

void BezierSurface::DrawPoints()
{
	glColor3fv(whiteMaterial);
	for(int r=0; r<4; r++)
	{
		for(int c=0; c<4; c++)
		{
			DrawSphere(cp[r][c].p[0], cp[r][c].p[1], cp[r][c].p[2], .05);
		}
	}
}

void BezierSurface::DrawSurfaceWireframe()
{
	const float steps = 20;
	const float inc = 1.0 / steps;
	float s=0,t=0;

	glColor3fv(whiteMaterial);
	glBegin(GL_LINES);

	int i,j;

	// Draw lines in s direction
	for(j=0; j<=steps; j++)
	{
		s=0;
		for(i=0; i<steps; i++)
		{
			Point3 from = Evaluate(s,t);
			Point3 to = Evaluate(s+inc,t);
			glVertex3fv((float*)&(from.p[0]));
			glVertex3fv((float*)&(to.p[0]));
			s+= inc;
		}
		t+= inc;
	}

	// Draw lines in t direction
	s=0;
	for(j=0; j<=steps; j++)
	{
		t=0;
		for(i=0; i<steps; i++)
		{
			Point3 from = Evaluate(s,t);
			Point3 to = Evaluate(s,t+inc);
			glVertex3fv((float*)&(from.p[0]));
			glVertex3fv((float*)&(to.p[0]));
			t+= inc;
		}
		s+= inc;
	}

	glEnd();
}


BezierSurface bez;

// Global Variables
// Some colors you can use, or make your own and add them
// here and in graphics.h
GLfloat redMaterial[] = {0.7f, 0.1f, 0.2f, 1.0f};
GLfloat greenMaterial[] = {0.1f, 0.7f, 0.4f, 1.0f};
GLfloat brightGreenMaterial[] = {0.1f, 0.9f, 0.1f, 1.0f};
GLfloat blueMaterial[] = {0.1f, 0.2f, 0.7f, 1.0f};
GLfloat whiteMaterial[] = {1.0f, 1.0f, 1.0f, 1.0f};

int screen_x = 700;
int screen_y = 500;

// 
// Functions that draw basic primitives
//


void DrawRectangle(int x1, int y1, int x2, int y2)
{
	glBegin(GL_QUADS);
	glVertex2f(x1,y1);
	glVertex2f(x2,y1);
	glVertex2f(x2,y2);
	glVertex2f(x1,y2);
	glEnd();
}

void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	glBegin(GL_TRIANGLES);
	glVertex2f(x1,y1);
	glVertex2f(x2,y2);
	glVertex2f(x3,y3);
	glEnd();
}

// Outputs a string of text at the specified location.
void text_output(int x, int y, char *string)
{
	void *font = GLUT_BITMAP_9_BY_15;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
	
	int len, i;
	glRasterPos2f(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++) 
	{
		glutBitmapCharacter(font, string[i]);
	}

    glDisable(GL_BLEND);
}


//
// GLUT callback functions
//

// This callback function gets called by the Glut
// system whenever it decides things need to be redrawn.
void display(void){
	/**/
	// Test lines that draw all three shapes and some text.
	// Delete these when you get your code working.
	glClear(GL_COLOR_BUFFER_BIT);

	for (unsigned  int i = 0; i < LINES.size(); i++) {
		LINES[i].DrawCurve();
		LINES[i].DrawControlPoints();
	}
	/*
	double m[4][4];
	build_rotmatrix(m, curquat);
	glMultMatrixd(m[0]);


	DrawAxes(1.0);
	bez.DrawPoints();
	bez.DrawSurfaceWireframe();
	*/
	
	glutSwapBuffers();
	glutPostRedisplay(); 
}


// This callback function gets called by the Glut
// system whenever a key is pressed.
void keyboard(unsigned char c, int x, int y)
{
  switch (c) {
  case 27: // escape character means to quit the program
	exit(0);
    break;
  case 'b':
    // do something when 'b' character is hit.
    break;
  default:
    return;
  }
  glutPostRedisplay();
}


void SetPerspectiveView(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float aspectRatio = (GLdouble) w/(GLdouble) h;
	gluPerspective( 
	/* field of view in degree */ 38.0,
	/* aspect ratio */ aspectRatio,
	/* Z near */ .9, /* Z far */ 100.0);
	gluLookAt(0,0,5,  0,0,0,  0,1,0);
	glMatrixMode(GL_MODELVIEW);
}

// This callback function gets called by the Glut
// This callback function gets called by the Glut
// system whenever the window is resized by the user.
void reshape(int w, int h)
{
	screen_x = w;
	screen_y = h;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
}


// This callback function gets called by the Glut
// system whenever any mouse button goes up or down.
/*ADDED FUNCTIONALTY*/
void mouse(int mouse_button, int state, int x, int y)
{
	y=screen_y-y-1; /* orient screen so 0,0 is bottom left, not top left */
	if (mouse_button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
		for (unsigned int i = 0; i < LINES.size(); i++) {
			int j = LINES[i].IsPicked(x, y);
			if (j != -1) {
				DRAG.first = i;
				DRAG.second = j;

				last_selected_curve = i;
			}
		}
	}
	if (mouse_button == GLUT_LEFT_BUTTON && state == GLUT_UP) 
	{
		DRAG.first = -1;
		DRAG.second = -1;
	}
	if (mouse_button == GLUT_MIDDLE_BUTTON) 
	{
	}

	glutPostRedisplay();
}


void mousemotion(int x, int y){

	if (DRAG.first != -1) {

		//invert mouse coords
		int newY = screen_y - y;

		LINES[DRAG.first].points[DRAG.second].x = x;
		LINES[DRAG.first].points[DRAG.second].y = newY;
	}

}
/* mousemotion */
/*Helpers*/
void addBezier() {
	int rad = 3;
	Bezier BL;
	srand(time(NULL));

	Point2 p0((rand() % screen_x) + 1, (rand() % screen_y) + 1);

	Point2 p1((rand() % screen_x) + 1, (rand() % screen_y) + 1);

	Point2 p2((rand() % screen_x) + 1, (rand() % screen_y) + 1);

	Point2 p3((rand() % screen_x) + 1, (rand() % screen_y) + 1);
	
	/*
	Point2 p0(300, 270);

	Point2 p1(300, 300);

	Point2 p2(400, 300);

	Point2 p3(400, 340);*/
	BL.points[0] = p0;
	BL.points[1] = p1;
	BL.points[2] = p2;
	BL.points[3] = p3;
	BL.color[0] = ((rand() % 255) + 1) / 255.0;
	BL.color[1] = ((rand() % 255) + 1) / 255.0;
	BL.color[2] = ((rand() % 255) + 1) / 255.0;
	LINES.push_back(BL);
}

// Your initialization code goes here.
void InitializeMyStuff(){
	/*trackball(curquat, (double)0.0, (double)0.0, (double)0.0, (double)0.0);*/
	addBezier();
}

void top_menu(int id) {
	if (id == 1) {
		exit(0);
	}
	else if (id == 2){
		addBezier();
	}
	glutPostRedisplay();
}

void color_menu(int id) {
	if (last_selected_curve != -1) {
		if (id == 1) {
			//purple
			LINES[last_selected_curve].color[0] = 1;
			LINES[last_selected_curve].color[1] = 0;
			LINES[last_selected_curve].color[2] = 1;
		}
		else if (id == 2) {
			//red
			LINES[last_selected_curve].color[0] = 1;
			LINES[last_selected_curve].color[1] = 0;
			LINES[last_selected_curve].color[2] = 0;
		}
		else if (id == 3) {
			//green
			LINES[last_selected_curve].color[0] = 0;
			LINES[last_selected_curve].color[1] = 1;
			LINES[last_selected_curve].color[2] = 0;
		}
		else if (id == 4) {
			//blue
			LINES[last_selected_curve].color[0] = 0;
			LINES[last_selected_curve].color[1] = 0;
			LINES[last_selected_curve].color[2] = 1;
		}
		else if (id == 5) {
			//orange
			LINES[last_selected_curve].color[0] = 1;
			LINES[last_selected_curve].color[1] = 1;
			LINES[last_selected_curve].color[2] = 0;
		}
		else if (id == 6) {
			//blue green
			LINES[last_selected_curve].color[0] = 0;
			LINES[last_selected_curve].color[1] = 1;
			LINES[last_selected_curve].color[2] = 1;
		}
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInitWindowSize(screen_x, screen_y);
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	int fullscreen = 0;
	if (fullscreen) 
	{
		glutGameModeString("800x600:32");
		glutEnterGameMode();
	} 
	else 
	{
		glutCreateWindow("Bezier Curves");
	}

	int sub_menu = glutCreateMenu(color_menu);
	glutAddMenuEntry("Purple", 1);
	glutAddMenuEntry("Red", 2);
	glutAddMenuEntry("Green", 3);
	glutAddMenuEntry("Blue", 4);
	glutAddMenuEntry("Yellow", 5);
	glutAddMenuEntry("Teal", 6);

	glutCreateMenu(top_menu);
	glutAddMenuEntry("Quit", 1);
	glutAddMenuEntry("Add Curve", 2);
	glutAddSubMenu("Set Color", sub_menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);


	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mousemotion);

	glColor3d(0, 0, 0); // forground color
	glClearColor(1.0, 1.0, 1.0, 0); // background color
	InitializeMyStuff();

	glutMainLoop();

	return 0;
}
