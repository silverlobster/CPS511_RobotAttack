#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <GL/glew.h>
//#include <GL/freeglut.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include "surfaceModeller.h"
#include "subdivcurve.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
  
#define GLEW_STATIC

GLdouble worldLeft = -12;
GLdouble worldRight = 12;
GLdouble worldBottom = -9;
GLdouble worldTop = 9;
GLdouble worldCenterX = 0.0;
GLdouble worldCenterY = 0.0;
GLdouble wvLeft = -12;
GLdouble wvRight = 12;
GLdouble wvBottom = -9;
GLdouble wvTop = 9;

//draw variables
double cannonAngle = 0;
double cannonElevation = 0;
double bodyAngle = 0.0;
double legWidth = 0.8;
double legAngle = 45;
double shinAngle = 0;

//robot variables
double robotZ = 15;
double bulletX = 0;
double bulletY = 0; //2  
double bulletZ = 0; //-5
bool bulletMoving = false;

double bx;
double by;
double bz;
double turnSpeed = 185;

GLUquadric* quad;

//VBO variables
double initializeVBO = 1;
std::vector<double> verticesArray;
std::vector<double> normalsArray;
std::vector<unsigned int> quadsArray;
double vnormalSize;
double quadSize;
unsigned int positionId;
unsigned int quadId;
unsigned int normalId;

//file variables
std::ofstream fp;
std::string mystring;
std::ifstream fileRead;
int buffer[] = { 1,2,3 };
double line;

GLint glutWindowWidth = 800;
GLint glutWindowHeight = 600;
GLint viewportWidth = glutWindowWidth;
GLint viewportHeight = glutWindowHeight;

// screen window identifiers
int window2D, window3D;

int window3DSizeX = 800, window3DSizeY = 600;
GLdouble aspect = (GLdouble)window3DSizeX / window3DSizeY;

int main(int argc, char* argv[])
{
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(glutWindowWidth, glutWindowHeight);
	glutInitWindowPosition(50, 100);

	// The 3D Window
	window3D = glutCreateWindow("Surface of Revolution");
	glutPositionWindow(900, 100);
	glutDisplayFunc(display3D);
	glutIdleFunc(animateRobot);
	glutReshapeFunc(reshape3D);
	glutMouseFunc(mouseButtonHandler3D);
	//glutMouseWheelFunc(mouseScrollWheelHandler3D);
	glutMotionFunc(mouseMotionHandler3D);
	glutPassiveMotionFunc(mousePassiveMotionHandler3D);
	glutKeyboardFunc(keyboardHandler3D);
	glutSpecialFunc(keyboardArrowsHandler3D);
	// Initialize the 3D system
	init3DSurfaceWindow();

	glewInit();

	// Annnd... ACTION!!
	glutMainLoop();

	return 0;
}

/************************************************************************************
 *
 *
 * 3D Window and Surface of Revolution Code
 *
 * Fill in the code in the empty functions
 ************************************************************************************/
 // Ground Mesh material
GLfloat groundMat_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat groundMat_specular[] = { 0.01, 0.01, 0.01, 1.0 };
GLfloat groundMat_diffuse[] = { 0.4, 0.4, 0.7, 1.0 };
GLfloat groundMat_shininess[] = { 1.0 };

