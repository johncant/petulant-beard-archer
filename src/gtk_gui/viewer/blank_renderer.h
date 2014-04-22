#ifndef __BLANK_RENDERER_H__
#define __BLANK_RENDERER_H__

#include "renderer.h"

namespace GtkGui {
  namespace Viewer {
    class BlankRenderer : public Renderer {
      public:
      BlankRenderer();
      ~BlankRenderer();
      void draw();
      void configure(unsigned int width, unsigned int height);
      void realize();
    };
  }
}

#endif
