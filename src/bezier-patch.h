#ifndef BEZIER_PATCH_H  // check if already included
#define BEZIER_PATCH_H

#include "include.h"

typedef enum {
    UNIFORM,
    ADAPTIVE
} TessDrawType;

class BezierPatch {
    private:
        // determines what points to draw
        TessDrawType draw_t;

		// normals
		vector<vector<Point3f*> > uniform_n;
		vector<vector<Point3f*> > adaptive_n;
		
        // control points
        vector<vector<Point3f*> > p;
        // uniform subdivision
        vector<vector<Point3f*> > uniform_p;
        // adaptive subdivision
        vector<vector<Point3f*> > adaptive_p;

        Point3f* calculateUVpoint(float u, float v);
		Vector3f* calculateUVnormal(float u, float v, float uv_delta = 0.0001);
        Matrix4f getDimensionMatrixOfPoints(int spec);

        void subdivide_triangle(vector<Point3f *>verts, vector<Vector2f *>uv_verts, float err_margin);
		
    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        // subdivision methods
        void uniform_subdivision(float step_size);
        void adaptive_subdivision(float error);


        // constructors
        BezierPatch();
        BezierPatch(vector<vector<Point3f*> > a);

        void draw(Transform<float,3,Affine> T);

        vector<vector<Point3f*> > get_matrix();
        void set_matrix(vector<vector<Point3f*> > a);
};

#endif // BEZIER_PATCH_H
