#ifndef __CORE_POINT2D_INDEXABLE_H__
#define __CORE_POINT2D_INDEXABLE_H__

#include "../point2d.h"
#include <boost/geometry.hpp>

namespace boost { namespace geometry { namespace index {

template <>
class indexable<Core::Point2D> {
  public:
  typedef boost::geometry::model::point<double, 2, cs::cartesian> result_type;
  result_type operator()(Core::Point2D const &v) const {
    return result_type(v.x, v.y);
  }
};

}}}
#endif

