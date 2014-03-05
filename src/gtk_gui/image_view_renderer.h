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
    class OriginUndistortionBase;

    class OriginUndistortionBase : public Core::Transform2D::Base {
      public:
      class Inverse;
      double zoom, sigma;

      OriginUndistortionBase(double _zoom, double _sigma);
      Core::Point2D t(Core::Point2D input);
      template <class other>
      OriginUndistortionBase(other t);
      Inverse inverse();
    };

    class OriginDistortionBase : public Core::Transform2D::Base {
      public:
      class Inverse;
      double zoom, sigma;

      OriginDistortionBase(double _zoom, double _sigma);
      Core::Point2D t(Core::Point2D input);
      template <class other>
      OriginDistortionBase(other t);
      Inverse inverse();
    };

    class OriginDistortionBase::Inverse : public OriginUndistortionBase {
      public:
      Inverse(double zoom, double sigma): OriginUndistortionBase(zoom, sigma) {}
    };

    class OriginUndistortionBase::Inverse : public OriginDistortionBase {
      public:
      Inverse(double zoom, double sigma): OriginDistortionBase(zoom, sigma) {}
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

    class UndistortionBase :
      public Core::Transform2D::AtOrigin<
        Core::Transform2D::Transformation<
          ImageViewRenderer::OriginUndistortionBase
        >
      >
    {
      public:
      UndistortionBase(double ox, double oy, double zoom, double sigma);
      UndistortionBase(Core::Point2D pt, double zoom, double sigma);
    };

    typedef Core::Transform2D::Transformation<DistortionBase> Distortion;
    typedef Core::Transform2D::Transformation<UndistortionBase> Undistortion;


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
    double ndist2 = input.x*input.x + input.y*input.y/(sigma*sigma);

    double warp = 1.0 + (zoom-1.0)*exp(-0.5*ndist2);

    return Core::Point2D(
      input.x*warp,
      input.y*warp
    );
  }

  template <class other>
  inline GtkGui::ImageViewRenderer::OriginDistortionBase::OriginDistortionBase(other t) : zoom(t.zoom), sigma(t.sigma) {
  }

  inline GtkGui::ImageViewRenderer::OriginDistortionBase::Inverse GtkGui::ImageViewRenderer::OriginDistortionBase::inverse() {
    return GtkGui::ImageViewRenderer::OriginDistortionBase::Inverse(this->zoom, this->sigma);
  }

  // Origin undistortionBase
  inline GtkGui::ImageViewRenderer::OriginUndistortionBase::OriginUndistortionBase(double _zoom, double _sigma) : zoom(_zoom), sigma(_sigma) {
  }

  inline Core::Point2D GtkGui::ImageViewRenderer::OriginUndistortionBase::t(Core::Point2D input) {
    double ndist2 = input.x*input.x + input.y*input.y/(sigma*sigma);

    double warp = 1.0 + (zoom-1.0)*exp(-0.5*ndist2);

    return Core::Point2D(
      input.x/warp,
      input.y/warp
    );
  }

  template <class other>
  inline GtkGui::ImageViewRenderer::OriginUndistortionBase::OriginUndistortionBase(other t) : zoom(t.zoom), sigma(t.sigma) {
  }

  inline GtkGui::ImageViewRenderer::OriginUndistortionBase::Inverse GtkGui::ImageViewRenderer::OriginUndistortionBase::inverse() {
    return GtkGui::ImageViewRenderer::OriginUndistortionBase::Inverse(zoom, sigma);
  }

  // DistortionBase anywhere
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

  // UndistortionBase anywhere
  inline GtkGui::ImageViewRenderer::UndistortionBase::UndistortionBase(double ox, double oy, double zoom, double sigma) :
    Core::Transform2D::AtOrigin<
      Core::Transform2D::Transformation<ImageViewRenderer::OriginUndistortionBase>
    > (ox, oy, Core::Transform2D::Transformation<ImageViewRenderer::OriginUndistortionBase>(zoom, sigma))
  {
  }

  inline GtkGui::ImageViewRenderer::UndistortionBase::UndistortionBase(Core::Point2D origin, double zoom, double sigma) :
    Core::Transform2D::AtOrigin<
      Core::Transform2D::Transformation<
        OriginUndistortionBase
      >
    > (origin.x, origin.y, Core::Transform2D::Transformation<ImageViewRenderer::OriginUndistortionBase>(zoom, sigma))
  {
  }

}




#endif
