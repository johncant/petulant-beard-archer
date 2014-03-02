#ifndef __IMAGE_VIEW_RENDERER_H__
#define __IMAGE_VIEW_RENDERER_H__

#include <boost/shared_ptr.hpp>
#include "renderer.h"
#include "../core/image.h"
#include <GL/glew.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

namespace GtkGui {
  class ImageViewRenderer : public Renderer {
    boost::shared_ptr<Core::Image> image;
    boost::shared_ptr<cv::Mat> pixels;

    // TODO - hide ugly impl members
    GLuint gl_tex_id;
    GLuint shader_program;
    GLuint vertex_shader;
    GLuint fragment_shader;
    void init_shaders();
    GLuint compile(const std::string& shader, GLenum type);
    GLuint compile(const char* shader, GLenum type);
    public:
    ImageViewRenderer(boost::shared_ptr<Core::Image> im);
    ~ImageViewRenderer();
    void draw();
    void configure(unsigned int width, unsigned int height);
    void realize();
  };
}

#endif
