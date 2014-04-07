#ifndef __GTKGUI_VIEWER_IMAGE_VIEW_POINT_VIEW_PARAMS__
#define __GTKGUI_VIEWER_IMAGE_VIEW_POINT_VIEW_PARAMS__

namespace GtkGui {
  namespace Viewer {
    namespace ImageView {

      class Controller;

      class PointViewParams {
        friend class ::GtkGui::Viewer::ImageView::Controller;

        enum FeatureType {
          SIFT, // Example
          SURF, // Example
          MANUAL
        };

        bool m_is_highlighted;
        bool m_is_selected;
        FeatureType m_feature_type;

        public:

      //          bool is_selected();
      //          bool is_highlighted();
      //          FeatureType feature_type();

        bool is_selected() const {
          return m_is_selected;
        }

        bool is_highlighted() const {
          return m_is_highlighted;
        }

        FeatureType feature_type() const {
          return m_feature_type;
        }

        PointViewParams() :
          m_is_selected(false),
          m_is_highlighted(false),
          m_feature_type(MANUAL)
        { }

      };

    }
  }
}

#endif
