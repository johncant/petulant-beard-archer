#ifndef __VIEWER_IMAGE_VIEW_TRANSFORMS_IMPL_H__
#define __VIEWER_IMAGE_VIEW_TRANSFORMS_IMPL_H__

// Give vim and IDEs some context even though we ....
#include "transforms.h"

// .... only allow inclusion from one place 
#ifndef __INSIDE_TRANSFORMS_H__
  #error "This file should never be included directly"
#endif


namespace GtkGui { namespace Viewer { namespace ImageView { namespace Transforms {


// Distortion

  // The transformation between image 0-1 coordinates and viewport 0-1 coordinates
  inline ImageToViewport::ImageToViewport
    (const double _img_aspect, const double _vp_aspect) :
      Core::Transform2D::AtOrigin<Core::Transform2D::Scaling>::type(
      0.5, 0.5,
      Core::Transform2D::Scaling(
        (_img_aspect > _vp_aspect) ? (_vp_aspect  / _img_aspect) : 1.0,
        (_img_aspect > _vp_aspect) ? 1.0                     : (_img_aspect /  _vp_aspect)
      )
    )
  {
  }

  // Origin distortion
  inline OriginDistortionBase::OriginDistortionBase(double _zoom, double _sigma) : zoom(_zoom), sigma(_sigma) {
  }

  inline Core::Point2D OriginDistortionBase::t(Core::Point2D input) {
    double dist = sqrt(input.x*input.x + input.y*input.y);
    double warp = radial_func(dist);

    return Core::Point2D(
      input.x*radial_func(dist),
      input.y*radial_func(dist)
    );
  }

  template <class other>
  inline OriginDistortionBase::OriginDistortionBase(other od) : zoom(od.zoom), sigma(od.sigma) {
  }

  inline double OriginDistortionBase::inverse_initial_llim(double dist) {
    return dist;
  }

  inline double OriginDistortionBase::inverse_initial_ulim(double dist) {
    return dist*(1.0+(zoom-1.0)*exp(-0.5*(dist/sigma)*(dist/sigma)));
  }

  inline double OriginDistortionBase::radial_func(double dist) {
    return dist/(1.0+(zoom-1.0)*exp(-0.5*(dist/sigma)*(dist/sigma)));
  }

  inline OriginDistortionBase::Inverse
    OriginDistortionBase::inverse() {
    namespace phx = boost::phoenix;
    namespace ph = boost::phoenix::placeholders;

    return OriginDistortionBase::Inverse(
      *this,
      phx::bind(&OriginDistortionBase::inverse_initial_llim, *this, ph::_1),
      phx::bind(&OriginDistortionBase::inverse_initial_ulim, *this, ph::_1),
      phx::bind(&OriginDistortionBase::radial_func, *this, ph::_1),
      0.0002 // Correct pixel on most displays
    );
  }


}}}}

#endif // __VIEWER_IMAGE_VIEW_TRANSFORMS_IMPL_H__
