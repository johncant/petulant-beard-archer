#ifndef __GTK_GUI_VIEWER_IMAGE_VIEW_SELECTION__
#define __GTK_GUI_VIEWER_IMAGE_VIEW_SELECTION__

#include <boost/unordered_set.hpp>

namespace GtkGui {
  namespace Viewer {
    namespace ImageView {
      template <typename selectable_t>
      class Selection : public boost::unordered_set<selectable_t> {
        public:
        void operator=(const selectable_t &s) {
          this->clear();
          this->insert(s);
        }
      };

      template <typename selectable_t>
      Selection<selectable_t> operator+=(Selection<selectable_t> &s, const selectable_t &r) {
        s.insert(r);
        return s;
      }

      template <typename selectable_t>
      Selection<selectable_t> operator+=(Selection<selectable_t> &s, const Selection<selectable_t> &r) {
        s.insert(r.begin(), r.end());
        return s;
      }

      template <typename selectable_t>
      Selection<selectable_t> operator-=(Selection<selectable_t> &s, const selectable_t &r) {
        s.remove(r);
        return s;
      }

      template <typename selectable_t>
      Selection<selectable_t> operator-=(Selection<selectable_t> &s, const Selection<selectable_t> &r) {
        s.remove(r.begin(), r.end());
        return s;
      }

    }
  }
}
#endif
