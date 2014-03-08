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
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind.hpp>

namespace GtkGui {
  class ImageViewRenderer : public Renderer {
    public:
    // The transformation between image 0-1 coordinates and viewport 0-1 coordinates
    class ImageToViewport :
      public Core::Transform2D::Transformation<
        Core::Transform2D::AtOrigin<
          Core::Transform2D::Scaling
        >
      >
    {

      public:
      typedef Core::Transform2D::Transformation<
        Core::Transform2D::AtOrigin<
          Core::Transform2D::Scaling
        >
      > Inverse;

      inline ImageToViewport(const double img_aspect, const double vp_aspect);
    };

    // TODO - Use a combination of transformations
    class OriginDistortionBase;

    class OriginDistortionBase : public Core::Transform2D::Base {
      public:
      typedef Core::Transform2D::Transformation<
        Core::Transform2D::BisectionUnscale<OriginDistortionBase>
      > Inverse;
      double zoom, sigma;

      OriginDistortionBase(double _zoom, double _sigma);
      Core::Point2D t(Core::Point2D input);
      template <class other>
      OriginDistortionBase(other t);
      Inverse inverse();
      private:
      inline double inverse_initial_llim(double dist);
      inline double inverse_initial_ulim(double dist);
      inline double radial_func(double dist);
    };

    class DistortionBase :
      public Core::Transform2D::AtOrigin<
        Core::Transform2D::Transformation<ImageViewRenderer::OriginDistortionBase>
      >
    {
      public:
      DistortionBase(double ox, double oy, double zoom, double sigma);
      DistortionBase(Core::Point2D pt, double zoom, double sigma);
    };

    typedef Core::Transform2D::Transformation<DistortionBase> Distortion;

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

    Distortion get_distortion_transform();
    ImageToViewport get_image_to_viewport_transform();
    void draw_image();
    void draw_points();
    public:
    ImageViewRenderer(boost::shared_ptr<Core::Image> im);
    ~ImageViewRenderer();
    void draw();
    void configure(unsigned int width, unsigned int height);
    void realize();
    void set_zoom(double zoom);
    void set_zoom_center(Core::Point2D zoom_center);
    Core::Point2D as_image_coords(Core::Point2D pt);
  };

  // The transformation between image 0-1 coordinates and viewport 0-1 coordinates
  inline GtkGui::ImageViewRenderer::ImageToViewport::ImageToViewport
    (const double _img_aspect, const double _vp_aspect) :
    Core::Transform2D::Transformation<
      Core::Transform2D::AtOrigin<
        Core::Transform2D::Scaling
      >
    >(
      0.5, 0.5,
      Core::Transform2D::Scaling(
        (_img_aspect > _vp_aspect) ? (_vp_aspect  / _img_aspect) : 1.0,
        (_img_aspect > _vp_aspect) ? 1.0                     : (_img_aspect /  _vp_aspect)
      )
    )
  {
  }

  // Origin distortion
  inline GtkGui::ImageViewRenderer::OriginDistortionBase::OriginDistortionBase(double _zoom, double _sigma) : zoom(_zoom), sigma(_sigma) {
  }

  inline Core::Point2D GtkGui::ImageViewRenderer::OriginDistortionBase::t(Core::Point2D input) {
    double dist = sqrt(input.x*input.x + input.y*input.y);
    double warp = radial_func(dist);

    return Core::Point2D(
      input.x*radial_func(dist),
      input.y*radial_func(dist)
    );
  }

  template <class other>
  inline GtkGui::ImageViewRenderer::OriginDistortionBase::OriginDistortionBase(other t) : zoom(t.zoom), sigma(t.sigma) {
  }

  inline double GtkGui::ImageViewRenderer::OriginDistortionBase::inverse_initial_llim(double dist) {
    return dist;
  }

  inline double GtkGui::ImageViewRenderer::OriginDistortionBase::inverse_initial_ulim(double dist) {
    return dist*(1.0+(zoom-1.0)*exp(-0.5*(dist/sigma)*(dist/sigma)));
  }

  inline double GtkGui::ImageViewRenderer::OriginDistortionBase::radial_func(double dist) {
    return dist/(1.0+(zoom-1.0)*exp(-0.5*(dist/sigma)*(dist/sigma)));
  }

  inline GtkGui::ImageViewRenderer::OriginDistortionBase::Inverse
    GtkGui::ImageViewRenderer::OriginDistortionBase::inverse() {
    namespace phx = boost::phoenix;
    namespace ph = boost::phoenix::placeholders;

    return GtkGui::ImageViewRenderer::OriginDistortionBase::Inverse(
      *this,
      phx::bind(&GtkGui::ImageViewRenderer::OriginDistortionBase::inverse_initial_llim, *this, ph::_1),
      phx::bind(&GtkGui::ImageViewRenderer::OriginDistortionBase::inverse_initial_ulim, *this, ph::_1),
      phx::bind(&GtkGui::ImageViewRenderer::OriginDistortionBase::radial_func, *this, ph::_1),
      0.0002 // Correct pixel on most displays
    );
  }

  // OriginDistortion anywhere
  inline GtkGui::ImageViewRenderer::DistortionBase::DistortionBase(double ox, double oy, double zoom, double sigma) :
    Core::Transform2D::AtOrigin<
      Core::Transform2D::Transformation<ImageViewRenderer::OriginDistortionBase>
    > (ox, oy, Core::Transform2D::Transformation<ImageViewRenderer::OriginDistortionBase>(zoom, sigma))
  {
  }

  inline GtkGui::ImageViewRenderer::DistortionBase::DistortionBase(Core::Point2D origin, double zoom, double sigma) :
    Core::Transform2D::AtOrigin<
      Core::Transform2D::Transformation<
        OriginDistortionBase
      >
    > (origin.x, origin.y, Core::Transform2D::Transformation<ImageViewRenderer::OriginDistortionBase>(zoom, sigma))
  {
  }

}

#endif
