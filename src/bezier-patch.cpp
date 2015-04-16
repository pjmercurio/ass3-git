#include "bezier-patch.h"

void BezierPatch::adaptive_subdivision(float error) {
    float step_size = (1.0f / p.size());

    // subdivide each quad of the patch
    for(unsigned int i=0; i<p.size(); i++) {
        for(unsigned int j=0; j<p[i].size(); j++) {
            Vector2f *a = new Vector2f(i*step_size, j*step_size);
            Vector2f *b = new Vector2f((i+1)*step_size, j*step_size);
            Vector2f *c = new Vector2f(i*step_size, (j+1)*step_size);
            Vector2f *d = new Vector2f((i+1)*step_size, (j+1)*step_size);

            Point3f *a1 = calculateUVpoint(i*step_size, j*step_size);
            Point3f *b1 = calculateUVpoint((i+1)*step_size, j*step_size);
            Point3f *c1 = calculateUVpoint(i*step_size, (j+1)*step_size);
            Point3f *d1 = calculateUVpoint((i+1)*step_size, (j+1)*step_size);

            // top left triangle
            vector<Point3f *>vs;
            vs.push_back(a1);
            vs.push_back(c1);
            vs.push_back(b1);

            vector<Vector2f *>uvs;
            uvs.push_back(a);
            uvs.push_back(c);
            uvs.push_back(b);

            subdivide_triangle(vs, uvs, error);

            // bottom right triangle
            vector<Point3f *>vs1;
            vs1.push_back(b1);
            vs1.push_back(c1);
            vs1.push_back(d1);

            vector<Vector2f *>uvs1;
            uvs1.push_back(b);
            uvs1.push_back(c);
            uvs1.push_back(d);

            subdivide_triangle(vs1, uvs1, error);
        }
    }

    draw_t = ADAPTIVE;
}

