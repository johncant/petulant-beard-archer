#include "image_view_controller.h"
#include "image_view_renderer.h"

GtkGui::ImageViewController::ImageViewController(boost::shared_ptr<Core::Image> im) :
  renderer(boost::shared_ptr<GtkGui::Renderer>(new GtkGui::ImageViewRenderer(im)))
{
}

GtkGui::ImageViewController::~ImageViewController() {
}

void GtkGui::ImageViewController::realize() {
  renderer->realize();
}

void GtkGui::ImageViewController::configure(unsigned int width, unsigned int height) {
  renderer->configure(width, height);
}

void GtkGui::ImageViewController::draw() {
  renderer->draw();
}

