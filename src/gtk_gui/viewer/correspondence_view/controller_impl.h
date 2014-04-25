#include "controller.h"
#include "boost/phoenix/core.hpp"
#include "boost/phoenix/bind.hpp"
#include "boost/algorithm/clamp.hpp"
#include <algorithm>
#include <math.h>

#ifndef __INSIDE_GTK_GUI_VIEWER_CORRESPONDENCE_VIEW_CONTROLLER_H__
  #error "This file should never be included directly"
#else

//#include "boost/phoenix/stl/algorithm.hpp"
//#include "boost/phoenix/bind.hpp"

using boost::algorithm::clamp;

namespace GtkGui { namespace Viewer { namespace CorrespondenceView {


// Impl

// *structors

template <class controllable_t, class renderer_t>
Controller<controllable_t, renderer_t>::Controller(
  boost::shared_ptr<controllable_t> controllable
) :
  renderer(new renderer_t()),
  widget_controllable(controllable)
{
  connect_signal_handlers();
}

template <class controllable_t, class renderer_t>
void Controller<controllable_t, renderer_t>::
  connect_signal_handlers() {

//  signal_connections.push_back(
//    widget_controllable->signal_button_press_event().connect(
//      sigc::mem_fun(
//        *this,
//        &GtkGui::Viewer::CorrespondenceView::Controller<controllable_t, renderer_t>::on_button_press_event
//      )
//    )
//  );
//
//  signal_connections.push_back(
//    widget_controllable->signal_button_release_event().connect(
//      sigc::mem_fun(
//        *this,
//        &GtkGui::Viewer::CorrespondenceView::Controller<controllable_t, renderer_t>::on_button_release_event
//      )
//    )
//  );
//
//  signal_connections.push_back(
//    widget_controllable->signal_motion_notify_event().connect(
//      sigc::mem_fun(
//        *this,
//        &GtkGui::Viewer::CorrespondenceView::Controller<controllable_t, renderer_t>::on_motion_notify_event
//      )
//    )
//  );
//
//  signal_connections.push_back(
//    widget_controllable->signal_scroll_event().connect(
//      sigc::mem_fun(
//        *this,
//        &GtkGui::Viewer::CorrespondenceView::Controller<controllable_t, renderer_t>::on_scroll
//      )
//    )
//  );

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
  // TODO - yuck
  std::vector<boost::tuple<std::vector<GtkGui::Viewer::ImageView::Renderer::tuple_pt_ud>, bool, Core::Point2D, Core::Point2D > > v;
  renderer->draw(v);
}

// Interaction events
//template <class controllable_t, class renderer_t>
//bool Controller<controllable_t, renderer_t>::on_button_press_event(GdkEventButton* evt) {
//}
//
//template <class controllable_t, class renderer_t>
//bool Controller<controllable_t, renderer_t>::on_button_release_event(GdkEventButton* evt) {
//}
//
//template <class controllable_t, class renderer_t>
//bool Controller<controllable_t, renderer_t>::on_motion_notify_event(GdkEventMotion* evt) {
//}
//
//template <class controllable_t, class renderer_t>
//bool Controller<controllable_t, renderer_t>::on_scroll(GdkEventScroll* evt) {
//}
}}}

#endif