void BezierPatch::subdivide_triangle(vector<Point3f *>verts, vector<Vector2f *>uv_verts, float err_margin) {
    if (verts.size() != 3 || uv_verts.size() != 3) {
        cout << "ERR: vects.size() or uv_verts.size() != 3" << endl;
        return;
    }

    // booleans for the three edge point tests
    bool a1, a2, a3;
    // count of how many pass
    int count = 0;

    // midpoint between uv_point0 and uv_point1
    Vector2f *uv1 = new Vector2f(((*uv_verts[0])[0] + (*uv_verts[1])[0])/2, ((*uv_verts[0])[1] + (*uv_verts[1])[1])/2);
    Point3f *u1 = calculateUVpoint((*uv1)[0], (*uv1)[1]);
    // midpoint between vert0 and vert1
    Point3f v1_ = (*verts[0] + *verts[1])/2.0f;
    Point3f *v1 = new Point3f(v1_[0], v1_[1], v1_[2]);
    // if the magnitude is less than the margin of error, it passes
    if((*v1 - *u1).norm() < err_margin) {
        a1 = true;
        count++;
    } else {
        a1 = false;
    }

    // midpoint between uv_point1 and uv_point2
    Vector2f *uv2 = new Vector2f(((*uv_verts[1])[0] + (*uv_verts[2])[0])/2, ((*uv_verts[1])[1] + (*uv_verts[2])[1])/2);
    Point3f *u2 = calculateUVpoint((*uv2)[0], (*uv2)[1]);
    // midpoint between vert1 and vert2
    Point3f v2_ = (*verts[1] + *verts[2])/2;
    Point3f *v2 = new Point3f(v2_[0], v2_[1], v2_[2]);
    // if the magnitude is less than the margin of error, it passes
    if((*v2 - *u2).norm() < err_margin) {
        a2 = true;
        count++;
    } else {
        a2 = false;
    }

    // midpoint between uv_point2 and uv_point0
    Vector2f *uv3 = new Vector2f(((*uv_verts[2])[0] + (*uv_verts[0])[0])/2, ((*uv_verts[2])[1] + (*uv_verts[0])[1])/2);
    Point3f *u3 = calculateUVpoint((*uv3)[0], (*uv3)[1]);
    // midpoint between vert2 and vert0
    Point3f v3_ = (*verts[2] + *verts[0])/2;
    Point3f *v3 = new Point3f(v3_[0], v3_[1], v3_[2]);
    // if the magnitude is less than the margin of error, it passes
    if((*v3 - *u3).norm() < err_margin) {
        a3 = true;
        count++;
    } else {
        a3 = false;
    }

    vector< vector<Point3f*> > triangle_verts;
    vector< vector<Vector2f*> > triangle_uv_verts;
    if (count == 3) {
        // DON'T SUBDIVIDE
        vector<Vector3f*> normals;
		normals.push_back(calculateUVnormal((*uv_verts[0])[0], (*uv_verts[0])[1]));
		normals.push_back(calculateUVnormal((*uv_verts[1])[0], (*uv_verts[1])[1]));
		normals.push_back(calculateUVnormal((*uv_verts[2])[0], (*uv_verts[2])[1]));

		adaptive_n.push_back(normals);
		adaptive_p.push_back(verts);
        return;
    } else if (count == 2) {
        // if a1 is the failing test
        if (!a1) {
            vector<Point3f*> vs;
            vs.push_back(verts[0]);
            vs.push_back(u1);
            vs.push_back(verts[2]);
            triangle_verts.push_back(vs);
            vector<Vector2f*> uvs;
            uvs.push_back(uv_verts[0]);
            uvs.push_back(uv1);
            uvs.push_back(uv_verts[2]);
            triangle_uv_verts.push_back(uvs);
            vector<Point3f*> vs1;
            vs1.push_back(verts[2]);
            vs1.push_back(u1);
            vs1.push_back(verts[1]);
            triangle_verts.push_back(vs1);
            vector<Vector2f*> uvs1;
            uvs1.push_back(uv_verts[2]);
            uvs1.push_back(uv1);
            uvs1.push_back(uv_verts[1]);
            triangle_uv_verts.push_back(uvs1);
        }
        else if (!a2) {
            vector<Point3f*> vs;
            vs.push_back(u2);
            vs.push_back(verts[0]);
            vs.push_back(verts[1]);
            triangle_verts.push_back(vs);
            vector<Vector2f*> uvs;
            uvs.push_back(uv2);
            uvs.push_back(uv_verts[0]);
            uvs.push_back(uv_verts[1]);
            triangle_uv_verts.push_back(uvs);
            vector<Point3f*> vs1;
            vs1.push_back(verts[0]);
            vs1.push_back(u2);
            vs1.push_back(verts[2]);
            triangle_verts.push_back(vs1);
            vector<Vector2f*> uvs1;
            uvs1.push_back(uv_verts[0]);
            uvs1.push_back(uv2);
            uvs1.push_back(uv_verts[2]);
            triangle_uv_verts.push_back(uvs1);
        }
        else if (!a3) {
            vector<Point3f*> vs;
            vs.push_back(verts[0]);
            vs.push_back(verts[1]);
            vs.push_back(u3);
            triangle_verts.push_back(vs);
            vector<Vector2f*> uvs;
            uvs.push_back(uv_verts[0]);
            uvs.push_back(uv_verts[1]);
            uvs.push_back(uv3);
            triangle_uv_verts.push_back(uvs);
            vector<Point3f*> vs1;
            vs1.push_back(verts[1]);
            vs1.push_back(verts[2]);
            vs1.push_back(u3);
            triangle_verts.push_back(vs1);
            vector<Vector2f*> uvs1;
            uvs1.push_back(uv_verts[1]);
            uvs1.push_back(uv_verts[2]);
            uvs1.push_back(uv3);
            triangle_uv_verts.push_back(uvs1);
        }
    }
    else if (count == 1) {
        if (!a1) {
            if (!a2) {
                vector<Point3f*> vs;
                vs.push_back(verts[1]);
                vs.push_back(u2);
                vs.push_back(u1);
                triangle_verts.push_back(vs);
                vector<Vector2f*> uvs;
                uvs.push_back(uv_verts[1]);
                uvs.push_back(uv2);
                uvs.push_back(uv1);
                triangle_uv_verts.push_back(uvs);

                vector<Point3f*> vs1;
                vs1.push_back(u1);
                vs1.push_back(u2);
                vs1.push_back(verts[2]);
                triangle_verts.push_back(vs1);
                vector<Vector2f*> uvs1;
                uvs1.push_back(uv1);
                uvs1.push_back(uv2);
                uvs1.push_back(uv_verts[2]);
                triangle_uv_verts.push_back(uvs1);

                vector<Point3f*> vs2;
                vs2.push_back(verts[0]);
                vs2.push_back(u1);
                vs2.push_back(verts[2]);
                triangle_verts.push_back(vs2);
                vector<Vector2f*> uvs2;
                uvs2.push_back(uv_verts[0]);
                uvs2.push_back(uv1);
                uvs2.push_back(uv_verts[2]);
                triangle_uv_verts.push_back(uvs2);
            } else if (!a3) {
                vector<Point3f*> vs;
                vs.push_back(verts[0]);
                vs.push_back(u1);
                vs.push_back(u3);
                triangle_verts.push_back(vs);
                vector<Vector2f*> uvs;
                uvs.push_back(uv_verts[0]);
                uvs.push_back(uv1);
                uvs.push_back(uv3);
                triangle_uv_verts.push_back(uvs);

                vector<Point3f*> vs1;
                vs1.push_back(u3);
                vs1.push_back(u1);
                vs1.push_back(verts[2]);
                triangle_verts.push_back(vs1);
                vector<Vector2f*> uvs1;
                uvs1.push_back(uv3);
                uvs1.push_back(uv1);
                uvs1.push_back(uv_verts[2]);
                triangle_uv_verts.push_back(uvs1);

                vector<Point3f*> vs2;
                vs2.push_back(verts[2]);
                vs2.push_back(u1);
                vs2.push_back(verts[1]);
                triangle_verts.push_back(vs2);
                vector<Vector2f*> uvs2;
                uvs2.push_back(uv_verts[2]);
                uvs2.push_back(uv1);
                uvs2.push_back(uv_verts[1]);
                triangle_uv_verts.push_back(uvs2);
            }
        } else if (!a2) {
            if (!a3) {
                vector<Point3f*> vs;
                vs.push_back(u3);
                vs.push_back(u2);
                vs.push_back(verts[2]);
                triangle_verts.push_back(vs);
                vector<Vector2f*> uvs;
                uvs.push_back(uv3);
                uvs.push_back(uv2);
                uvs.push_back(uv_verts[2]);
                triangle_uv_verts.push_back(uvs);

                vector<Point3f*> vs1;
                vs1.push_back(u2);
                vs1.push_back(u3);
                vs1.push_back(verts[1]);
                triangle_verts.push_back(vs1);
                vector<Vector2f*> uvs1;
                uvs1.push_back(uv2);
                uvs1.push_back(uv3);
                uvs1.push_back(uv_verts[1]);
                triangle_uv_verts.push_back(uvs1);

                vector<Point3f*> vs2;
                vs2.push_back(verts[1]);
                vs2.push_back(u3);
                vs2.push_back(verts[0]);
                triangle_verts.push_back(vs2);
                vector<Vector2f*> uvs2;
                uvs2.push_back(uv_verts[1]);
                uvs2.push_back(uv3);
                uvs2.push_back(uv_verts[0]);
                triangle_uv_verts.push_back(uvs2);
            }
        }
    } else if (count == 0) {
        vector<Point3f*> vs;
        vs.push_back(u3);
        vs.push_back(u2);
        vs.push_back(verts[2]);
        triangle_verts.push_back(vs);
        vector<Vector2f*> uvs;
        uvs.push_back(uv3);
        uvs.push_back(uv2);
        uvs.push_back(uv_verts[2]);
        triangle_uv_verts.push_back(uvs);

        vector<Point3f*> vs1;
        vs1.push_back(u2);
        vs1.push_back(u1);
        vs1.push_back(verts[1]);
        triangle_verts.push_back(vs1);
        vector<Vector2f*> uvs1;
        uvs1.push_back(uv2);
        uvs1.push_back(uv1);
        uvs1.push_back(uv_verts[1]);
        triangle_uv_verts.push_back(uvs1);

        vector<Point3f*> vs2;
        vs2.push_back(verts[0]);
        vs2.push_back(u1);
        vs2.push_back(u3);
        triangle_verts.push_back(vs2);
        vector<Vector2f*> uvs2;
        uvs2.push_back(uv_verts[0]);
        uvs2.push_back(uv1);
        uvs2.push_back(uv3);
        triangle_uv_verts.push_back(uvs2);

        vector<Point3f*> vs3;
        vs3.push_back(u2);
        vs3.push_back(u3);
        vs3.push_back(u1);
        triangle_verts.push_back(vs3);
        vector<Vector2f*> uvs3;
        uvs3.push_back(uv2);
        uvs3.push_back(uv3);
        uvs3.push_back(uv1);
        triangle_uv_verts.push_back(uvs3);
    }

    for(unsigned int i=0; i<triangle_verts.size(); i++) {
        subdivide_triangle(triangle_verts[i], triangle_uv_verts[i], err_margin);
    }
}

