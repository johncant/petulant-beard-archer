#ifndef __VIEWER_IMAGE_VIEW_TRANSFORMS_IMPL_H__
#define __VIEWER_IMAGE_VIEW_TRANSFORMS_IMPL_H__

// Give vim and IDEs some context even though we ....
#include "transforms.h"
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind.hpp>

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
  inline StandardOriginDistortionBase::StandardOriginDistortionBase(double _zoom) : zoom(_zoom) {
  }

  inline Core::Point2D StandardOriginDistortionBase::t(Core::Point2D input) {

    double dist = sqrt(input.x*input.x + input.y*input.y);
    double warp = radial_func(dist)/dist;

    return Core::Point2D(
      input.x*warp,
      input.y*warp
    );
  }

  template <class other>
  inline StandardOriginDistortionBase::StandardOriginDistortionBase(other od) : zoom(od.zoom) {
  }

  inline double StandardOriginDistortionBase::inverse_initial_llim(double dist) {
    return dist;
  }

  inline double StandardOriginDistortionBase::inverse_initial_ulim(double dist) {
    return dist*(1.0+(zoom-1.0)*exp(-0.5*(dist)*(dist)));
  }

  inline double StandardOriginDistortionBase::radial_func(double dist) {
    return dist/(1.0+(zoom-1.0)*exp(-0.5*(dist)*(dist)));
  }

  inline StandardOriginDistortionBase::Inverse
    StandardOriginDistortionBase::inverse() {
    namespace phx = boost::phoenix;
    namespace ph = boost::phoenix::placeholders;

    return StandardOriginDistortionBase::Inverse(
      *this,
      phx::bind(&StandardOriginDistortionBase::inverse_initial_llim, *this, ph::_1),
      phx::bind(&StandardOriginDistortionBase::inverse_initial_ulim, *this, ph::_1),
      phx::bind(&StandardOriginDistortionBase::radial_func, *this, ph::_1),
      0.0002 // Correct pixel on most displays
    );
  }

  inline OriginDistortionBase::OriginDistortionBase(double _zoom, double sigma_x, double sigma_y) :
    Core::Transform2D::Combination<
      Core::Transform2D::Combination<
        Core::Transform2D::Scaling,
        StandardOriginDistortionBase
      >::type,
      Core::Transform2D::Scaling
    >::type(
      Core::Transform2D::Combination<
        Core::Transform2D::Scaling,
        StandardOriginDistortionBase
      >::type(
        Core::Transform2D::Scaling(sigma_x, sigma_y).inverse(),
        StandardOriginDistortionBase(_zoom)
      ),
      Core::Transform2D::Scaling(sigma_x, sigma_y)
    ) {
  }

}}}}

#endif // __VIEWER_IMAGE_VIEW_TRANSFORMS_IMPL_H__
