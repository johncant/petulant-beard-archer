#include <fstream>
#include <gtkmm/builder.h>
#include "../core/scene.h"
#include "photogrammetry_window.h"
#include "image_chooser.h"
#include "viewer_widget.h"

// Testing only
#include "../importers/insight3d.h"

using boost::shared_ptr;
using Glib::RefPtr;
using namespace Core;

GtkGui::PhotogrammetryWindow::PhotogrammetryWindow() : Glib::ObjectBase("photogrammetry_window"), Gtk::Window() {
}

GtkGui::PhotogrammetryWindow::PhotogrammetryWindow(GtkWindow *gobj, Glib::RefPtr<Gtk::Builder> builder) : Gtk::Window(gobj) {

  GtkGui::ViewerWidget *viewer = 0;
  builder->get_widget_derived("drawingarea_viewer", viewer);
  this->viewer = RefPtr<GtkGui::ViewerWidget>(viewer);

  // Add a test scene
  std::ifstream file("enta4.i3d", std::fstream::in);
  Importers::Insight3dImporter i(file, std::cout);

  shared_ptr<Scene> new_scene(i.import());

  shared_ptr<Image> test_image(new Image());
  test_image->path = "test.jpg";
  new_scene->add_image(test_image);

  // Image chooser
  GtkGui::ImageChooser *image_chooser = 0;
  builder->get_widget_derived("iconview_image_chooser", image_chooser);

  image_chooser->signal_image_chosen().connect(
    sigc::mem_fun(
      *this,
      &GtkGui::PhotogrammetryWindow::show_image
    )
  );

  image_chooser->set_scene(new_scene);

}

void GtkGui::PhotogrammetryWindow::show_image(shared_ptr<Core::Image> im) {
  viewer->show_image(im);
}

RefPtr<GtkGui::ViewerWidget> GtkGui::PhotogrammetryWindow::get_viewer_widget() {
  return viewer;
}


//void GtkGui::PhotogrammetryWindow::register_type () {
//  if (gtype) return;
//
//  PhotogrammetryWindow dummy;
//
//  GtkWidget *widget = GTK_WIDGET (dummy.gobj ());
//
//  gtype = G_OBJECT_TYPE (widget);
//
//  Glib::wrap_register (gtype, PhotogrammetryWindow::wrap_new);
//}
//
//GType GtkGui::PhotogrammetryWindow::gtype = 0;