void BezierPatch::uniform_subdivision(float step_size) {
    // clear the normals and points
    uniform_n.clear();
    uniform_p.clear();

    // compute number of subdivisions for step-size
    int num_subdivisions = ((1.0f + 0.0005f) / step_size);

    for(int i=0; i<=num_subdivisions; i++) {
        // create new rows for the new 2D vector
        vector<Point3f*> row(num_subdivisions+1);
		uniform_p.push_back(row);
		uniform_n.push_back(row);

        float u = i*step_size;
        for(int j=0; j<=num_subdivisions; j++) {
            float v = j*step_size;

            uniform_p[i][j] = calculateUVpoint(u, v);
			uniform_n[i][j] = calculateUVnormal(u, v);
        }
    }
}

Point3f* BezierPatch::calculateUVpoint(float u, float v) {
    RowVector4f U(pow(u, 3), pow(u, 2), u, 1);
    RowVector4f V(pow(v, 3), pow(v, 2), v, 1);
    Vector4f trans_V = V.transpose();

    Matrix4f M;
    M <<  -1,  3, -3, 1,
           3, -6,  3, 0,
          -3,  3,  0, 0,
           1,  0,  0, 0;

    Matrix4f trans_M = M.transpose();

    // get the matrix of x's for the points only
    Matrix4f p_x = getDimensionMatrixOfPoints(0);
    float x = U * M * p_x * trans_M * trans_V;
    Matrix4f p_y = getDimensionMatrixOfPoints(1);
    float y = U * M * p_y * trans_M * trans_V;
    Matrix4f p_z = getDimensionMatrixOfPoints(2);
    float z = U * M * p_z * trans_M * trans_V;

    Point3f *p = new Point3f(x, y, z);
    return p;
}

