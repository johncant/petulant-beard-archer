#include "gtk_gui.h"
#include <gtkmm/builder.h>

namespace GtkGui {
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
}
