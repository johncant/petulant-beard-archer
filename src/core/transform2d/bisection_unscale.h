#ifndef __TRANSFORM2D_BISECTION_UNSCALE_H__
#define __TRANSFORM2D_BISECTION_UNSCALE_H__

#include "base.h"
#include "transform.h"
#include "translation.h"

#include <boost/function.hpp>
#include <limits>

namespace Core {

  namespace Transform2D {

    template <class tform_type>
    class BisectionUnscaleBase : public Base {
      // Finds the inverse of a variable scaling by bisection
      // Only use if the scaling is monotonically increasing
      // and has no stationary points.

      public:
      typedef Transform<tform_type> Inverse;

      tform_type tform;
      double precision;
      boost::function<double(double)> ulim_func;
      boost::function<double(double)> llim_func;
      boost::function<double(double)> radial_func;

      inline BisectionUnscaleBase(
        tform_type _tform,
        boost::function<double(double)> _llim_func,
        boost::function<double(double)> _ulim_func,
        boost::function<double(double)> _radial_func,
        double _precision
      ) :
        tform(_tform),
        ulim_func(_ulim_func),
        llim_func(_llim_func),
        radial_func(_radial_func),
        precision(_precision) {
      }

      inline Core::Point2D t(Core::Point2D input) {
        double dist = sqrt(input.x*input.x + input.y*input.y);
        double ulim = ulim_func(dist);
        double llim = llim_func(dist);
        double ulim_result = radial_func(ulim);
        double llim_result = radial_func(llim);
        double test_val = std::numeric_limits<double>::max(),
               test_result,
               prev_test_val;

        do {
          prev_test_val = test_val;
          test_val = 0.5*(ulim+llim);
          test_result = radial_func(test_val);
          if (test_result < dist) {
            llim = test_val;
            llim_result = test_result;
          } else {
            ulim = test_val;
            ulim_result = test_result;
          }
        } while(fabs(test_val - prev_test_val) > precision);

        double final_result = 0.5*(ulim+llim), final_scale;

        if (fabs(final_result) < precision) {
          final_scale = 0;
        } else {
          final_scale = final_result/dist;
        }

        return Core::Point2D(input.x*final_scale, input.y*final_scale);
      }

      inline Inverse inverse() {
        return tform;
      }
    };

    template <typename tform_type>
    struct BisectionUnscale {
      typedef Transform<BisectionUnscaleBase<tform_type> > type;
    };

  }
}

#endif
