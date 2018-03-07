#include "gl_texture.h"

#include <cassert>
#include <cstdio>
#include <string>
#include <iostream>

#include <png.h>

#define TEXTURE_LOAD_ERROR 0

using namespace std;

gl_image_texture::gl_image_texture()
{
}

gl_image_texture::~gl_image_texture()
{
}

int gl_image_texture::load_texture(const std::string& filename)
{
    texid = loadTexture(filename, width, height);

    if (texid == TEXTURE_LOAD_ERROR) {
        return LOAD_ERROR;
    } else {
        return SUCCESS;
    }
}

void gl_image_texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, texid);
}

GLuint loadTexture(const string &filename, int &width, int &height)
{
    //header for testing if it is a png
    png_byte header[8];

    //open file as binary
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        return TEXTURE_LOAD_ERROR;
    }

    //read the header
    if (!fread(header, 1, 8, fp)) {
        fclose(fp);
        return TEXTURE_LOAD_ERROR;
    }

    //test if png
    int is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png) {
        fclose(fp);
        return TEXTURE_LOAD_ERROR;
    }

    //create png struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
        NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return (TEXTURE_LOAD_ERROR);
    }

    //create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
        fclose(fp);
        return (TEXTURE_LOAD_ERROR);
    }

    //create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return (TEXTURE_LOAD_ERROR);
    }

    //png error stuff, not sure libpng man suggests this.
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return (TEXTURE_LOAD_ERROR);
    }

    //init png reading
    png_init_io(png_ptr, fp);

    //let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    //variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 twidth, theight;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
        NULL, NULL, NULL);

    if (bit_depth != 8) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return (TEXTURE_LOAD_ERROR);
    }

    GLenum gl_color_type;
    if (color_type == PNG_COLOR_TYPE_RGB) {
        gl_color_type = GL_RGB;
    } else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        gl_color_type = GL_RGBA;
    } else {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return (TEXTURE_LOAD_ERROR);
    }

    //update width and height based on png info
    width = twidth;
    height = theight;


    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte *image_data = new png_byte[rowbytes * height];
    if (!image_data) {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return TEXTURE_LOAD_ERROR;
    }

    //row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep *row_pointers = new png_bytep[height];
    if (!row_pointers) {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        delete[] image_data;
        fclose(fp);
        return TEXTURE_LOAD_ERROR;
    }
    // set the individual row_pointers to point at the correct offsets of image_data
    for(int i = 0; i < height; ++i)
        row_pointers[height - 1 - i] = image_data + i * rowbytes;

    //read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    //Now generate the OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D,0, gl_color_type, width, height, 0,
        gl_color_type, GL_UNSIGNED_BYTE, (GLvoid*) image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, 0);

    //clean up memory and close stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    delete[] image_data;
    delete[] row_pointers;
    fclose(fp);

    return texture;
}
