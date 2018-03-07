#include "gl_viewer.h"

#include <cassert>
#include <iostream>

using namespace std;

gl_viewer* gl_viewer::singleton = NULL;

gl_viewer::gl_viewer()
    :use_depth_buffer(true),use_perspective(true),do_rotation(false),
    do_zoom(false),do_translation(false),field_of_view_y(50),solid(true)
{
    assert(!singleton); // ensure only one instance is created
    singleton = this;
    rotation_matrix.make_id();
}

gl_viewer::~gl_viewer()
{
    singleton = NULL;
}

void gl_viewer::init(int argc, char *argv[], int width, int height)
{
    // set up glut
    glutInit(&argc, argv);

    // create a 24-bit double-buffered window
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | (use_depth_buffer?GLUT_DEPTH:0));

    glutInitWindowSize(width, height);
    glutCreateWindow("OGL 3D Viewer");

    // called when Glut needs to display
    glutDisplayFunc(&gl_viewer::glut_display_event_wrapper);

    // called when Glut has detected a mouse click
    glutMouseFunc(&gl_viewer::glut_mouse_click_event_wrapper);

    // called when Glut has detected mouse motion
    glutMotionFunc(&gl_viewer::glut_mouse_move_event_wrapper);

    // called when Glut has key input
    glutKeyboardFunc(&gl_viewer::glut_keyboard_event_wrapper);

    // called when GLUT has nothing to do
    glutIdleFunc(&gl_viewer::glut_display_event_wrapper);

    // called when the window is resized
    glutReshapeFunc(&gl_viewer::glut_reshape_event_wrapper);

    // clear our background to black when glClear is called
    glClearColor(0.0, 0.0, 0.0, 0);

    this->width = width;
    this->height = height;

    singleton->init_event();
}

void gl_viewer::run()
{
    // pass execution to Glut. Now Glut is in control of the main loop.
    glutMainLoop();
}

void gl_viewer::glut_display_event_wrapper()
{
    // clear our color buffer
    glClear(GL_COLOR_BUFFER_BIT | (singleton->use_depth_buffer?GL_DEPTH_BUFFER_BIT:0));
    glLoadIdentity();

    // call our gl_viewers draw event and render to back buffer
    singleton->draw_event();

    // swap the back buffer with the front (user always sees front
    // buffer on display)
    glutSwapBuffers();
}

void gl_viewer::glut_mouse_click_event_wrapper(int button, int state, int x, int y)
{
    // normally (0,0) is at the top left of screen. Since this is
    // somewhat unintuitive, it has been changed to be the bottom
    // left
    singleton->mouse_click_event(button, state, x, singleton->height - y);
    singleton->mouse_last_x = x;
    singleton->mouse_last_y = y;

    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        singleton->begin_rotation(x,y);

    if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        singleton->end_rotation(x,y);

    if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
        singleton->begin_translation(x,y);

    if(button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
        singleton->end_translation(x,y);

    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        singleton->begin_zoom(x,y);

    if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
        singleton->end_zoom(x,y);
}

void gl_viewer::glut_mouse_move_event_wrapper(int x, int y)
{
    // normally (0,0) is at the top left of screen. Since this is
    // somewhat unintuitive, it has been changed to be the bottom
    // left
    singleton->mouse_move_event(x, singleton->height - y);

    if(singleton->do_rotation) singleton->apply_rotation(x,y);
    if(singleton->do_zoom) singleton->apply_zoom(x,y);
    if(singleton->do_translation) singleton->apply_translation(x,y);

    singleton->mouse_last_x = x;
    singleton->mouse_last_y = y;
}

void gl_viewer::glut_keyboard_event_wrapper(unsigned char key, int x, int y)
{
    if(key == '=') singleton->solid = !singleton->solid;

    singleton->keyboard_event(key, x, y);
}

void gl_viewer::glut_reshape_event_wrapper(int width, int height)
{
    singleton->width = width;
    singleton->height = height;

    // adjust the view frustrum and viewport
    // to reflect the new window dimensions
    glViewport(0, 0, width, height);

    if (height == 0) height = 1;

    // set up perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(singleton->use_perspective) gluPerspective(45.0f, width / (float)height, 0.1f, 100.0f);
    else glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

void gl_viewer::apply_gl_transform() const
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(field_of_view_y,(float)width/height,nearclip,farclip);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-distance);
    glMultTransposeMatrixf(&arcball.rotation_matrix(0,0));
    glMultTransposeMatrixf(&rotation_matrix(0,0));
    glTranslatef(-target_position[0],-target_position[1],-target_position[2]);
}

vec2 gl_viewer::convert_mouse_coordinates(int x, int y) const
{
    return vec2((float)x/(width-1)*2-1,1-(float)y/(height-1)*2);
}

void gl_viewer::begin_rotation(int x, int y)
{
    vec2 v=convert_mouse_coordinates(x,y);
    arcball.begin_drag(v);
    do_rotation = true;
}

void gl_viewer::apply_rotation(int x, int y)
{
    vec2 v=convert_mouse_coordinates(x,y);
    arcball.update(v);
    glutPostRedisplay();
}

void gl_viewer::end_rotation(int x, int y)
{
    rotation_matrix=arcball.rotation_matrix*rotation_matrix;
    arcball.end_drag();
    do_rotation = false;
}

void gl_viewer::begin_zoom(int x, int y)
{
    do_zoom = true;
}

void gl_viewer::apply_zoom(int x, int y)
{
    int dy = singleton->mouse_last_y - y;
    float factor=pow(1.01f,dy);
    distance*=factor;
    nearclip=0.0625f*distance;
    farclip=4*distance;
    glutPostRedisplay();
}

void gl_viewer::end_zoom(int x, int y)
{
    do_zoom = false;
}

void gl_viewer::begin_translation(int x, int y)
{
    // Find the unprojected points
    GLint viewport[4];
    GLdouble mvmatrix[16]={ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 },projmatrix[16];
    GLdouble wx,wy,wz;
    GLdouble win_x,win_y,win_z;

    glGetIntegerv(GL_VIEWPORT,viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX,mvmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX,projmatrix);
    gluProject(target_position[0],target_position[1],target_position[2],
        mvmatrix,projmatrix,viewport,&win_x,&win_y,&win_z);
    gluUnProject(win_x+1,win_y,win_z,mvmatrix,projmatrix,viewport,&wx,&wy,&wz);
    target_x_drag_vector=vec3(wx,wy,wz)-target_position;
    gluUnProject(win_x,win_y+1,win_z,mvmatrix,projmatrix,viewport,&wx,&wy,&wz);
    target_y_drag_vector=vec3(wx,wy,wz)-target_position;
    do_translation = true;
}

void gl_viewer::apply_translation(int x, int y)
{
    float dx = singleton->mouse_last_x - x;
    float dy = y - singleton->mouse_last_y;
    target_position+=dx*target_x_drag_vector+dy*target_y_drag_vector;
    glutPostRedisplay();
}

void gl_viewer::end_translation(int x, int y)
{
    do_translation = false;
}

void gl_viewer::set_camera_for_box(const vec3& lo,const vec3& hi)
{
    target_position=(lo+hi)/2;
    vec3 size=hi-lo;

    float factor = .5f/tan(.5f*field_of_view_y*(float)M_PI/180);
    distance = factor*std::min(size[0]*height/width,size[1]) + .5f*size[2];
    nearclip = 0.0625f*distance;
    farclip = 4*distance;
}


