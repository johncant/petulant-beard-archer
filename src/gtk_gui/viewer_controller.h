#ifndef __VIEWER_CONTROLLER_H__
#define __VIEWER_CONTROLLER_H__

#include <boost/shared_ptr.hpp>
#include "renderer.h"
#include <gdk/gdk.h>

namespace GtkGui {
  class ViewerController {
    public:
    ViewerController();
    ~ViewerController();
    virtual void draw(GdkWindow *window) = 0;
    virtual void configure(unsigned int width, unsigned int height, GdkWindow *window) = 0;
    virtual void realize(GdkWindow *window) = 0;
  };
}

#endif
