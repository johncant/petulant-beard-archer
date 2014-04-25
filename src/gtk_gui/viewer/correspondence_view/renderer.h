#ifndef __CORRESPONDENCE_VIEW_RENDERER_H__
#define __CORRESPONDENCE_VIEW_RENDERER_H__

#include <boost/shared_ptr.hpp>
#include <GL/glew.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include "../renderer.h"
#include "../image_view/renderer.h"
#include "../../../core/image.h"
#include "boost/tuple/tuple.hpp"

namespace GtkGui {
  namespace Viewer {
    namespace CorrespondenceView {
      class Renderer : public GtkGui::Viewer::Renderer {

        double vp_width;
        double vp_height;
        int shader_program;
        int vertex_shader;
        int fragment_shader;
        void init_shaders();
        GLuint compile(const std::string& shader, GLenum type);
        GLuint compile(const char* shader, GLenum type);

        public:
        Renderer();
        ~Renderer();
        // TODO - Yuck
        void draw(
          const std::vector<
            boost::tuple<
              std::vector<GtkGui::Viewer::ImageView::Renderer::tuple_pt_ud>,
              bool,
              Core::Point2D,
              Core::Point2D
            >
          >& image_draw_args
        );
        void configure(unsigned int width, unsigned int height);
        void realize();

      };

    }
  }
}

#endif
