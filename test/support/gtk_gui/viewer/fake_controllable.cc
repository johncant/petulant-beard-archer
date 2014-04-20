#include "fake_controllable.h"

namespace GtkGui { namespace Viewer {

unsigned int FakeControllable::get_width() const {
  return 200;
}

unsigned int FakeControllable::get_height() const {
  return 200;
}

void trigger_redraw() {
  // No need to do anything here :)
}

void FakeControllable::set_cursor(const CursorType &ct) {
  // No need to do anything here :)
}

sigc::signal<bool, GdkEventCrossing*> FakeControllable::signal_enter_notify_event() {
  return m_signal_enter_notify_event;
}

sigc::signal<bool, GdkEventMotion*> FakeControllable::signal_motion_notify_event() {
  return m_signal_motion_notify_event;
}

sigc::signal<bool, GdkEventButton*> FakeControllable::signal_button_press_event() {
  return m_signal_button_press_event;
}

sigc::signal<bool, GdkEventButton*> FakeControllable::signal_button_release_event() {
  return m_signal_button_release_event;
}

sigc::signal<bool, GdkEventScroll*> FakeControllable::signal_scroll_event() {
  return m_signal_scroll_event;
}

}}
