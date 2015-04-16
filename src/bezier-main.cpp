#include "bezier-main.h"

#define PI 3.14159265359

BezierMain::BezierMain() {
    t = Transform<float,3,Affine>::Identity();
    normal = Vector3f(0,0,-1);
    right = Vector3f(1,0,0);
    draw_t = WIREFRAME;
	shading_t = SMOOTH;
}

void BezierMain::parsePatchfile(char *filename) {
    // code for rendering the object sized in the screen
    float max_coord = 0.0f;

    ifstream in(filename);

    string line;
    string patch;

    getline(in, line);

    istringstream sin(line);
    sin >> patch;

    int num_patches = atoi(patch.c_str());

    for(int i=0; i<num_patches; i++) {
        string value;

        // need to create rows every time we use 2d vector
        vector<vector<Vector3f*> > mat;
        for(int i=0; i<4; i++)
        {
            vector<Vector3f*> row(4);
            mat.push_back(row);
        }

        // four rows of values
        for(int j=0; j<4; j++) {
            getline(in, line);
            istringstream sin(line);
            // four values per row
            for(int k=0; k<4; k++) {
                Vector3f *p = new Vector3f;
                sin >> value;
                (*p)[0] = atof(value.c_str());
                sin >> value;
                (*p)[1] = atof(value.c_str());
                sin >> value;
                (*p)[2] = atof(value.c_str());

                // code for resizing
                float abs_x = abs((*p)[0]);
                float abs_y = abs((*p)[1]);
                if (abs_x > max_coord)
                    max_coord = abs_x;
                else if (abs_y > max_coord)
                    max_coord = abs_y;

                mat[j][k] = p;
            }
        }
        // one line used to seperate patches
        getline(in, line);

        /* used to print out the patch
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                cout << vectorString((*(mat[i][j]))) << " ";
            }
            cout << endl;
        }
        */

        BezierPatch *new_patch = new BezierPatch(mat);
        patches.push_back(new_patch);
    }

    // transform the points so they all fit in 0-1
    float scale_factor = 1.0f/(max_coord * 1.2f);
    transform(SCALE, Vector3f(scale_factor, scale_factor, scale_factor));
}

void BezierMain::apply_uniform_subdivision(float step_size) {
    for(unsigned int i=0; i<patches.size(); i++) {
        patches[i]->uniform_subdivision(step_size);
    }
    printf("num patches: %lu", patches.size());
}

void BezierMain::apply_adaptive_subdivision(float error) {
    for(unsigned int i=0; i<patches.size(); i++) {
        patches[i]->adaptive_subdivision(error);
    }
}

void BezierMain::draw() {
    if (draw_t == WIREFRAME) {
        // to go to wireframe mode
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else if (draw_t == FILL) {
        // to go back to normal mode
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }

	if(shading_t == SMOOTH) {
		glShadeModel(GL_SMOOTH);
	} else if(shading_t == FLAT) {
		glShadeModel(GL_FLAT);
	}

    for(unsigned int i=0; i<patches.size(); i++) {
        patches[i]->draw(t);
    }
}

void BezierMain::transform(TransformationType a, Vector3f axis, float degrees) {
    Transform<float,3,Affine> back(Translation<float,3>(-t.translation()));
    Transform<float,3,Affine> forward(Translation<float,3>(t.translation()));

    t = back * t;
    t = AngleAxisf((degrees*PI)/180.0f, axis) * t;
    t = forward * t;
}

void BezierMain::transform(TransformationType a, Vector3f amount) {
    Transform<float,3,Affine> back(Translation<float,3>(-t.translation()));
    Transform<float,3,Affine> forward(Translation<float,3>(t.translation()));

    if (a == TRANSLATION) {
        t = Translation<float,3>(amount) * t;
    } else if (a == SCALE) {
        t = back * t;
        t = Scaling(amount) * t;
        t = forward * t;
    }
}

Vector3f BezierMain::get_normal() {
    return (t.linear() * normal).normalized();
}

Vector3f BezierMain::get_right() {
    return (t.linear() * right).normalized();
}

void BezierMain::set_fill_mode() {
    draw_t = FILL;
}

void BezierMain::set_wireframe_mode() {
    draw_t = WIREFRAME;
}

void BezierMain::toggle_shading_mode() {
	if (shading_t == FLAT) {
		shading_t = SMOOTH;
	} else {
		shading_t = FLAT;
	}
}

void BezierMain::toggle_draw_mode() {
    if (draw_t == FILL) {
        draw_t = WIREFRAME;
    } else {
        draw_t = FILL;
    }
}