Vector3f* BezierPatch::calculateUVnormal(float u, float v, float uv_delta) {
	Point3f* p = calculateUVpoint(u, v);
	Point3f* p_u = calculateUVpoint(u + uv_delta, v);
	Point3f* p_v = calculateUVpoint(u, v + uv_delta);

	Point3f* n = new Vector3f((*p_u - *p).cross(*p_v - *p));

	if(*n == Vector3f(0.0, 0.0, 0.0)) {
		delete n;
		n = calculateUVnormal(u + uv_delta, v + uv_delta);
	}
	return n;
}

Matrix4f BezierPatch::getDimensionMatrixOfPoints(int spec) {
    Matrix4f m;
    // pull out the value you want from the points and create a 4x4 matrix
    m << (*(p[0][0]))[spec], (*(p[0][1]))[spec], (*(p[0][2]))[spec], (*(p[0][3]))[spec],
         (*(p[1][0]))[spec], (*(p[1][1]))[spec], (*(p[1][2]))[spec], (*(p[1][3]))[spec],
         (*(p[2][0]))[spec], (*(p[2][1]))[spec], (*(p[2][2]))[spec], (*(p[2][3]))[spec],
         (*(p[3][0]))[spec], (*(p[3][1]))[spec], (*(p[3][2]))[spec], (*(p[3][3]))[spec];
    return m;
}

