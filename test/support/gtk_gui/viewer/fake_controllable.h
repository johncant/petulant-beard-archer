#ifndef __GTK_GUI_VIEWER_IMAGE_VIEW_FAKE_CONTROLLABLE_H__
#define __GTK_GUI_VIEWER_IMAGE_VIEW_FAKE_CONTROLLABLE_H__

#include <sigc++/sigc++.h>
#include <gdk/gdkevents.h>


namespace GtkGui {
  namespace Viewer {

    class FakeControllable {
      public:
      enum CursorType {
        CROSSHAIR=0xefffffff
      };

      void set_cursor(const CursorType &ct);

      unsigned int get_width() const;
      unsigned int get_height() const;

      void trigger_redraw();

      sigc::signal<bool, GdkEventCrossing*> m_signal_enter_notify_event;
      sigc::signal<bool, GdkEventMotion*> m_signal_motion_notify_event;
      sigc::signal<bool, GdkEventButton*> m_signal_button_press_event;
      sigc::signal<bool, GdkEventButton*> m_signal_button_release_event;
      sigc::signal<bool, GdkEventScroll*> m_signal_scroll_event;

      sigc::signal<bool, GdkEventCrossing*> signal_enter_notify_event();
      sigc::signal<bool, GdkEventMotion*> signal_motion_notify_event();
      sigc::signal<bool, GdkEventButton*> signal_button_press_event();
      sigc::signal<bool, GdkEventButton*> signal_button_release_event();
      sigc::signal<bool, GdkEventScroll*> signal_scroll_event();
    };


  }
}

#endif
