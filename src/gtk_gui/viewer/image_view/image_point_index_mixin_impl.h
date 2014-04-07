#ifndef __VIEWER_IMAGE_VIEW_IMAGE_POINT_INDEX_MIXIN_IMPL_H__
#define __VIEWER_IMAGE_VIEW_IMAGE_POINT_INDEX_MIXIN_IMPL H__

#include <boost/phoenix.hpp>
#include <boost/tuple/tuple.hpp>
#include <algorithm>
#include "image_point_index_mixin.h"

#ifndef __INSIDE_IMAGE_POINT_INDEX_MIXIN_H__
  #error "This file should never be included directly"
#else

#include "controller.h"

namespace GtkGui {
namespace Viewer {
namespace ImageView {

namespace phx {
  using namespace boost::phoenix::arg_names;
  using namespace boost::phoenix::local_names;
  using namespace boost::phoenix;
}

template <class base, typename user_data_t>
typename ImagePointIndexMixin<base, user_data_t>::PointRef ImagePointIndexMixin<base, user_data_t>::add_point(Core::Point2D point, const user_data_t& user_data = user_data_t()) {
  PointRef ref(*this, ++max_index);

  // Add to model
  this->image->points.push_back(point);

  // Add to indices
  points.insert(std::make_pair(max_index, boost::make_tuple(point, user_data)));
  rtree.insert(ref);

  return ref;
}

template <class base, typename user_data_t>
std::vector<
  typename ImagePointIndexMixin<base, user_data_t>::PointRef
> ImagePointIndexMixin<base, user_data_t>::get_points() {

  typedef std::vector<PointRef> result_t;
  result_t result;

  for (
    typename tuple_pt_ud::iterator it = points.begin();
    it != points.end();
    it++
  ) {
    result.push_back(it->first);
  }

  return result;
}

template <class base, typename user_data_t>
std::vector<
  typename ImagePointIndexMixin<base, user_data_t>::tuple_pt_ud
> ImagePointIndexMixin<base, user_data_t>::get_points_values() {

  std::vector<tuple_pt_ud> result;

  for (
    typename std::map<unsigned int, tuple_pt_ud>::iterator it = points.begin();
    it != points.end();
    it++
  ) {
    result.push_back(it->second);
  }

  return result;
}

template <class base, typename user_data_t>
void ImagePointIndexMixin<base, user_data_t>::remove_point(ImagePointIndexMixin<base, user_data_t>::PointRef point_id) {
  rtree.remove(point_id.id);
  points.erase(point_id.id);
}

template <class base, typename user_data_t>
boost::tuple<Core::Point2D, user_data_t>& ImagePointIndexMixin<base, user_data_t>::get_point(const ImagePointIndexMixin<base, user_data_t>::PointRef& p) {
  return points.at(p.id);
}

template <class base, typename user_data_t>
const boost::tuple<Core::Point2D, user_data_t>& ImagePointIndexMixin<base, user_data_t>::get_point(const ImagePointIndexMixin<base, user_data_t>::PointRef& p) const {
  return points.at(p.id);
}

template <class mixin_base, typename user_data_t>
ImagePointIndexMixin<mixin_base, user_data_t>::ImagePointIndexMixin(boost::shared_ptr<Core::Image> image) :
  mixin_base(image),
  max_index(image->points.size() - 1)
{

  for (int i=0; i<=max_index ; i++) {
    const PointRef ref(*this, i);
    points.insert(std::make_pair(i, boost::make_tuple(image->points[i], user_data_t())));
    rtree.insert(ref);
  }

  std::cout << image->points.size() << " points inserted on initialization" << std::endl;
}

template <class mixin_base, typename user_data_t>
template <typename bounding_geometry_t>
typename ImagePointIndexMixin<mixin_base, user_data_t>::PointRef
ImagePointIndexMixin<mixin_base, user_data_t>::
get_point_under_cursor(const Core::Point2D& cursor, const bounding_geometry_t& bounding_geometry ) {

  typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> pt_t;
  typedef std::vector<PointRef> result_t;

  pt_t __cursor(cursor.x, cursor.y);

  result_t rtree_result;

  rtree.query(
    boost::geometry::index::nearest(__cursor, 1) &&
    boost::geometry::index::within(bounding_geometry),
    std::back_inserter(rtree_result)
  );

  if (rtree_result.size() > 0) {
    return rtree_result[0];
  } else {
    return PointRef(*this);
  }
}

}}}

#endif
#endif
