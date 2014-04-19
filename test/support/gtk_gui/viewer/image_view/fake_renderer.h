#ifndef __TEST_GTKGUI_VIEWER_IMAGE_VIEW_FAKE_RENDERER_H__
#define __TEST_GTKGUI_VIEWER_IMAGE_VIEW_FAKE_RENDERER_H__

#include "src/gtk_gui/viewer/image_view/renderer.h"

namespace GtkGui {
  namespace Viewer {
    namespace ImageView {
      class FakeRenderer : public GtkGui::Viewer::ImageView::Renderer {
        public:
        FakeRenderer(boost::shared_ptr<Core::Image> im);
        void draw(std::vector<boost::tuple<Core::Point2D, GtkGui::Viewer::ImageView::PointViewParams, unsigned int> > const & points);
        void configure(unsigned int width, unsigned int height);
        void realize();
      };

    }
  }
}

#endif
