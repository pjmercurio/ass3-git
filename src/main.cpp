#include "include.h"
#include "bezier-main.h"

class Viewport;

class Viewport {
public:
    int w, h; // width and height
};

BezierMain mainBez;
float sub_param = 0.7;
float asub_param = 0.1;
Viewport	viewport;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// clear the color buffer

    glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
    glLoadIdentity();				        // zero out transformations

    mainBez.draw();

    glFlush();
    glutSwapBuffers();					// swap buffers - double buffer set earlier
}

void reshape(int w, int h) {
    viewport.w = w;
    viewport.h = h;
    
    glViewport (0,0,viewport.w,viewport.h);
    display();
}

void handleInput(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:    // esc
        exit(0);
        break;
    // spacebar closes window
    case ' ':
        exit(0);
        break;
    case '-':
        mainBez.transform(SCALE, Vector3f(0.8,0.8,0.8));
        display();
        break;
	case '+':
    case '=':
        mainBez.transform(SCALE, Vector3f(1.2,1.2,1.2));
        display();
        break;
    case 'u':
        sub_param -= 0.1;
        mainBez.apply_uniform_subdivision(sub_param);
        display();
        break;
    case 'a':
        asub_param -= 0.01;
        mainBez.apply_adaptive_subdivision(asub_param);
        display();
        break;
    case 'w':
        mainBez.toggle_draw_mode();
        display();
        break;
	case 's':
		mainBez.toggle_shading_mode();
		display();
		break;
    }
}

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
} ArrowKeyState;

ArrowKeyState arrow_state;

void handleSpecialKeypress(int key, int x, int y) {
    int mod = glutGetModifiers();
    switch (key) {
        case GLUT_KEY_LEFT:
            arrow_state = LEFT;
            // handle left key
            if (mod == GLUT_ACTIVE_SHIFT) {
                // translate it
                mainBez.transform(TRANSLATION, Vector3f(-0.05,0,0));
            } else {
                // rotate it around the normal
                mainBez.transform(ROTATION, mainBez.get_normal(), 8);
            }
            break;

        case GLUT_KEY_RIGHT:
            arrow_state = RIGHT;
            // handle right key
            if (mod == GLUT_ACTIVE_SHIFT) {
                // translate it
                mainBez.transform(TRANSLATION, Vector3f(0.05,0,0));
            } else {
                // rotate it around the normal
                mainBez.transform(ROTATION, mainBez.get_normal(), -8);
            }
            break;

        case GLUT_KEY_UP:
            arrow_state = UP;
            // handle up key
            if (mod == GLUT_ACTIVE_SHIFT) {
                // translate it
                mainBez.transform(TRANSLATION, Vector3f(0,0.05,0));
            } else {
                // rotate it around the right vector
                mainBez.transform(ROTATION, mainBez.get_right(), -8);
            }
            break;

        case GLUT_KEY_DOWN:
            arrow_state = DOWN;
            // handle down key
            if (mod == GLUT_ACTIVE_SHIFT) {
                // translate it
                mainBez.transform(TRANSLATION, Vector3f(0,-0.05,0));
            } else {
                // rotate it around the right vector
                mainBez.transform(ROTATION, mainBez.get_right(), 8);
            }
            break;
    }
    display();
}

void handleSpecialKeyReleased(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
            arrow_state = NONE;
            break;
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        mainBez.parsePatchfile(argv[1]);
        float subdivision_param = atof(argv[2]);
        String type = (argc > 3) ? argv[3] : "-u";
        if (type == "-u") {
            mainBez.apply_uniform_subdivision(subdivision_param);
        } else if (type == "-a") {
            mainBez.apply_adaptive_subdivision(subdivision_param);
        } else {
            mainBez.apply_uniform_subdivision(sub_param);
        }
    } else {
        cout << "No filename input." << endl;
    }

    glutInit(&argc, argv);

    //This tells glut to use a double-buffered window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    viewport.w = 500;
    viewport.h = 500;
    glutInitWindowSize(viewport.w, viewport.h);
    glutInitWindowPosition(0,0);
    glutCreateWindow(argv[0]);

    glutDisplayFunc(display);				// function to run when its time to draw something
    glutReshapeFunc(reshape);				// function to run when the window gets resized

    // set handleInput() function to take keyboard events
    glutKeyboardFunc(handleInput);
    glutSpecialFunc(handleSpecialKeypress);
    glutSpecialUpFunc(handleSpecialKeyReleased);

	// shading stuff
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	// random light behind camera/eye
	GLfloat diffuse0[]={1.0, 1.0, 1.0, 1.0};
	GLfloat ambient0[]={1.0, 1.0, 1.0, 1.0};
	GLfloat specular0[]={1.0, 1.0, 1.0, 1.0};
	GLfloat light0_pos[]={0.0, 0.0, 5.0, 1.0};

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

    glutMainLoop();

    return 0;
}