GLfloat light_position0[] = { 4.0, 8.0, 8.0, 1.0 };
GLfloat light_diffuse0[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat light_position1[] = { -4.0, 8.0, 8.0, 1.0 };
GLfloat light_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

//
// Surface of Revolution consists of vertices and quads
//
// Set up lighting/shading and material properties for surface of revolution
GLfloat quadMat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat quadMat_specular[] = { 0.45, 0.55, 0.45, 1.0 };
GLfloat quadMat_diffuse[] = { 0.1, 0.35, 0.1, 1.0 };
GLfloat quadMat_shininess[] = { 10.0 };


// Quads and Vertices of the surface of revolution

// Quads

// Vertices 
#define NUMBEROFSIDES 16 // You may want to lower this to 4 or 5 when debugging

GLdouble fov = 60.0;

int lastMouseX;
int lastMouseY;
int currentButton;

boolean drawAsLines = false;
boolean drawAsPoints = false;
boolean drawNormals = false;

GLdouble eyeX = 0.0, eyeY = 0.0, eyeZ = 0.5;
//GLdouble eyeX = 0.0, eyeY = 0.0, eyeZ = 0.0;
GLdouble radius = eyeZ;
GLdouble zNear = 0.1, zFar = 40.0;

//camera variables
double cameraTarget = 0;
double cameraElevation = 0;
double fieldOfView = 80;

bool shooting = false;
Bullet bullets[8];

void init3DSurfaceWindow()
{
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, model_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, model_ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear

	quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);

	glViewport(0, 0, (GLsizei)window3DSizeX, (GLsizei)window3DSizeY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cameraTarget, cameraElevation, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void reshape3D(int w, int h)
{
	glutWindowWidth = (GLsizei)w;
	glutWindowHeight = (GLsizei)h;
	glViewport(0, 0, glutWindowWidth, glutWindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void display3D() 
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, aspect, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// Set up the Viewing Transformation (V matrix)	
	//gluLookAt(0, 0, 0.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	gluLookAt(eyeX, eyeY, eyeZ, 0, 0.0, 0, 0.0, 1.0, 0.0);
	//glTranslatef(0, -1, 0);
	//glTranslatef(0, 0, -5);
	//glRotatef(-eyeX, 0, 1, 0);

	glPushMatrix();
	drawGround();
	glPopMatrix();

	// Build and Draw Surface of Revolution (Quad Mesh)
	drawVBO();

	// Draw quad mesh
	glPushMatrix();

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, quadMat_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, quadMat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, quadMat_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, quadMat_shininess);

		glPushMatrix();
		drawQuads();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, 2, -robotZ);
		drawRobot();
		glPopMatrix();

		glPushMatrix();
		for (int i = 0; i < 8; i++) {
				drawBullet(&bullets[i]);
		}
		//glTranslatef(bulletX, bulletY, bulletZ);
		//drawBullet();
		glPopMatrix();
		   
	glPopMatrix();  
	glutSwapBuffers();
}

void animateRobot(void) {
	robotZ -= 0.005;
	glutPostRedisplay();
}

void animateBullet(void) {
	if (shooting) {
		for (int i = 0; i < 8; i++) {
			if (bullets[i].active == 0) {
				bullets[i].active = true;
				bullets[i].dx = bx * .25;
				bullets[i].dy = by * .25;
				bullets[i].dz = bz * .25;
				bullets[i].x = 0;
				bullets[i].y = 0;
				bullets[i].z = 1;
				break;
			}
		}
		shooting = false;
	};

	for (int i = 0; i < 8; i++) {

		if (bullets[i].active == 1) {
			bullets[i].x -= bullets[i].dx;
			bullets[i].y -= bullets[i].dy;
			bullets[i].z -= bullets[i].dz;
		}
	};

	//bulletX -= bx * .25;
	//bulletY -= by * .25;
	//bulletZ -= bz * .25;
	//bulletZ -= 0.25;
	glutPostRedisplay();
}

void drawBullet(Bullet *b) {
	glPushMatrix();
	glTranslatef(b->x, b->y, b->z);
	//glTranslatef(bulletX, bulletY, bulletZ);
	//glRotatef(-eyeY * turnSpeed, 1, 0, 0);
	//glRotatef(eyeX * turnSpeed, 0, 1, 0);
	glScalef(0.5, 0.5, 0.5);
	glutSolidCube(0.5);
	glPopMatrix();
}

void drawRobot(void) {
	//glRotatef(spin, 0.0, 1.0, 0.0);
	drawBody();
	drawHip();
	drawLegs();
}

void drawBody(void) {
	//glPushMatrix();

	glRotatef(bodyAngle, 0, 1, 0);

	//head
	glPushMatrix();
	glTranslatef(0.0, 2.0, 0.0);
	glScalef(1.0, 1.0, 1.0);
	glutSolidCube(2.0);
	glPopMatrix();

	//body
	glPushMatrix();
	glTranslatef(0.0, 1.5, 0.0);
	glScalef(0.5, 2.0, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	//cannon
	glPushMatrix();
	glTranslatef(0.0, 1.3, 0.0);
	glRotatef(cannonAngle, 0.0, 0.0, 1.0);
	glScalef(1.0, 1.0, 3.0);
	gluCylinder(quad, 0.2, 0.2, 1.0, 15, 15);
	glPopMatrix();

	//cannon barrel
	glPushMatrix();
	glTranslatef(0.0, 1.3, 3.0);
	glRotatef(cannonAngle, 0.0, 0.0, 1.0);
	glScalef(1.0, 1.0, 3.0);
	glutSolidCube(0.15);
	glPopMatrix();

	//glPopMatrix();
}

void drawHip(void) {
	//glPushMatrix();

		//bend legs backwards
		glTranslatef(0.0, 0.5, 0.0);
		glRotatef(legAngle, 1.0, 0.0, 0.0);
		glTranslatef(0.0, -0.5, 0.0);

		//hip
		glPushMatrix();
		glTranslatef(0.0, 0.5, 0.0);
		glScalef(0.5, 0.5, 0.5);
		glutSolidSphere(1.5, 10, 10);
		glPopMatrix();

	//glPopMatrix();
}

void drawLegs(void) {
	glPushMatrix();

		glTranslatef(-legWidth, -2.0, 0.0);
		glRotatef(shinAngle, 1, 0, 0);
		glTranslatef(legWidth, 2.0, 0.0);

		//left leg joint
		glPushMatrix();
		glTranslatef(-legWidth, -2.0, 0.0);
		glScalef(0.5, 0.5, 0.5);
		glutSolidSphere(0.6, 10, 10);
		glPopMatrix();

		//left calve
		glPushMatrix();
		glTranslatef(-legWidth, -0.5, 0.0);
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(1.0);
		glPopMatrix();

		glRotatef(-90, 1, 0, 0);

		//left shin
		glPushMatrix();
		glTranslatef(-legWidth, -1.4, -2.1);
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(1.0);
		glPopMatrix();

		//left foot 
		glPushMatrix();
		glTranslatef(-legWidth, -2.9, -2.1);
		glScalef(0.5, 0.5, 0.5);
		glutSolidSphere(0.6, 10, 10);
		glPopMatrix();

	glPopMatrix();

	glPushMatrix();

		glTranslatef(-legWidth, 0.5, 0.0);
		glRotatef(shinAngle, 1.0, 0.0, 0.0);
		glTranslatef(legWidth, -0.5, 0.0);

		//right leg joint
		glPushMatrix();
		glTranslatef(legWidth, -2.0, 0.0);
		glScalef(0.5, 0.5, 0.5);
		glutSolidSphere(0.6, 10, 10);
		glPopMatrix();

		//right calve
		glPushMatrix();
		glTranslatef(legWidth, -0.6, 0.0);
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(1.0);
		glPopMatrix();

		glRotatef(-90, 1, 0, 0);

		//right shin
		glPushMatrix();
		glTranslatef(legWidth, -1.4, -2.1);
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(1.0);
		glPopMatrix();

		//right foot
		glPushMatrix();
		glTranslatef(legWidth, -2.9, -2.1);
		glScalef(0.5, 0.5, 0.5);
		glutSolidSphere(0.6, 10, 10);
		glPopMatrix();

	glPopMatrix();
}

void drawVBO() {
	if (initializeVBO == 1) {
		glGenBuffers(1, &positionId);
		glGenBuffers(1, &quadId);
		glGenBuffers(1, &normalId);

		std::ifstream infile("quadmesh.txt");

		int vindex = 0;
		double a;
		bool isNormals = false;
		bool isQuads = false;
		std::string line;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);

			iss >> a;
			if (a == 12345678) {
				isNormals = true;
				isQuads = false;
				continue;
			}
			if (a == 87654321) {
				isQuads = true;
				isNormals = false;
				continue;
			}
			if (isNormals == false && isQuads == false) {
				verticesArray.push_back(a);
			}
			else if (isNormals) {
				normalsArray.push_back(a);
			}
			else if (isQuads) {
				quadsArray.push_back((int)a);
			}
		}

		infile.close();

		initializeVBO = 0;
	}

	glBindBuffer(GL_ARRAY_BUFFER, positionId);
	glBufferData(GL_ARRAY_BUFFER, verticesArray.size() * sizeof(double), &verticesArray[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normalId);
	glBufferData(GL_ARRAY_BUFFER, normalsArray.size() * sizeof(double), &normalsArray[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadsArray.size() * sizeof(unsigned int), &quadsArray[0], GL_STATIC_DRAW);
}

void drawGround()
{
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, groundMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, groundMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, groundMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, groundMat_shininess);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-44.0f, -2.0f, -44.0f);
	glVertex3f(-44.0f, -2.0f, 44.0f);
	glVertex3f(44.0f, -2.0f, 44.0f);
	glVertex3f(44.0f, -2.0f, -44.0f);
	glEnd();
	glPopMatrix();
}

