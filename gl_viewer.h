#ifndef GL_VIEWER_H
#define GL_VIEWER_H

#include "gl_arcball.h"
#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

class gl_viewer
{
public:
    gl_viewer();
    virtual ~gl_viewer();

    // Polymorphic callback events. Implement in inheriting subclass.
    //
    // draw_event is called by glut when the screen needs to be
    // redrawn. In this implementation this occurs during mouse
    // and key inputs, as well as when the screen is resized,
    // overlapped by other windows, or minimized and maximized.
    //
    // keyboard_event is called when a key is pressed. The
    // character of the key as well as the x and y location of the
    // mouse cursors current location are passed into this callback.
    // The x and y value are in window coordinates.
    //
    // mouse_event is called when a button on the mouse
    // is first pressed and then once again when it is released. The
    // first parameter is the button id, the second parameter is
    // whether the button is being pushed down or released, and
    // finally x and y are once again the position of the mouse cursor
    // during the event.

    virtual void init_event() {}
    virtual void draw_event() {}
    virtual void keyboard_event(unsigned char key, int x, int y) {}
    virtual void mouse_click_event(int button, int button_state, int x, int y) {}
    virtual void mouse_move_event(int x, int y) {}

    // Call before run. Initializes Glut. Glut is a OpenGL helper
    // library designed to allow easy GL window creation on all
    // of the various operating system.

    void init(int argc, char *argv[], int width, int height);


    // Begins the main loop. At this point execution is controlled by
    // glut.
    void run();

    bool use_depth_buffer;
    bool use_perspective;

protected:
    // window width & height
    int width, height;

    bool do_rotation;
    bool do_zoom;
    bool do_translation;

    int mouse_last_x, mouse_last_y;

    // Camera parameters
    gl_arcball arcball;
    float distance;
    float nearclip,farclip;
    float field_of_view_y;
    vec3 target_position;
    mat4 rotation_matrix;
    int translation_direction;
    vec3 target_x_drag_vector;
    vec3 target_y_drag_vector;

    bool solid;

    static void glut_display_event_wrapper();
    static void glut_mouse_click_event_wrapper(int button, int state, int x, int y);
    static void glut_mouse_move_event_wrapper(int x, int y);
    static void glut_keyboard_event_wrapper(unsigned char key, int x, int y);
    static void glut_reshape_event_wrapper(int width, int height);

    void begin_rotation(int x, int y);
    void apply_rotation(int x, int y);
    void end_rotation(int x, int y);
    void begin_zoom(int x, int y);
    void apply_zoom(int x, int y);
    void end_zoom(int x, int y);
    void begin_translation(int x, int y);
    void apply_translation(int x, int y);
    void end_translation(int x, int y);

    void apply_gl_transform() const;
    vec2 convert_mouse_coordinates(int x, int y) const;
    void set_camera_for_box(const vec3& lo,const vec3& hi);

private:
    static gl_viewer* singleton;
};

#endif
