
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
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
inline float sqr(float x) { return x*x; }

using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};

struct patch {
  float a, b, c, d,
        e, f, g, h,
        i, j, k, l,
        m, n, o, p;
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

patch *patchArray;


//****************************************************
// Parse Input File
//****************************************************
void parseInput() {
  string line;
  myfile.open(inputFileName);
  if (myfile.is_open()) {
    getline(myfile, line);
    numPatches = atoi(line.c_str());

    patchArray = new patch[numPatches];

    while (getline(myfile, line)) {
      char *pch = strtok(line.c_str(), " ");
      while (pch != NULL) {
        printf("%s\n",pch);
        pch = strtok(NULL, " ");
      }
      printf("%s\n",line.c_str());
    }

    myfile.close();
  }
  else {
    printf("Unable to open file\n");
  }   
  printf("Num Patches: %i\n",numPatches);
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


        //setPixel(i, j, R(0), R(1), R(2));

        // This is amusing, but it assumes negative color values are treated reasonably.
        // setPixel(i,j, x/radius, y/radius, z/radius );
      }


    }
    glEnd();
  }


  

void keyPressed(unsigned char key, int x, int y) {
  switch (key) {
    case ' ':
      printf("Spacebar key pressed, program quitting...\n");
      exit(1);
      break;
    case 's':
      //Toggle flat and smooth shading
      printf("'s' key pressed.\n");
      break;
    case 'w':
      //Toggle filled and wireframe mode
      printf("'w' key pressed.\n");
      break;
    default:
      break;  
  }
  if (key == ' ') {
    exit(1);
  }
}
//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {
//glutKeyboardFunc(keyPressed);
  glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();				        // make sure transformation is "zero'd"


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

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  //Set the keyboard function
  glutKeyboardFunc(keyPressed);

  initScene();							// quick function to set up scene

  glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  glutReshapeFunc(myReshape);				// function to run when the window gets resized

  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}








