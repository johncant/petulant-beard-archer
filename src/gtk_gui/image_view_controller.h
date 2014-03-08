#ifndef __IMAGE_VIEW_CONTROLLER_H__
#define __IMAGE_VIEW_CONTROLLER_H__

#include <boost/shared_ptr.hpp>
#include "viewer_controller.h"
#include "image_view_renderer.h"
#include "../core/image.h"
#include "../core/point2d.h"

namespace GtkGui {
  class ImageViewController : public ViewerController {
    double zoom_level;
    Core::Point2D zoom_center;
    GdkCursor *cursor_crosshair;

    protected:
    boost::shared_ptr<GtkGui::ImageViewRenderer> renderer;
    boost::shared_ptr<Core::Image> image;

    public:
    ImageViewController(boost::shared_ptr<Core::Image> im);
    ~ImageViewController();

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

#endif