void drawQuads()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, quadMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, quadMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, quadMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, quadMat_shininess);

	// Replace this code with VAO or VBO and drawElements()

	//CREATE VBO and move qarray and varray data into allocated arrays
	//glRotatef(eyeY * 0.25, 0, 0, 1);
	//glRotatef(eyeX * 0.25, 0, 1, 0);
	//glRotatef(cameraElevation, 1, 0, 0);
	//glRotatef(cameraTarget, 0, 1, 0);
	glRotatef(eyeX * turnSpeed, 0, 1, 0);
	glRotatef(-eyeY * turnSpeed - 15, 1, 0, 0);

	glTranslatef(0, -1, 0);
	glRotatef(-90, 1, 0, 0);

	//render
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, positionId);
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, normalId);
	glNormalPointer(GL_DOUBLE, 0, 0);

	glDrawElements(GL_QUADS, quadsArray.size(), GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//glPopMatrix();
}

// Mouse and Key Handling
//
//
void mouseButtonHandler3D(int button, int state, int x, int y)
{
	currentButton = button;
	lastMouseX = x;
	lastMouseY = y;
	switch (button)
	{
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN)
		{
			// Fill in code for zooming or use scroll wheel
		}
		break;
	default:
		break;
	}
}

void mouseScrollWheelHandler3D(int button, int dir, int xMouse, int yMouse)
{
	// Fill in this code for zooming in and out
}

