#ifndef __IMAGE_VIEW_CONTROLLER_H__
#define __IMAGE_VIEW_CONTROLLER_H__

#include <boost/shared_ptr.hpp>
#include <gtkmm/widget.h>
#include "../controller.h"
#include "../renderer.h"
#include "renderer.h"
#include "image_controller.h"
#include "../../../core/image.h"
#include "../../../core/point2d.h"
#include "point_view_params.h"

namespace GtkGui {
  namespace Viewer {
    namespace ImageView {

//      class Selection {
//        std::vector<PointRef> points;
//
//        public:
//        Selection operator+(const Selection& other) const;
//        std::vector<PointRef> get_points();
//        void add_point();
//      };

      template <class widget_controllable_t, class renderer_t = GtkGui::Viewer::ImageView::Renderer>
      class Controller : public Viewer::Controller {
        boost::shared_ptr<widget_controllable_t> widget_controllable;
        public:

        typedef GtkGui::Viewer::ImageView::ImageController<
          PointViewParams
        > ImageController;
        typedef ImageController::PointRef PointRef;

        protected:
        PointRef highlighted_point;
        PointRef drag_point;
        boost::shared_ptr<ImageController> image_controller;
        double zoom_level;
        Core::Point2D zoom_center;

        protected:
        boost::shared_ptr<renderer_t> renderer;
        std::vector<sigc::connection> signal_connections;

        void connect_signal_handlers();
        void disconnect_signal_handlers();

        public:
        Controller(
          boost::shared_ptr<widget_controllable_t> controllable,
          boost::shared_ptr<ImageController> im
        );
        ~Controller();

        void draw();
        void configure(unsigned int width, unsigned int height);
        void realize();
        double get_zoom();
        bool on_motion_notify_event(GdkEventMotion* evt);
        bool on_enter_notify_event(GdkEventCrossing* evt);
        bool on_leave_notify_event(GdkEventCrossing* evt);
        bool on_scroll(GdkEventScroll* evt);
        bool on_button_press_event(GdkEventButton* evt);
        bool on_button_release_event(GdkEventButton* evt);

      };

    }
  }
}

#define __INSIDE_GTK_GUI_VIEWER_IMAGE_VIEW_CONTROLLER_H__
#include "controller_impl.h"
#undef __INSIDE_GTK_GUI_VIEWER_IMAGE_VIEW_CONTROLLER_H__

#endif
