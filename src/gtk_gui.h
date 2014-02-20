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

  class PhotogrammetryWindow : public Gtk::Window {
    static GType gtype;
    static Glib::ObjectBase *wrap_new (GObject *o) {
      if (gtk_widget_is_toplevel(GTK_WIDGET(o))) {
        return new PhotogrammetryWindow(GTK_WINDOW(o));
      } else {
        return Gtk::manage(new PhotogrammetryWindow (GTK_WINDOW(o)));
      }
    }

    public:
    Glib::RefPtr<Gtk::Builder> builder;
    Glib::RefPtr<Gtk::Application> app;

    PhotogrammetryWindow() : Glib::ObjectBase("mywidget"), Gtk::Window() {
//      Glib::ObjectBase("Photogrammetry root window");
    }

    PhotogrammetryWindow(GtkWindow *gobj) : Gtk::Window(gobj) {
    }

    PhotogrammetryWindow(GtkWindow *gobj, Glib::RefPtr<Gtk::Builder> builder) : Gtk::Window(gobj) {
    }


    void register_type () {

      if (gtype) return;

      PhotogrammetryWindow dummy;

      GtkWidget *widget = GTK_WIDGET (dummy.gobj ());

      gtype = G_OBJECT_TYPE (widget);

      Glib::wrap_register (gtype, PhotogrammetryWindow::wrap_new);
    }


  };

  int main(int argc, char** argv) {

    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("org.photogrammetry");
    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("iface/window.glade");

    // Get the window to work
    GtkGui::PhotogrammetryWindow* root_window = 0;
    builder->get_widget_derived("root", root_window);

    // Test GtkGlExt - gtkglextmm doesn't support Gtk+-3.0 yet

    GtkGui::Viewer *viewer = 0;
    builder->get_widget_derived("drawingarea_viewer", viewer);

    viewer->signal_configure_event().connect(sigc::mem_fun(viewer, &Viewer::on_configure2));
    viewer->signal_draw().connect(sigc::mem_fun(viewer, &Viewer::on_expose2));
    viewer->signal_realize().connect(sigc::mem_fun(viewer, &Viewer::on_realize2));

    app->run(*root_window, argc, argv);

  }

  GType PhotogrammetryWindow::gtype = 0;
};

#endif // __GTK_GUI_H__