void mousePassiveMotionHandler3D(int x, int y) {
	int dx = -(x - lastMouseX);
	int dy = -(y - lastMouseY);

	cameraTarget += dx;
	cameraElevation += dy;
	
	if (cameraTarget < 0) {
		cameraTarget = 0;
	}
	else if (cameraTarget > 180) {
		cameraTarget = 180;
	}
	if (cameraElevation > 30) {
		cameraElevation = 30;
	}
	else if (cameraElevation < 0) {
		cameraElevation = 0;
	}

	double orbitX = cos(M_PI / 180 * cameraElevation);
	double elevateX = cos(M_PI / 180 * cameraTarget);
	double orbitY = sin(M_PI / 180 * cameraElevation);
	double elevateZ = sin(M_PI / 180 * cameraTarget);
	eyeX = -(orbitX * elevateX * radius);
	eyeY = -(orbitY * radius);
	eyeZ = orbitX * elevateZ * radius;

	lastMouseX = x;
	lastMouseY = y;
	glutPostRedisplay();
}

void mouseMotionHandler3D(int x, int y)
{
	int dx = -(x - lastMouseX);
	int dy = -(y - lastMouseY);

	cameraElevation += dy;
	cameraTarget += dx;

	if (cameraTarget < 0) {
		cameraTarget = 0;
	}
	else if (cameraTarget > 180) {
		cameraTarget = 180;
	}
	if (cameraElevation > 30) {
		cameraElevation = 30;
	}
	else if (cameraElevation < 0) {
		cameraElevation = 0;
	}

	double orbitX = cos(M_PI / 180 * cameraElevation);
	double elevateX = cos(M_PI / 180 * cameraTarget);
	double orbitY = sin(M_PI / 180 * cameraElevation);
	double elevateZ = sin(M_PI / 180 * cameraTarget);
	eyeX = -(orbitX * elevateX * radius);
	eyeY = -(orbitY * radius);
	eyeZ = orbitX * elevateZ * radius;

	lastMouseX = x;
	lastMouseY = y;
	glutPostRedisplay();
}

void keyboardHandler3D(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
	case 'Q':
	case 27:
		// Esc, q, or Q key = Quit 

		exit(0);
		break;
	case 32:
		shooting = true;
		bx = eyeX;
		by = eyeY;
		bz = eyeZ;
		glutIdleFunc(animateBullet);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void keyboardArrowsHandler3D(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_RIGHT: //right
			cameraTarget -= 2;
			break;
		case GLUT_KEY_LEFT:  //left
			cameraTarget += 2;
			break;
		case GLUT_KEY_UP: //up
			cameraElevation += 2;
			break;
		case GLUT_KEY_DOWN: //down
			cameraElevation -= 2;
			break;
		default:
			break;
	}
	if (cameraTarget > 180) {
		cameraTarget = 180;
	}
	else if (cameraTarget < 0) {
		cameraTarget = 0;
	}

	if (cameraElevation > 10) {
		cameraElevation = 10;
	}
	else if (cameraElevation < -30) {
		cameraElevation = -30;
	}

	double orbitX = cos(M_PI / 180 * cameraElevation);
	double elevateX = cos(M_PI / 180 * cameraTarget);
	double orbitY = sin(M_PI / 180 * cameraElevation);
	double elevateZ = sin(M_PI / 180 * cameraTarget);
	eyeX = -(orbitX * elevateX * radius);
	eyeY = -(orbitY * radius);
	eyeZ = orbitX * elevateZ * radius;

	glutPostRedisplay();
}
