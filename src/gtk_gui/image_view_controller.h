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

    protected:
    boost::shared_ptr<GtkGui::ImageViewRenderer> renderer;
    boost::shared_ptr<Core::Image> image;

    public:
    ImageViewController(boost::shared_ptr<Core::Image> im);
    ~ImageViewController();

    void draw();
    void configure(unsigned int width, unsigned int height);
    void realize();
    double get_zoom();
    bool on_motion_notify_event(GdkEventMotion* evt);
    bool on_scroll(GdkEventScroll* evt);
  };
}

#endif
