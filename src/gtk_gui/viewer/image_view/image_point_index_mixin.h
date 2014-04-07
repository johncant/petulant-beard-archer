#ifndef __VIEWER_IMAGE_VIEW_IMAGE_POINT_INDEX_MIXIN_H__
#define __VIEWER_IMAGE_VIEW_IMAGE_POINT_INDEX_MIXIN_H__

#include <exception>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/index/parameters.hpp>
#include <boost/tuple/tuple.hpp>
#include "../../../core/point2d.h"
#include "../../../core/point2d/indexable.h"


namespace GtkGui {
  namespace Viewer {
    namespace ImageView {

      template <class mixin_base, typename user_data_t> class ImagePointIndexMixin;

      template <class mixin_t>
      class PointReference {
        template <class mixin_base, typename user_data_t>
        friend class ImagePointIndexMixin;

        public: // TODO - not public
        int id;
        mixin_t* controller;

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

        operator bool() const {
          return id != -1;
        }

        bool operator==(const PointReference<mixin_t>& other) const {
          return other.controller == controller
              && other.id         == id;
        }

        void operator=(const PointReference<mixin_t>& other) {
          if (controller == other.controller) {
            id = other.id;
          } else {
            throw std::invalid_argument("PointReference: Assignment across controllers");
          }
        }

        PointReference(mixin_t& c) :
          controller(&c), id(-1) {}

        template <class mixin_derived_t>
        PointReference(mixin_derived_t& c, const int _id) :
          controller(&c), id(_id) {}

        PointReference(const PointReference& ref) :
          controller(ref.controller), id(ref.id) {}

        PointerAdapter const operator->() const {
          if (!*this) {
            throw std::runtime_error("PointReference: bad dereference");
          } else {
            return PointerAdapter(controller->get_point(*this).template get<0>());
          }
        }

        PointViewParams& params() {

          return controller->get_point(*this).template get<1>();
        }
      };

      template <class mixin_base, typename user_data_t>
      class ImagePointIndexMixin : public mixin_base {
        // This class acts like a database

        public:
        typedef PointReference<ImagePointIndexMixin> PointRef;
        typedef boost::tuple<Core::Point2D, user_data_t> tuple_pt_ud;

        private:
        // Index by some ID we assign
        std::map<unsigned int, tuple_pt_ud> points;

        boost::geometry::index::rtree<
          PointRef,
          boost::geometry::index::rstar<16, 4>
        > rtree;

        int max_index;

        // Add/remove points
        public:
        std::vector<PointRef> get_points();
        std::vector<tuple_pt_ud> get_points_values();
        PointRef add_point(Core::Point2D point, const user_data_t& user_data);
        void remove_point(PointRef point_id);
        tuple_pt_ud& get_point(const PointRef& p);
        const tuple_pt_ud& get_point(const PointRef& p) const;
        template <typename bounding_geometry_t>
        PointRef get_point_under_cursor(const Core::Point2D& cursor, const bounding_geometry_t &geometry);

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