BezierPatch::BezierPatch() {
    for(int i=0; i<4; i++)
    {
        vector<Point3f*> row(4);
        p.push_back(row);
    }
    draw_t = UNIFORM;
}

BezierPatch::BezierPatch(vector<vector<Point3f*> > a) {
    for(int i=0; i<4; i++)
    {
        vector<Point3f*> row(4);
        p.push_back(row);
    }
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            p[i][j] = a[i][j];
        }
    }
    draw_t = UNIFORM;
}

void BezierPatch::draw(Transform<float,3,Affine> T) {
    if (draw_t == UNIFORM) {
        // draw each quad of the patch
        for(unsigned int i=0; i<uniform_p.size()-1; i++) {
            for(unsigned int j=0; j<uniform_p[i].size()-1; j++) {
				Point3f a0 = T * (*(uniform_p[i+1][j]));
                Point3f a1 = T * (*(uniform_p[i][j]));
                Point3f a2 = T * (*(uniform_p[i][j+1]));
                Point3f a3 = T * (*(uniform_p[i+1][j+1]));

				Vector3f n0 = T.linear() * (*(uniform_n[i+1][j]));
				Vector3f n1 = T.linear() * (*(uniform_n[i]  [j]));
				Vector3f n2 = T.linear() * (*(uniform_n[i]  [j+1]));
				Vector3f n3 = T.linear() * (*(uniform_n[i+1][j+1]));


                glBegin(GL_QUADS);
                    glNormal3f(n0[0], n0[1], n0[2]);
                    glVertex3f(a0[0], a0[1], a0[2]);

					glNormal3f(n1[0], n1[1], n1[2]);
					glVertex3f(a1[0], a1[1], a1[2]);

					glNormal3f(n2[0], n2[1], n2[2]);
					glVertex3f(a2[0], a2[1], a2[2]);

					glNormal3f(n3[0], n3[1], n3[2]);
					glVertex3f(a3[0], a3[1], a3[2]);
                glEnd();
            }
        }
    } else if (draw_t == ADAPTIVE) {
        for(unsigned int i=0; i<adaptive_p.size(); i++) {
            // draw each triangle
            Point3f a0 = T * (*(adaptive_p[i][0]));
            Point3f a1 = T * (*(adaptive_p[i][1]));
            Point3f a2 = T * (*(adaptive_p[i][2]));

			Vector3f n0 = T.linear() * (*(adaptive_n[i][0]));
			Vector3f n1 = T.linear() * (*(adaptive_n[i][1]));
			Vector3f n2 = T.linear() * (*(adaptive_n[i][2]));


            glBegin(GL_TRIANGLES);
				// counter-clockwise
                glNormal3f(n0[0], n0[1], n0[2]);
                glVertex3f(a0[0], a0[1], a0[2]);

				glNormal3f(n1[0], n1[1], n1[2]);
				glVertex3f(a1[0], a1[1], a1[2]);

				glNormal3f(n2[0], n2[1], n2[2]);
                glVertex3f(a2[0], a2[1], a2[2]);
            glEnd();
        }
    }
}

vector<vector<Point3f*> > BezierPatch::get_matrix() {
    return p;
}

void BezierPatch::set_matrix(vector<vector<Point3f*> > a) {
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            p[i][j] = a[i][j];
        }
    }
}

