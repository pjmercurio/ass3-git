
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <utility>
#include <iterator>
#include <sstream>
#include "Eigen/Dense"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>

using namespace Eigen;
using namespace std;


#define PI 3.14159265  // Should be used from mathlib
#define WIDTH 600
#define HEIGHT 400
inline float sqr(float x) { return x*x; }

//****************************************************
// Some Functions
//****************************************************

void renderScene(int argc, char *argv[]);
void parseInput();
void keyPressed(unsigned char key, int x, int y);
void arrowKeyPressed(int key, int x, int y);

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};

struct curve {
  Vector3f a, b, c, d;
};

struct patch {
  Matrix4f x, y, z;
};



//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;

string inputFileName;
float subdivisionParameter;
bool uniform = true;
int numPatches;
ifstream myfile;

curve *curveArray;
patch *patchArray;



void renderAdaptive() {

}


void renderUniform() {
  // Each iteration is for one patch, 4 curves
  for (int i = 0; i < numPatches*4; i+=4) {
    // Each iteration is one step in the calculation

  }
}


//****************************************************
// Parse Input File
//****************************************************
void parseInput() {
  string line;
  myfile.open(inputFileName);

  if (myfile.is_open()) {
    getline(myfile, line);
    numPatches = atoi(line.c_str());

    curveArray = new curve[numPatches*4];

    patchArray = new patch[numPatches];

    Matrix4f X;
    Matrix4f Y;
    Matrix4f Z;

    int i = 0;
    int p = 0;
    while (getline(myfile, line)) {
      istringstream this_line(line);
      istream_iterator<float> begin(this_line), end;
      vector<float> v(begin, end);



      if (v.size() == 12) {
        X(p,0) = v[0];
        X(p,1) = v[3];
        X(p,2) = v[6];
        X(p,3) = v[9];

        Y(p,0) = v[1];
        Y(p,1) = v[4];
        Y(p,2) = v[7];
        Y(p,3) = v[10];

        Z(p,0) = v[2];
        Z(p,1) = v[5];
        Z(p,2) = v[8];
        Z(p,3) = v[11];

        i++;
        p++;

        if (p == 4) {
          patch temp = {X, Y, Z};
          patchArray[i/4 - 1] = temp;
          p = 0;
        }

        //Vector3f a(v[0],v[1],v[2]);
        //Vector3f b(v[3],v[4],v[5]);
        //Vector3f c(v[6],v[7],v[8]);
        //Vector3f d(v[9],v[10],v[11]);

        //curve temp = {a, b, c, d};

        //curveArray[i] = temp;

      }
    }

    myfile.close();
  }
  else {
    printf("Unable to open file\n");
  }   
  printf("Num Patches: %i\n",numPatches);

  // Print out all curves cleanly to test data input parsing
  for (int j = 0; j < numPatches*4; j++) {
    printf("Curve: %f %f %f %f %f %f %f %f %f %f %f %f\n",
      curveArray[j].a(0),curveArray[j].a(1),curveArray[j].a(2),
      curveArray[j].b(0),curveArray[j].b(1),curveArray[j].b(2),
      curveArray[j].c(0),curveArray[j].c(1),curveArray[j].c(2),
      curveArray[j].d(0),curveArray[j].d(1),curveArray[j].d(2));
  }

  if (uniform == true) {
    renderUniform();
  }
  else {
    renderAdaptive();
  }
}


//****************************************************
// Simple init function
//****************************************************
void initScene(){

  // Nothing to do here for this simple example.

}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}


//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

// Returns a random float between 0 and 1
float random_float()
{
  float the_number = (float)rand()/RAND_MAX;
  printf("rand: %f\n",the_number);
  return the_number;
}

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  glColor3f(r, g, b);
  glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel
  // centers 
  // Note: Need to check for gap
  // bug on inst machines.
}

//****************************************************
// Draw a filled circle.  
//****************************************************


