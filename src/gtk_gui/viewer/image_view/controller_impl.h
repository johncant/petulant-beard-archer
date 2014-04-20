#include "controller.h"
#include "boost/phoenix/core.hpp"
#include "boost/phoenix/bind.hpp"
#include "boost/algorithm/clamp.hpp"
#include <algorithm>

#ifndef __INSIDE_GTK_GUI_VIEWER_IMAGE_VIEW_CONTROLLER_H__
  #error "This file should never be included directly"
#else

//#include "boost/phoenix/stl/algorithm.hpp"
//#include "boost/phoenix/bind.hpp"

using boost::algorithm::clamp;

namespace GtkGui { namespace Viewer { namespace ImageView {

// Helper
template <class controllable_t, class event>
static Core::Point2D point_from_event(const controllable_t& c, event* e) {
  unsigned int w, h;

  w = c->get_width();
  h = c->get_height();

  return Core::Point2D(double(e->x)/double(w), double(e->y)/double(h));
}






// Impl

// *structors



template <class controllable_t, class renderer_t>
Controller<controllable_t, renderer_t>::Controller(
  boost::shared_ptr<controllable_t> controllable,
  boost::shared_ptr<ImageController> imc
) :
  image_controller(imc),
  renderer(new renderer_t(imc->image)),
  zoom_level(0),
  zoom_center(0.5, 0.5),
  highlighted_point(*imc),
  drag_point(*imc),
  widget_controllable(controllable)
{
  connect_signal_handlers();
}

template <class controllable_t, class renderer_t>
void Controller<controllable_t, renderer_t>::
  connect_signal_handlers() {

  signal_connections.push_back(
    widget_controllable->signal_button_press_event().connect(
      sigc::mem_fun(
        *this,
        &GtkGui::Viewer::ImageView::Controller<controllable_t, renderer_t>::on_button_press_event
      )
    )
  );

  signal_connections.push_back(
    widget_controllable->signal_button_release_event().connect(
      sigc::mem_fun(
        *this,
        &GtkGui::Viewer::ImageView::Controller<controllable_t, renderer_t>::on_button_release_event
      )
    )
  );

  signal_connections.push_back(
    widget_controllable->signal_motion_notify_event().connect(
      sigc::mem_fun(
        *this,
        &GtkGui::Viewer::ImageView::Controller<controllable_t, renderer_t>::on_motion_notify_event
      )
    )
  );

  signal_connections.push_back(
    widget_controllable->signal_scroll_event().connect(
      sigc::mem_fun(
        *this,
        &GtkGui::Viewer::ImageView::Controller<controllable_t, renderer_t>::on_scroll
      )
    )
  );

}

template <class controllable_t, class renderer_t>
void Controller<controllable_t, renderer_t>::disconnect_signal_handlers() {
  using boost::phoenix::bind;
  using boost::phoenix::arg_names::_1;

  std::for_each(
    signal_connections.begin(),
    signal_connections.end(),
    bind(&sigc::connection::disconnect, _1)
  );

}

template <class controllable_t, class renderer_t>
Controller<controllable_t, renderer_t>::~Controller() {
  disconnect_signal_handlers();
}

// Accessors

template <class controllable_t, class renderer_t>
double Controller<controllable_t, renderer_t>::get_zoom() {
  // TODO - CONFIG
  return exp(zoom_level*log(20.0));
}

// Events that come from Viewer
template <class controllable_t, class renderer_t>
void Controller<controllable_t, renderer_t>::realize() {
  widget_controllable->set_cursor(controllable_t::CROSSHAIR);
  renderer->realize();
}

template <class controllable_t, class renderer_t>
void Controller<controllable_t, renderer_t>::configure(unsigned int width, unsigned int height) {
  renderer->configure(width, height);
}

template <class controllable_t, class renderer_t>
void Controller<controllable_t, renderer_t>::draw() {
  renderer->draw(image_controller->get_points_values());
}

// Interaction events
template <class controllable_t, class renderer_t>
bool Controller<controllable_t, renderer_t>::on_button_press_event(GdkEventButton* evt) {

  Core::Point2D pos = point_from_event(widget_controllable, evt);

  if (evt->button == 1 && (evt->state & GDK_SHIFT_MASK)) {
    // TODO - box select and include selected points
  } else if (evt->button == 1 && (evt->state & GDK_CONTROL_MASK)) {
    // TODO - select or deselect individual other points
  } else if (evt->button == 1) {
    // TODO - Select 1 point or create point

    drag_point = image_controller->
    get_point_under_cursor(
      pos,
      renderer->get_reverse_marker_bounds(pos)
    );

  }

}

template <class controllable_t, class renderer_t>
bool Controller<controllable_t, renderer_t>::on_button_release_event(GdkEventButton* evt) {

  Core::Point2D pos = point_from_event(widget_controllable, evt);

  if (drag_point) {
    drag_point.unassign();
  } else {
    image_controller->add_point(renderer->as_image_coords(pos));
    std::cout << "point added" << std::endl;
  }

  gdk_window_invalidate_rect(evt->window, NULL, true);
}

template <class controllable_t, class renderer_t>
bool Controller<controllable_t, renderer_t>::on_motion_notify_event(GdkEventMotion* evt) {
  Core::Point2D vp_mouse_pos = point_from_event(widget_controllable, evt);

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

  // Handle point dragging
  if (drag_point) {
    Core::Point2D im_mouse_pos = renderer->as_image_coords(vp_mouse_pos);

    drag_point.move(im_mouse_pos);
  }

  // Redraw
  gdk_window_invalidate_rect(evt->window, NULL, true);
}

template <class controllable_t, class renderer_t>
bool Controller<controllable_t, renderer_t>::on_scroll(GdkEventScroll* evt) {
  // TODO - CONFIG
  double increment = 0.1;

  if (zoom_level <= 0.5*increment) {
    zoom_center = point_from_event(widget_controllable, evt);
  }

  if (evt->direction == GDK_SCROLL_UP) {
    zoom_level = boost::algorithm::clamp(zoom_level+increment, 0.0, 1.0);
  } else {
    zoom_level = boost::algorithm::clamp(zoom_level-increment, 0.0, 1.0);
  }

  renderer->set_zoom(get_zoom());
  renderer->set_zoom_center(zoom_center);

  gdk_window_invalidate_rect(evt->window, NULL, true);
  return true;
}

}}}

#endif
