#ifndef __RENDERER_H__
#define __RENDERER_H__

namespace GtkGui {
  class Renderer {
    public:
    Renderer();
    ~Renderer();
    virtual void draw();
    virtual void configure(unsigned int width, unsigned int height);
    virtual void realize();
  };
}

#endif
