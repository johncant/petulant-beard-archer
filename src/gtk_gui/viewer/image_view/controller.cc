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



Controller::Controller(Gtk::Widget& parent, boost::shared_ptr<ImageController> imc) :
  image_controller(imc),
  renderer(new Renderer(imc->image)),
  zoom_level(0),
  zoom_center(0.5, 0.5),
  highlighted_point(*imc)
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
  renderer->draw(image_controller->get_points_values());
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
  Core::Point2D vp_mouse_pos = point_from_event(evt);

  // TODO - Zoom center should be decided in here - render should be more dumb
  renderer->set_zoom_center(vp_mouse_pos);

  // Change highlight

  if (highlighted_point) {
    highlighted_point.params().m_is_highlighted = false;
  }

  highlighted_point = image_controller->
  get_point_under_cursor(
    vp_mouse_pos,
    renderer->get_reverse_marker_bounds(vp_mouse_pos)
  );

//  std::cout << highlighted_point.id << std::endl;
  if (highlighted_point) {
    highlighted_point.params().m_is_highlighted = true;
  }

  // Redraw
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
