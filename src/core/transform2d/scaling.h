#ifndef __TRANSFORM2D_SCALING_H__
#define __TRANSFORM2D_SCALING_H__

#include "base.h"
#include "transform.h"

namespace Core {
  namespace Transform2D {

    class ScalingBase;

    class ScalingBase : public Base {
      public:
      typedef Transform<ScalingBase> Inverse;

      double scale_x;
      double scale_y;

      inline ScalingBase(const double _scale_x, const double _scale_y) : scale_x(_scale_x), scale_y(_scale_y) {
      }

      inline Core::Point2D t(Core::Point2D input) {
        return Core::Point2D(scale_x*input.x, scale_y*input.y);
      }

      inline Inverse inverse() {
        return Inverse(1.0/scale_x, 1.0/scale_y);
      }
    };

    typedef Transform<ScalingBase> Scaling;

  }
}

#endif
