#ifndef __VIEWER_IMAGE_VIEW_IMAGE_POINT_INDEX_MIXIN_H__
#define __VIEWER_IMAGE_VIEW_IMAGE_POINT_INDEX_MIXIN_H__

#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/index/parameters.hpp>
#include "../../../core/point2d.h"
#include "../../../core/point2d/indexable.h"


namespace GtkGui {
  namespace Viewer {
    namespace ImageView {

      template <class mixin_base> class ImagePointIndexMixin;

      template <class mixin_t>
      class PointReference {
        template <class mixin_base>
        friend class ImagePointIndexMixin;

        int id;
        const mixin_t* controller;

        public:
        class PointerAdapter {
          Core::Point2D point;
          public:
          PointerAdapter(const Core::Point2D &pt)
            : point(pt) {}
          Core::Point2D const* operator->() const {
            return &point;
          }
        };

        PointReference(mixin_t const & c, int _id) :
          controller(&c), id(_id) {}
        PointReference(const PointReference& ref) :
          controller(ref.controller), id(ref.id) {
        }

        PointerAdapter const operator->() const {
          return PointerAdapter(controller->get_point(*this));
        }

      };

      template <class mixin_base>
      class ImagePointIndexMixin : public mixin_base {
        // This class acts like a database

        public:
        typedef PointReference<ImagePointIndexMixin> PointRef;

        private:
        // Index by some ID we assign
        std::map<unsigned int, Core::Point2D> points;
        boost::geometry::index::rtree<
          PointRef,
          boost::geometry::index::rstar<16, 4>
        > rtree;

        int max_index;

        // Add/remove points
        public:

        PointRef add_point(Core::Point2D point);
        void remove_point(PointRef point_id);
        Core::Point2D const get_point(const PointRef& p) const;

        // Queries - TODO
//        Core::Point2D query_id(PointRef& pr);
//        Core::Point2D query_(Core::Point2D top_left, Core::Point2D bottom_right);
//        Core::Point2D query(Core::Point2D qp, double dist);

        ImagePointIndexMixin(boost::shared_ptr<Core::Image> const image);
      };

    }
  }
}

namespace boost {
namespace geometry {
namespace index {

template <class base>
class indexable<GtkGui::Viewer::ImageView::PointReference<base> > {
  public:
  typedef boost::geometry::model::point<double, 2, cs::cartesian> result_type;
  result_type operator()(GtkGui::Viewer::ImageView::PointReference<base> const &r) const {
    return result_type(r->x, r->y);
  }
};

}}}

#define __INSIDE_IMAGE_POINT_INDEX_MIXIN_H__
#include "image_point_index_mixin_impl.h"
#undef __INSIDE_IMAGE_POINT_INDEX_MIXIN_H__

#endif
