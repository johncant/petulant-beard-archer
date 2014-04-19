#include "fake_renderer.h"


// External names
using Core::Image;
using Core::Point2D;

namespace TF {
  using namespace Core::Transform2D;
  using namespace GtkGui::Viewer::ImageView::Transforms;
};

// Get in context
namespace GtkGui { namespace Viewer { namespace ImageView {

FakeRenderer::FakeRenderer(boost::shared_ptr<Core::Image> im) :
  GtkGui::Viewer::ImageView::Renderer(im) {
}

void FakeRenderer::realize() {
  std::cout << "Realize event overridden" << std::endl;
}

void FakeRenderer::configure(unsigned int width, unsigned int height) {
  std::cout << "Configure event overridden" << std::endl;
  vp_width = double(width);
  vp_height = double(height);
}

void FakeRenderer::draw(std::vector<boost::tuple<Core::Point2D, PointViewParams> > const & points) {
  std::cout << "Draw event overridden" << std::endl;
}

}}}