void circle(float centerX, float centerY, float radius) {
  // Draw inner circle
  glBegin(GL_POINTS);

  // We could eliminate wasted work by only looping over the pixels
  // inside the sphere's radius.  But the example is more clear this
  // way.  In general drawing an object by loopig over the whole
  // screen is wasteful.

  int i,j;  // Pixel indices

  int minI = max(0,(int)floor(centerX-radius));
  int maxI = min(viewport.w-1,(int)ceil(centerX+radius));

  int minJ = max(0,(int)floor(centerY-radius));
  int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));


  for (i=0;i<viewport.w;i++) {
    for (j=0;j<viewport.h;j++) {
        setPixel(i, j, 0, 0, 0);
      }
    }
    glEnd();
  }
  

void keyPressed(unsigned char key, int x, int y) {
  switch (key) {
    case ' ':
      printf("Spacebar key pressed: program quitting.\n");
      exit(1);
      break;
    case 's':
      //Toggle flat and smooth shading
      printf("'s' key pressed: toggling between flat and smooth shading.\n");
      break;
    case 'w':
      //Toggle filled and wireframe mode
      printf("'w' key pressed: toggling between filled and wireframe mode.\n");
      break;
    case '+':
      printf("'+' key pressed: zooming in.\n");
      break;
    case '-':
      printf("'-' key pressed: zooming out.\n");
      break;  
    default:
      printf("'%c' key pressed, doing nothing.\n", key);
      break;  
  }
}

void arrowKeyPressed(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_UP:
      if (glutGetModifiers() == 1) {
        printf("Up arrow key pressed, translating object up.\n");
      }
      break;
    case GLUT_KEY_DOWN:
      if (glutGetModifiers() == 1) {
        printf("Down arrow key pressed, translating object down.\n");
      }
      break;
    case GLUT_KEY_LEFT:
      if (glutGetModifiers() == 1) {
        printf("Left arrow key pressed, translating object left.\n");
      }
      break;
    case GLUT_KEY_RIGHT:
      if (glutGetModifiers() == 1) {
        printf("Right arrow key pressed, translating object right.\n");
      }
      break;      
    default:
      break; 
  }
}

//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {
//glutKeyboardFunc(keyPressed);
  glClear(GL_COLOR_BUFFER_BIT);		// clear the color buffer

  // indicate we are specifying camera transformations
  glMatrixMode(GL_MODELVIEW);	

  // make sure transformation is "zero'd"		
  glLoadIdentity();				        


  // Start drawing
  circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);

  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set double buffer)
  
}



//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
  //This initializes glut
  srand(time(NULL));
  glutInit(&argc, argv);
    
  //Grab the command line arguments
  if (argc < 3) {
    printf("ERROR! Too few arguments given.\n");
  }
  else if (argc > 4) {
    printf("ERROR! Too many arguments given!\n");
  }
  else if (argc == 3) {
    inputFileName = argv[1];
    subdivisionParameter = atof(argv[2]);
    parseInput();
  }
  else if (argc == 4) {
    inputFileName = argv[1];
    subdivisionParameter = atof(argv[2]);
    if (strcmp(argv[3], "-a") == 0) {
      uniform = false;
    }
    else {
      printf("ERROR! Unknown flag '%s'.\n",argv[3]);
    }
    parseInput();
  }
  else {
    printf("UNKNOWN ERROR!\n");
  }
  renderScene(argc, argv);
  return 0;
 }

void renderScene(int argc, char *argv[]) { 

  printf("Rendering scene...\n");

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = WIDTH;
  viewport.h = HEIGHT;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  //Set the keyboard function
  glutKeyboardFunc(keyPressed);
  glutSpecialFunc(arrowKeyPressed);

  initScene();						    // quick function to set up scene

  glutDisplayFunc(myDisplay);	// function to run when its time to draw something
  glutReshapeFunc(myReshape);	// function to run when the window gets resized

  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else
}

