#ifndef __IMAGE_VIEW_RENDERER_H__
#define __IMAGE_VIEW_RENDERER_H__

#include <boost/shared_ptr.hpp>
#include <GL/glew.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include "../renderer.h"
#include "../../../core/image.h"
#include "../../../core/transform2d.h"
#include "transforms.h"

namespace GtkGui {
  namespace Viewer {
    namespace ImageView {
      class Renderer : public GtkGui::Viewer::Renderer {

        private:
        boost::shared_ptr<Core::Image> image;
        boost::shared_ptr<cv::Mat> pixels;
        boost::shared_ptr<cv::Mat> sprite_pixels;

        // TODO - hide ugly impl members
        GLuint gl_sprite_tex_id;
        GLuint gl_tex_id;
        GLuint shader_program;
        GLuint vertex_shader;
        GLuint fragment_shader;

        double zoom;
        Core::Point2D zoom_center;

        double vp_width;
        double vp_height;
        void init_shaders();
        GLuint compile(const std::string& shader, GLenum type);
        GLuint compile(const char* shader, GLenum type);

        GtkGui::Viewer::ImageView::Transforms::Distortion
          get_distortion_transform();
        GtkGui::Viewer::ImageView::Transforms::ImageToViewport
          get_image_to_viewport_transform();

        void draw_image();
        void draw_points();
        public:
        Renderer(boost::shared_ptr<Core::Image> im);
        ~Renderer();
        void draw();
        void configure(unsigned int width, unsigned int height);
        void realize();
        void set_zoom(double zoom);
        void set_zoom_center(Core::Point2D zoom_center);
        Core::Point2D as_image_coords(Core::Point2D pt);
      };

    }
  }
}

#endif
