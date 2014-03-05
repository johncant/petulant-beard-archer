#include "image_view_controller.h"
#include "boost/algorithm/clamp.hpp"

using boost::algorithm::clamp;

GtkGui::ImageViewController::ImageViewController(boost::shared_ptr<Core::Image> im) :
  image(im),
  renderer(boost::shared_ptr<GtkGui::ImageViewRenderer>(new GtkGui::ImageViewRenderer(im))),
  zoom_level(0),
  zoom_center(0.5, 0.5)
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

double GtkGui::ImageViewController::get_zoom() {
  // TODO - CONFIG
  double z = exp(zoom_level*log(20.0));
  std::cout << "Zooming to " << z << std::endl;
  return z;
}

bool GtkGui::ImageViewController::on_motion_notify_event(GdkEventMotion* evt) {
  gint w, h;

  gdk_window_get_geometry(evt->window, NULL, NULL, &w, &h, NULL);
  zoom_center = Core::Point2D(double(evt->x)/double(w), double(evt->y)/double(h));

  renderer->set_zoom_center(zoom_center);
  gdk_window_invalidate_rect(evt->window, NULL, true);
}

bool GtkGui::ImageViewController::on_scroll(GdkEventScroll* evt) {
  // TODO - CONFIG
  double increment = 0.1;

  if (zoom_level <= 0.5*increment) {
    gint w, h;
    gdk_window_get_geometry(evt->window, NULL, NULL, &w, &h, NULL);
    zoom_center = Core::Point2D(double(evt->x)/double(w), double(evt->y)/double(h));
  }

  if (evt->direction == GDK_SCROLL_UP) {
    zoom_level = clamp(zoom_level+increment, 0.0, 1.0);
  } else {
    zoom_level = clamp(zoom_level-increment, 0.0, 1.0);
  }

  renderer->set_zoom(get_zoom());
  renderer->set_zoom_center(zoom_center);

  gdk_window_invalidate_rect(evt->window, NULL, true);
  return true;
}

