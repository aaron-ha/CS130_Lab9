#include "gl_arcball.h"
#include <iostream>

gl_arcball::gl_arcball()
    :sphere_radius(1)
{
    rotation_matrix.make_id();
}

void gl_arcball::begin_drag(const vec2 &mouse_now)
{
    mouse_down=mouse_now;
    rotation_matrix.make_id();
}

void gl_arcball::end_drag()
{
    rotation_matrix.make_id();
}

void gl_arcball::update(const vec2 &mouse_now)
{
    vec3 from=mouse_on_sphere(mouse_down,center,sphere_radius);
    vec3 to=mouse_on_sphere(mouse_now,center,sphere_radius);

    rotation_matrix=from_rotated_vector(from,to);
}

vec3 gl_arcball::mouse_on_sphere(const vec2 &mouse,const vec2 &center,double radius) const
{
    vec2 ball_mouse=(mouse-center)/radius;
    float mag=ball_mouse.magnitude_squared();
    if(mag<=1) return vec3(ball_mouse[0],ball_mouse[1],sqrt(1-mag));
    ball_mouse=ball_mouse.normalized();
    return vec3(ball_mouse[0],ball_mouse[1],0);
}
