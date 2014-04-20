#ifndef __IMAGE_VIEW_RENDERER_H__
#define __IMAGE_VIEW_RENDERER_H__

#include <boost/shared_ptr.hpp>
#include <GL/glew.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include "../renderer.h"
#include "../../../core/image.h"
#include "../../../core/transform2d.h"
#include "transforms.h"
#include "point_view_params.h"
#include "boost/tuple/tuple.hpp"

namespace GtkGui {
  namespace Viewer {
    namespace ImageView {
      class Renderer : public GtkGui::Viewer::Renderer {

        typedef boost::tuple<Core::Point2D, GtkGui::Viewer::ImageView::PointViewParams, unsigned int> tuple_pt_ud;
        protected:
        boost::shared_ptr<Core::Image> image;
        boost::shared_ptr<cv::Mat> pixels;
        boost::shared_ptr<cv::Mat> sprite_pixels;

        // TODO - hide ugly impl members
        GLuint gl_sprite_tex_id;
        GLuint gl_tex_id;
        GLuint shader_program;
        GLuint vertex_shader;
        GLuint fragment_shader;

        double zoom;
        double sigma_x;
        double sigma_y;
        Core::Point2D zoom_center;

        double vp_width;
        double vp_height;
        void init_shaders();
        GLuint compile(const std::string& shader, GLenum type);
        GLuint compile(const char* shader, GLenum type);

        GtkGui::Viewer::ImageView::Transforms::Distortion
          get_distortion_transform();
        GtkGui::Viewer::ImageView::Transforms::ImageToViewport
          get_image_to_viewport_transform();

        void draw_image();
        void draw_points(std::vector<tuple_pt_ud> const & points);
        public:
        Renderer(boost::shared_ptr<Core::Image> im);
        ~Renderer();
        void draw(std::vector<tuple_pt_ud> const & points);
        void configure(unsigned int width, unsigned int height);
        void realize();
        void set_zoom(double zoom);
        void set_zoom_center(Core::Point2D zoom_center);
        Core::Point2D as_image_coords(Core::Point2D pt);
        Core::Point2D as_viewport_coords(Core::Point2D pt);

        boost::geometry::model::polygon<
          boost::geometry::model::point<
            double, 2, boost::geometry::cs::cartesian
          >
        > get_reverse_marker_bounds(const Core::Point2D& pt);
      };

    }
  }
}

#endif
