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

      class Controller : public Viewer::Controller {

        public:

        typedef GtkGui::Viewer::ImageView::ImageController<
          PointViewParams
        > ImageController;
        typedef ImageController::PointRef PointRef;

        protected:
        PointRef highlighted_point;
        boost::shared_ptr<ImageController> image_controller;
        double zoom_level;
        Core::Point2D zoom_center;
        GdkCursor *cursor_crosshair;

        protected:
        boost::shared_ptr<GtkGui::Viewer::ImageView::Renderer> renderer;
        std::vector<sigc::connection> signal_connections;

        void connect_signal_handlers(Gtk::Widget &parent);
        void disconnect_signal_handlers();

        public:
        Controller(
          Gtk::Widget &parent,
          boost::shared_ptr<ImageController> im
        );
        ~Controller();

        void draw(GdkWindow *window);
        void configure(unsigned int width, unsigned int height, GdkWindow *window);
        void realize(GdkWindow *window);
        double get_zoom();
        bool on_motion_notify_event(GdkEventMotion* evt);
        bool on_enter_notify_event(GdkEventCrossing* evt);
        bool on_leave_notify_event(GdkEventCrossing* evt);
        bool on_scroll(GdkEventScroll* evt);
        bool on_button_press_event(GdkEventButton* evt);

      };

    }
  }
}

#endif
