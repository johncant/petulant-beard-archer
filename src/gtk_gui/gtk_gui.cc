#include <string>
#include <vector>
#include <iostream>
#include <gtkmm/main.h>
#include <gtkmm/entry.h>
#include <gtkmm/builder.h>
#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>
#include <gtk/gtk.h>

#include "gtk_gui.h"
#include "viewer_widget.h"
#include "photogrammetry_window.h"

using namespace std;

namespace GtkGui {
  int main(int argc, char** argv) {

    // This needs to exist or Gtk::Builder will crash
    #if GTK_CHECK_VERSION(3,0,0)
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("org.photogrammetry");
    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("iface/window.glade");
    #else
    Gtk::Main kit(argc, argv);
    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("iface/window-gtk2.glade");
    #endif


    // Get the window to work
    GtkGui::PhotogrammetryWindow* root_window = 0;
    builder->get_widget_derived("root", root_window);

    // Test GtkGlExt - gtkglextmm doesn't support Gtk+-3.0 yet

    GtkGui::ViewerWidget *viewer = 0;
    builder->get_widget_derived("drawingarea_viewer", viewer);

    // Signal gets renamed in Gtk+-3.0
    #if GTK_CHECK_VERSION(3,0,0)
    viewer->signal_draw().connect(sigc::mem_fun(viewer, &ViewerWidget::on_expose_gtk3));
    #else
    viewer->signal_expose_event().connect(sigc::mem_fun(viewer, &ViewerWidget::on_expose_gtk2));
    #endif
    viewer->signal_configure_event().connect(sigc::mem_fun(viewer, &ViewerWidget::on_configure2));
    viewer->signal_realize().connect(sigc::mem_fun(viewer, &ViewerWidget::on_realize2));


    #if GTK_CHECK_VERSION(3,0,0)
    app->run(*root_window, argc, argv);
    #else
    kit.run(*root_window);
    #endif

  }
}
