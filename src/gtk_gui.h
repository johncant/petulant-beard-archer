#include <string>
#include <vector>
#include <gtkmm/window.h>
#include <gtkmm/builder.h>

class GtkGui {
  Gtk::Window mainwindow;
  Glib::RefPtr<Gtk::Builder> builder;
  Glib::RefPtr<Gtk::Application> app;

  public:
  GtkGui() : builder() {
  }

  int main(int argc, char** argv) {

    app = Gtk::Application::create("org.photogrammetry");
    builder = Gtk::Builder::create_from_file("iface/window.glade");

    Gtk::Window* root;

    builder->get_widget("root", root);

    app->run(*root, argc, argv);
  }
};
