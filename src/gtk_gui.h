#ifndef __GTK_GUI_H__
#define __GTK_GUI_H__

#include <string>
#include <vector>
#include <iostream>
#include <gtkmm/window.h>
#include <gtkmm/entry.h>
#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>
#include <gtk/gtk.h>
#include "viewer.h"

namespace GtkGui {

  class ApplicationRootWindow : public Gtk::Window {
    static GType gtype;
    static Glib::ObjectBase *wrap_new (GObject *o) {
      if (gtk_widget_is_toplevel(GTK_WIDGET(o))) {
        return new ApplicationRootWindow(GTK_WINDOW(o));
      } else {
        return Gtk::manage(new ApplicationRootWindow (GTK_WINDOW(o)));
      }
    }

    public:
    Glib::RefPtr<Gtk::Builder> builder;
    Glib::RefPtr<Gtk::Application> app;

    ApplicationRootWindow() : Glib::ObjectBase("mywidget"), Gtk::Window() {
//      Glib::ObjectBase("Photogrammetry root window");
    }

    ApplicationRootWindow(GtkWindow *gobj) : Gtk::Window(gobj) {
    }


    void register_type () {

      if (gtype) return;

      ApplicationRootWindow dummy;

      GtkWidget *widget = GTK_WIDGET (dummy.gobj ());

      gtype = G_OBJECT_TYPE (widget);

      Glib::wrap_register (gtype, ApplicationRootWindow::wrap_new);
    }

    int main(int argc, char** argv) {

      app = Gtk::Application::create("org.photogrammetry");
      builder = Gtk::Builder::create_from_file("iface/window.glade");

      // Get the window to work
      Gtk::Window* root_window;
      builder->get_widget("root", root_window);

      // Test GtkGlExt - gtkglextmm doesn't support Gtk+-3.0 yet

      Gtk::DrawingArea *da;
      builder->get_widget("drawingarea_viewer", da);

      Viewer viewer(*this, *da);

      da->signal_configure_event().connect(sigc::mem_fun(viewer, &Viewer::on_configure));
      da->signal_draw().connect(sigc::mem_fun(viewer, &Viewer::on_expose));
      da->signal_realize().connect(sigc::mem_fun(viewer, &Viewer::on_realize));

      app->run(*root_window, argc, argv);

    }

  };

  GType ApplicationRootWindow::gtype = 0;
};

#endif // __GTK_GUI_H__
