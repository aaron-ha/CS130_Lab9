#ifndef __arcball__
#define __arcball__

#include "vec.h"
#include "mat.h"

struct gl_arcball
{
    double sphere_radius;
    vec2 center,mouse_down;
    mat4 rotation_matrix;

    gl_arcball();
    void update(const vec2 &mouse_now);
    mat4 rotation() const {return rotation_matrix;}
    void begin_drag(const vec2 &mouse_now);
    void end_drag();
    vec3 mouse_on_sphere(const vec2 &mouse,const vec2 &ballCenter,double ballRadius) const;
};

#endif
