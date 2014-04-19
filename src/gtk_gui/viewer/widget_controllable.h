#ifndef __GTK_GUI_VIEWER_WIDGET_CONTROLLABLE_H__
#define __GTK_GUI_VIEWER_WIDGET_CONTROLLABLE_H__

#include "../viewer_widget.h"
#include "gdk/gdkcursor.h"

namespace GtkGui {
  namespace Viewer {

    class WidgetControllable {

      // This is something that can be controlled by one of the
      // controllers here. During testing we can replace this so
      // we can test the controller without requiring GTK

      friend class GtkGui::ViewerWidget;
      ViewerWidget &widget;
      WidgetControllable(ViewerWidget& w);

      public:
      enum CursorType {
        CROSSHAIR = GDK_CROSSHAIR
      };

      unsigned int get_width() const;
      unsigned int get_height() const;

      void trigger_redraw();

      void set_cursor(const CursorType &ct);
      Glib::SignalProxy1<bool, GdkEventCrossing*> signal_enter_notify_event();
      Glib::SignalProxy1<bool, GdkEventCrossing*> signal_leave_notify_event();
      Glib::SignalProxy1< bool, GdkEventButton*> signal_button_press_event ();
      Glib::SignalProxy1< bool, GdkEventButton*> signal_button_release_event ();
      Glib::SignalProxy1< bool, GdkEventScroll*> signal_scroll_event();
      Glib::SignalProxy1< bool, GdkEventMotion*> signal_motion_notify_event();
    };
  }
}


#endif
