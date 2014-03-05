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
#include "../core/transform2d.h"

namespace GtkGui {
  class ImageViewRenderer : public Renderer {
    public:
    // The transformation between image 0-1 coordinates and viewport 0-1 coordinates
    class ImageToViewport : public Core::Transform2D::Transformation<Core::Transform2D::AtOrigin<Core::Transform2D::Scaling> > {
      public:
      typedef Core::Transform2D::Scaling Inverse;

      inline ImageToViewport(double img_aspect, double vp_aspect);
    };

    // TODO - Use a combination of transformations
    class OriginDistortion;
    class OriginUndistortion;

    class OriginUndistortion : public Core::Transform2D::Base {
      public:
      typedef OriginDistortion Inverse;

      double zoom, sigma;

      OriginUndistortion(double _zoom, double _sigma);
      Core::Point2D t(Core::Point2D input);
    };

    class OriginDistortion : public Core::Transform2D::Base {
      public:
      typedef OriginUndistortion Inverse;
      double zoom, sigma;

      OriginDistortion(double _zoom, double _sigma);
      Core::Point2D t(Core::Point2D input);
    };

    class DistortionBase : public Core::Transform2D::AtOrigin<OriginDistortion> {
      public:
      DistortionBase(double ox, double oy, double zoom, double sigma);
      DistortionBase(Core::Point2D pt, double zoom, double sigma);
    };

    typedef Core::Transform2D::Transformation<DistortionBase> Distortion;

    private:
    boost::shared_ptr<Core::Image> image;
    boost::shared_ptr<cv::Mat> pixels;

    // TODO - hide ugly impl members
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
    public:
    ImageViewRenderer(boost::shared_ptr<Core::Image> im);
    ~ImageViewRenderer();
    void draw();
    void configure(unsigned int width, unsigned int height);
    void realize();
    Core::Point2D image_position_from_cursor(double x, double y);
    Distortion get_distortion_transform();
    ImageToViewport get_image_to_viewport_transform();
    void set_zoom(double zoom);
    void set_zoom_center(Core::Point2D zoom_center);
  };
}

#endif
