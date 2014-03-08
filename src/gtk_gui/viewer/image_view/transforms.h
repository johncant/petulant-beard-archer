#ifndef __VIEWER_IMAGE_VIEW_TRANSFORMS_H__
#define __VIEWER_IMAGE_VIEW_TRANSFORMS_H__

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind.hpp>

#include "../../../core/transform2d.h"

namespace GtkGui {
  namespace Viewer {
    namespace ImageView {
      namespace Transforms {

        // The transform between image 0-1 coordinates and viewport 0-1 coordinates
        class ImageToViewport :
          public Core::Transform2D::AtOrigin<
            Core::Transform2D::Scaling
          >::type
        {

          public:
          typedef Core::Transform2D::AtOrigin<
            Core::Transform2D::Scaling
          > Inverse;

          inline ImageToViewport(const double img_aspect, const double vp_aspect);

        };

        // TODO - Use a combination of transformations
        class OriginDistortionBase;

        class OriginDistortionBase : public Core::Transform2D::Base {
          public:
          typedef Core::Transform2D::BisectionUnscale<OriginDistortionBase>::type Inverse;
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

        typedef Core::Transform2D::Transform<OriginDistortionBase> OriginDistortion;

        typedef Core::Transform2D::AtOrigin<OriginDistortion>::type Distortion;

      }
    }
  }
}

#define __INSIDE_TRANSFORMS_H__
#include "transforms_impl.h"
#undef __INSIDE_TRANSFORMS_H__

#endif // __VIEWER_IMAGE_VIEW_TRANSFORMS_H__

