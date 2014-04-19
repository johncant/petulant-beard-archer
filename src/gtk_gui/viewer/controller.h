#ifndef __VIEWER_CONTROLLER_H__
#define __VIEWER_CONTROLLER_H__

#include <boost/shared_ptr.hpp>
#include "renderer.h"
#include <gdk/gdk.h>

namespace GtkGui {
  namespace Viewer {
    class Controller {
      public:
      Controller();
      ~Controller();
      virtual void draw() = 0;
      virtual void configure(unsigned int width, unsigned int height) = 0;
      virtual void realize() = 0;
    };
  }
}

#endif
