#include <string>
#include <vector>
#include <iostream>
#include <gtkmm/main.h>
#include <gtkmm/entry.h>
#include <gtkmm/builder.h>
#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>
#include "../core/image.h"
#include "../core/scene.h"
#include <gtk/gtk.h>
#include "gtk_gui.h"
#include "viewer_widget.h"
#include "photogrammetry_window.h"
#include "image_chooser.h"

// For testing only
#include "../importers/insight3d.h"
#include <fstream>


using namespace std;
using namespace Core;
using boost::shared_ptr;

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


    #if GTK_CHECK_VERSION(3,0,0)
    app->run(*root_window, argc, argv);
    #else
    kit.run(*root_window);
    #endif

  }
}
