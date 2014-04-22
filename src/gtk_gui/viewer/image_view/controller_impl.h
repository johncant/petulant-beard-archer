#include "controller.h"
#include "boost/phoenix/core.hpp"
#include "boost/phoenix/bind.hpp"
#include "boost/algorithm/clamp.hpp"
#include <algorithm>
#include <math.h>

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
  widget_controllable(controllable),
  rectangle_start(0.0, 0.0),
  rectangle_end(0.0, 0.0),
  allow_rectangle_select(false),
  allow_point_creation_on_release(false)
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

template <class controllable_t, class renderer_t>
typename Controller<controllable_t, renderer_t>::Selection
Controller<controllable_t, renderer_t>::get_selection() {
  return selection;
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
  renderer->draw(image_controller->get_points_values(), allow_rectangle_select, rectangle_start, rectangle_end);
}

// Interaction events
template <class controllable_t, class renderer_t>
bool Controller<controllable_t, renderer_t>::on_button_press_event(GdkEventButton* evt) {

  using boost::phoenix::bind;
  using boost::phoenix::arg_names::_1;

  Core::Point2D pos = point_from_event(widget_controllable, evt);
  allow_point_creation_on_release = false;
  allow_rectangle_select = false;

  // Image coords
  Core::Point2D im_mouse_pos = renderer->as_image_coords(pos);

  if (evt->button == 1 && (evt->state & GDK_SHIFT_MASK || evt->state & GDK_CONTROL_MASK)) {

    PointRef select_point = image_controller->
    get_point_under_cursor(
      pos,
      renderer->get_reverse_marker_bounds(pos)
    );

    if (select_point) {
      select_point.params().m_is_selected = true;

      selection += select_point;
    }

  } else if (evt->button == 1) {
    // TODO - Select 1 point or create point

    drag_point = image_controller->
    get_point_under_cursor(
      pos,
      renderer->get_reverse_marker_bounds(pos)
    );
//bind(&PointViewParams::m_is_selected, val(
////
//    std::for_each(
//      selection.begin(),
//      selection.end(),
//      bind(static_cast< & (PointRef::*) () const >&PointRef::params, _1)
//    );

    if (drag_point) {
      Core::Point2D vp_drag_point(
        renderer->as_viewport_coords(
          Core::Point2D(
            drag_point->x,
            drag_point->y
          )
        )
      );

      drag_offset_x = vp_drag_point.x - pos.x;
      drag_offset_y = vp_drag_point.y - pos.y;

      for (Selection::iterator it = selection.begin(); it!= selection.end(); it++) {
        it->params().m_is_selected = false;
      }

      drag_point.params().m_is_selected = true;
      selection = drag_point;

    } else {
      allow_rectangle_select = true;
      allow_point_creation_on_release = true;
      rectangle_start = im_mouse_pos;
      std::cout << "Starting rectangle select" << std::endl;
      rectangle_end = im_mouse_pos;
    }

  }

}

template <class controllable_t, class renderer_t>
bool Controller<controllable_t, renderer_t>::on_button_release_event(GdkEventButton* evt) {

  Core::Point2D pos = point_from_event(widget_controllable, evt);
  Core::Point2D im_mouse_pos = renderer->as_image_coords(pos);

  if (drag_point) {
    drag_point.unassign();
  }

  if (allow_rectangle_select) {
    if (
      fabs(rectangle_start.x - im_mouse_pos.x) > 1e-4 &&
      fabs(rectangle_start.y - im_mouse_pos.y) > 1e-4
    ) {
      std::cout << "Finish rectangle select" << std::endl;
      for (
        Selection::iterator it = selection.begin();
        it != selection.end();
        it++
      ) {
        if (rectangle_selection.count(*it) == 0) {
          it->params().m_is_selected = false;
        }
      }
      selection = rectangle_selection;
    }
    allow_rectangle_select = false;
  }

  if (allow_point_creation_on_release) {
    allow_point_creation_on_release = false;
    image_controller->add_point(renderer->as_image_coords(pos));
    std::cout << "point added" << std::endl;
  }

  widget_controllable->trigger_redraw();
}

template <class controllable_t, class renderer_t>
bool Controller<controllable_t, renderer_t>::on_motion_notify_event(GdkEventMotion* evt) {
  Core::Point2D vp_mouse_pos = point_from_event(widget_controllable, evt);

  allow_point_creation_on_release = false;
  // TODO - Zoom center should be decided in here - render should be more dumb
  renderer->set_zoom_center(vp_mouse_pos);

  // Image coords
  Core::Point2D im_mouse_pos = renderer->as_image_coords(vp_mouse_pos);

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
    Core::Point2D new_drag_point(
      vp_mouse_pos.x + drag_offset_x,
      vp_mouse_pos.y + drag_offset_y
    );
    drag_point.move(renderer->as_image_coords(new_drag_point));
    allow_point_creation_on_release = false;
    allow_rectangle_select = false;
  } else if (allow_rectangle_select) {
    // Handle rectangle select
    rectangle_end = im_mouse_pos;

    std::cout << "Move during rectangle select" << std::endl;
    for (
      Selection::iterator it = rectangle_selection.begin();
      it != rectangle_selection.end();
      it++
    ) {
      if (selection.count(*it) == 0) {
        // This point is not selected. Make it look unselected.
        it->params().m_is_selected = false;
      }
    }

    std::vector<PointRef> selected_points =
      image_controller->get_points_in_rectangle(
        rectangle_start,
        rectangle_end
      );

    rectangle_selection.clear();
    rectangle_selection.insert(
      selected_points.begin(),
      selected_points.end()
    );

    for (
      Selection::iterator it = rectangle_selection.begin();
      it != rectangle_selection.end();
      it++
    ) {
      // Make it look selected.
      it->params().m_is_selected = true;
    }

  }

  widget_controllable->trigger_redraw();
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

  widget_controllable->trigger_redraw();
  return true;
}

}}}

#endif
