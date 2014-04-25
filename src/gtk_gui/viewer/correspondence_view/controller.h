#ifndef __CORRESPONDENCE_VIEW_CONTROLLER_H__
#define __CORRESPONDENCE_VIEW_CONTROLLER_H__

#include <boost/shared_ptr.hpp>
#include <gtkmm/widget.h>
#include "../controller.h"
#include "../renderer.h"
#include "renderer.h"
#include "../../../core/image.h"
#include "../../../core/point2d.h"


namespace GtkGui {
  namespace Viewer {
    namespace CorrespondenceView {

//      class Selection {
//        std::vector<PointRef> points;
//
//        public:
//        Selection operator+(const Selection& other) const;
//        std::vector<PointRef> get_points();
//        void add_point();
//      };

      template <class widget_controllable_t, class renderer_t = GtkGui::Viewer::CorrespondenceView::Renderer>
      class Controller : public Viewer::Controller {

        boost::shared_ptr<renderer_t> renderer;
        boost::shared_ptr<widget_controllable_t> widget_controllable;
        std::vector<sigc::connection> signal_connections;
        public:

        void connect_signal_handlers();
        void disconnect_signal_handlers();
        Controller(
          boost::shared_ptr<widget_controllable_t> controllable
        );
        ~Controller();

        void draw();
        void configure(unsigned int width, unsigned int height);
        void realize();

      };
    }
  }
}

#define __INSIDE_GTK_GUI_VIEWER_CORRESPONDENCE_VIEW_CONTROLLER_H__
#include "controller_impl.h"
#undef __INSIDE_GTK_GUI_VIEWER_CORRESPONDENCE_VIEW_CONTROLLER_H__

#endif
