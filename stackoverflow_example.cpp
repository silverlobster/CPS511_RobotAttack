#include <GL/glut.h>
#include<iostream>
#include<math.h>
using namespace std;

void display(void);

void reshape(int, int);
void mouseHandler(int button, int state, int x, int y);
void keyBoardHandler(unsigned char c, int x, int y);
void specialKeyHandler(int key, int x, int y);
float angle = 0;
bool camDefault = true, perspectiveOrtho = true;

float iX = 0, iY = -0.8, iZ = 0, lX = 0, lY = -0.8, lZ = -1, uX = 0, uY = 1, uZ = 0;  
float camAngle = 0;
int main(int argc, char** argv)

{
    glutInit(&argc, argv);
    glutInitWindowSize(512, 512);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("FPS Camera");
    glutDisplayFunc(display);
    glutIdleFunc(display);
    //look from negative x axis towards origin. with y on top
    glutReshapeFunc(reshape);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
    glutMouseFunc(mouseHandler);
    glutKeyboardFunc(keyBoardHandler);
    glutSpecialFunc(specialKeyHandler);
    glutMainLoop();

    return 0;
}


void drawEarth() {

    glColor3f(0, 255, 0);
    glBegin(GL_QUADS);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, -1, -1);
    glEnd();
}

void drawWalls() {

    glColor3f(0, 0, 255);
    glBegin(GL_QUADS);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1, 1, -1);
    glEnd();


    glColor3f(0, 234, 255);
    glBegin(GL_QUADS);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, -1, 1);
    glEnd();


}

float unitVector(float dir, float x, float y, float z) {
    float sumSqr = pow(x, 2) + pow(y, 2) + pow(z, 2);
    return dir / (sqrt(sumSqr));
}
void display()

{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(255, 0, 0);

    glPushMatrix();
        glTranslatef(iX + 0.05, iY, iZ - 0.05);
        glRotatef(-camAngle * 57.2957795, 0, 1, 0);
        glTranslatef(0.05, 0, -0.05); // Offset to draw the object

        glutWireCone(0.005, 0.1, 20, 20);
    glPopMatrix();

    drawEarth();
    drawWalls();

    glutSwapBuffers();

}



void reshape(int width, int height)

{

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, 1, 0.001, 1000);
    gluLookAt(iX, iY, iZ, iX + lX, lY, iZ + lZ, uX, uY, uZ);
    glMatrixMode(GL_MODELVIEW);

}

void updateLookAt() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, 1, 0.001, 1000);
    gluLookAt(iX, iY, iZ, iX + lX, lY, iZ + lZ, uX, uY, uZ);
    glMatrixMode(GL_MODELVIEW);
}


void keyBoardHandler(unsigned char c, int x, int y) {
    switch (c) {
    case 'f':  // go left
        iX -= 0.01;
        lX -= 0.01;
        cout << endl << "S pressed";
        break;

    case 's':  // go right
        iX += 0.01;
        lX += 0.01;
        break;

    case 'e': // go up
        iZ += 0.01;
        lZ += 0.01;
        break;

    case 'd': // go down
        iZ -= 0.01;
        lZ -= 0.01;
        break;

    default:
        break;
    }

    updateLookAt();

}

void mouseHandler(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (camDefault) {
            glLoadIdentity();
            gluLookAt(-1, 0, 0, 0.0, 0.0, 0.0, 0.0, 1, 0);
            camDefault = false;
        }

        else {
            glLoadIdentity();
            gluLookAt(0, 0, 0, 0.0, 0.0, 0.0, 0.0, 1, 0);
            camDefault = true;
        }
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (perspectiveOrtho) {
            gluPerspective(90, 1, 0.00001, 1000);
            perspectiveOrtho = false;
        }

        else {

            glOrtho(-1, 1, -1, 1, -1, 100);
            perspectiveOrtho = true;

        }
        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();
        if (camDefault)
            gluLookAt(0, 0, 1, 0.0, 0.0, 0.0, 0.0, 1, 0);
        else
            gluLookAt(-1, 0, 0, 0.0, 0.0, 0.0, 0.0, 1, 0);


    }
}



void specialKeyHandler(int key, int x, int y) {

    float fraction = 0.05f;

    switch (key) {
    case GLUT_KEY_LEFT:
        camAngle -= 0.01f;
        lX = sin(camAngle);
        lZ = -cos(camAngle);
        break;
    case GLUT_KEY_RIGHT:
        camAngle += 0.01f;
        lX = sin(camAngle);
        lZ = -cos(camAngle);
        break;
    case GLUT_KEY_UP:
        iX += lX * fraction;
        iZ += lZ * fraction;
        break;
    case GLUT_KEY_DOWN:
        iX -= lX * fraction;
        iZ -= lZ * fraction;
        break;
    default:
        break;
    }


    updateLookAt();
}