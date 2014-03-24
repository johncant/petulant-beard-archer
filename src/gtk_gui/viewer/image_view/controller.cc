#include "controller.h"
#include "boost/phoenix.hpp"
#include "boost/algorithm/clamp.hpp"
#include <algorithm>

//#include "boost/phoenix/stl/algorithm.hpp"
//#include "boost/phoenix/bind.hpp"

using boost::algorithm::clamp;
namespace phx {
  using namespace boost::phoenix;
  using namespace boost::phoenix::arg_names;
}
using namespace Core;

namespace GtkGui { namespace Viewer { namespace ImageView {

// Helper
template <class event>
static Point2D point_from_event(event* e) {
  gint w, h;

  gdk_window_get_geometry(e->window, NULL, NULL, &w, &h, NULL);

  return Point2D(double(e->x)/double(w), double(e->y)/double(h));
}






// Impl

// *structors
Controller::Controller(Gtk::Widget& parent, boost::shared_ptr<Image> im) :
  image_controller(new ImageController(im)),
  renderer(new Renderer(im)),
  zoom_level(0),
  zoom_center(0.5, 0.5)
{
  connect_signal_handlers(parent);
  cursor_crosshair = gdk_cursor_new(GDK_CROSSHAIR);
}

void Controller::connect_signal_handlers(Gtk::Widget &parent) {

  signal_connections.push_back(
    parent.signal_button_press_event().connect(
      sigc::mem_fun(
        *this,
        &GtkGui::Viewer::ImageView::Controller::on_button_press_event
      )
    )
  );

  signal_connections.push_back(
    parent.signal_motion_notify_event().connect(
      sigc::mem_fun(
        *this,
        &GtkGui::Viewer::ImageView::Controller::on_motion_notify_event
      )
    )
  );

  signal_connections.push_back(
    parent.signal_scroll_event().connect(
      sigc::mem_fun(
        *this,
        &GtkGui::Viewer::ImageView::Controller::on_scroll
      )
    )
  );

}

void Controller::disconnect_signal_handlers() {

  std::for_each(
    signal_connections.begin(),
    signal_connections.end(),
    phx::bind(&sigc::connection::disconnect, phx::_1)
  );

}

Controller::~Controller() {
  disconnect_signal_handlers();
}

// Accessors

double Controller::get_zoom() {
  // TODO - CONFIG
  return exp(zoom_level*log(20.0));
}

// Events that come from Viewer
void Controller::realize(GdkWindow* window) {
  renderer->realize();
  gdk_window_set_cursor(window, cursor_crosshair);
}

void Controller::configure(unsigned int width, unsigned int height, GdkWindow *window) {
  renderer->configure(width, height);
}

void Controller::draw(GdkWindow *window) {
  renderer->draw();
}

// Interaction events
bool Controller::on_button_press_event(GdkEventButton* evt) {

  Point2D pos = point_from_event(evt);

  if (evt->button == 1 && (evt->state & GDK_SHIFT_MASK)) {
    // TODO - box select and include selected points
  } else if (evt->button == 1 && (evt->state & GDK_CONTROL_MASK)) {
    // TODO - select or deselect individual other points
  } else if (evt->button == 1) {
    // TODO - Select 1 point or create point
    image_controller->add_point(renderer->as_image_coords(pos));
    std::cout << "point added" << std::endl;
  }

  gdk_window_invalidate_rect(evt->window, NULL, true);
  
//  Glib::RefPtr<Gtk::widget> widget = Glib::wrap(evt->window->get_user_data());
//  widget->signal_button_release_event()
}

bool Controller::on_motion_notify_event(GdkEventMotion* evt) {
  zoom_center = point_from_event(evt);
  renderer->set_zoom_center(zoom_center);

  gdk_window_invalidate_rect(evt->window, NULL, true);
}

bool Controller::on_scroll(GdkEventScroll* evt) {
  // TODO - CONFIG
  double increment = 0.1;

  if (zoom_level <= 0.5*increment) {
    zoom_center = point_from_event(evt);
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

}}}
