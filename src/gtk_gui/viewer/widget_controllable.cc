#include "widget_controllable.h"
#include <gdk/gdkcursor.h>
#include <gdk/gdkwindow.h>


namespace GtkGui { namespace Viewer {

WidgetControllable::WidgetControllable(ViewerWidget& w) : widget(w) {
}

unsigned int WidgetControllable::get_width() const {
  gint w;
  gdk_window_get_geometry(gtk_widget_get_window(GTK_WIDGET(widget.gobj())), NULL, NULL, &w, NULL, NULL);
  return w;
}

unsigned int WidgetControllable::get_height() const {
  gint h;
  gdk_window_get_geometry(gtk_widget_get_window(GTK_WIDGET(widget.gobj())), NULL, NULL, NULL, &h, NULL);
  return h;
}

void WidgetControllable::trigger_redraw() {
  gdk_window_invalidate_rect(gtk_widget_get_window(GTK_WIDGET(widget.gobj())), NULL, true);
}

void WidgetControllable::set_cursor(const CursorType &ct) {
  GdkCursor *cursor;
  GdkWindow *window;

  window = gtk_widget_get_window(GTK_WIDGET(widget.gobj()));
  cursor = gdk_cursor_new((GdkCursorType) ct);
  gdk_window_set_cursor(window, cursor);
}

// Feel free to add more events here...
Glib::SignalProxy1<bool, GdkEventCrossing*> WidgetControllable::signal_enter_notify_event() {
  return widget.signal_enter_notify_event();
}

Glib::SignalProxy1<bool, GdkEventCrossing*> WidgetControllable::signal_leave_notify_event() {
  return widget.signal_leave_notify_event();
}

Glib::SignalProxy1< bool, GdkEventButton*> WidgetControllable::signal_button_press_event () {
  return widget.signal_button_press_event();
}

Glib::SignalProxy1< bool, GdkEventButton*> WidgetControllable::signal_button_release_event () {
  return widget.signal_button_release_event();
}

Glib::SignalProxy1< bool, GdkEventScroll*> WidgetControllable::signal_scroll_event() {
  return widget.signal_scroll_event();
}

Glib::SignalProxy1< bool, GdkEventMotion*> WidgetControllable::signal_motion_notify_event() {
  return widget.signal_motion_notify_event();
}

}}

