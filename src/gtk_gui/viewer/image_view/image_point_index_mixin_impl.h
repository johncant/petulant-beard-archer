#ifndef __VIEWER_IMAGE_VIEW_IMAGE_POINT_INDEX_MIXIN_IMPL_H__
#define __VIEWER_IMAGE_VIEW_IMAGE_POINT_INDEX_MIXIN_IMPL H__

#include "image_point_index_mixin.h"

#ifndef __INSIDE_IMAGE_POINT_INDEX_MIXIN_H__
  #error "This file should never be included directly"
#else

#include "controller.h"

namespace GtkGui {
namespace Viewer {
namespace ImageView {

template <class base>
typename ImagePointIndexMixin<base>::PointRef ImagePointIndexMixin<base>::add_point(Core::Point2D point) {
  PointRef ref(*this, ++max_index);

  // Add to model
  this->image->points.push_back(point);

  // Add to indices
  points.insert(std::make_pair(max_index, point));
  rtree.insert(ref);

  return ref;
}

template <class base>
void ImagePointIndexMixin<base>::remove_point(ImagePointIndexMixin<base>::PointRef point_id) {
  rtree.remove(point_id.id);
  points.erase(point_id.id);
}

template <class base>
Core::Point2D const ImagePointIndexMixin<base>::get_point(const ImagePointIndexMixin<base>::PointRef& p) const {
  return points.at(p.id);
}

template <class mixin_base>
ImagePointIndexMixin<mixin_base>::ImagePointIndexMixin(boost::shared_ptr<Core::Image> image) :
  mixin_base(image),
  max_index(image->points.size() - 1)
{
  for (int i=0; i<=max_index ; i++) {
    const PointRef ref(*this, i);
    points.insert(std::make_pair(i, image->points[i]));
    rtree.insert(ref);
  }

}

}}}

#endif
#endif
