#ifndef __IMAGE_VIEW_RENDERER_H__
#define __IMAGE_VIEW_RENDERER_H__

#include <boost/shared_ptr.hpp>
#include "renderer.h"
#include "../core/image.h"

namespace GtkGui {
  class ImageViewRenderer : public Renderer {
    boost::shared_ptr<Core::Image> image;

    public:
    ImageViewRenderer(boost::shared_ptr<Core::Image> im);
    ~ImageViewRenderer();
    void draw();
    void configure(unsigned int width, unsigned int height);
    void realize();
  };
}

#endif
