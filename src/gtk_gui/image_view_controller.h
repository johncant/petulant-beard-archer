#ifndef __IMAGE_VIEW_CONTROLLER_H__
#define __IMAGE_VIEW_CONTROLLER_H__

#include <boost/shared_ptr.hpp>
#include "viewer_controller.h"
#include "../core/image.h"
#include "../core/point2d.h"

namespace GtkGui {
  class ImageViewController : public ViewerController {
    protected:
    boost::shared_ptr<GtkGui::Renderer> renderer;
    boost::shared_ptr<Core::Image> image;

    public:
    ImageViewController(boost::shared_ptr<Core::Image> im);
    ~ImageViewController();

    void draw();
    void configure(unsigned int width, unsigned int height);
    void realize();
  };
}

#endif
