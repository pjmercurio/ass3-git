#ifndef BEZIER_MAIN_H
#define BEZIER_MAIN_H

#include "include.h"
#include "bezier-patch.h"
#include <sstream>

typedef enum {
    ROTATION,
    TRANSLATION,
    SCALE
} TransformationType;

typedef enum {
    WIREFRAME,
    FILL
} DrawingType;

typedef enum {
	FLAT,
	SMOOTH
} ShadingType;

class BezierMain {
    public:
        // constructors
        BezierMain();

        void parsePatchfile(char* filename);
        void draw();

        void transform(TransformationType a, Vector3f axis, float degrees);
        void transform(TransformationType a, Vector3f amount);

        Vector3f get_normal();
        Vector3f get_right();

        void set_fill_mode();
        void set_wireframe_mode();
		void toggle_shading_mode();
        void toggle_draw_mode();

        void apply_uniform_subdivision(float step_size);
        void apply_adaptive_subdivision(float error);

    private:
        DrawingType draw_t;
		ShadingType shading_t;
		
        vector<BezierPatch*> patches;
        Transform<float,3,Affine> t;
        Vector3f normal, right;
};

#endif // BEZIER_MAIN_H
