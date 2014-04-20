#include "image_chooser.h"
#include "../core/scene.h"
#include "../core/image.h"
#include <boost/phoenix.hpp>
#include <algorithm>

using boost::shared_ptr;
using namespace std;
using namespace Core;
namespace phx {
  using namespace boost::phoenix::arg_names;
  using namespace boost::phoenix;
}

GtkGui::ImageChooser::ImageListStore::Columns
GtkGui::ImageChooser::ImageListStore::columns;

namespace GtkGui {

// ImageChooser impl

ImageChooser::ImageChooser(GtkIconView* gobj, Glib::RefPtr<Gtk::Builder> builder) : Gtk::IconView(gobj) {
  set_text_column(ImageListStore::columns.name);
  set_pixbuf_column(ImageListStore::columns.pixbuf);
}

ImageChooser::~ImageChooser() {
}

void ImageChooser::on_item_activated(const Gtk::TreeModel::Path& path) {
  std::cout << "activated" << std::endl;

  Glib::RefPtr<Gtk::TreeModel> model = get_model();
  Gtk::TreeModel::iterator iter = model->get_iter(path);
  Gtk::TreeModel::Row row = *iter;

  m_signal_image_chosen.emit(row[ImageChooser::ImageListStore::columns.image]);
}

ImageChooser::ImageSelectedSignal GtkGui::ImageChooser::signal_image_chosen() {
  return m_signal_image_chosen;
}

void ImageChooser::set_scene(shared_ptr<Scene> scene) {
  Glib::RefPtr<ImageListStore> store = ImageListStore::create();  for_each(scene->images.begin(), scene->images.end(),
    phx::bind(&ImageListStore::add_entry, store.operator->(), phx::_1)
  );

  set_model(store);
}

ImageChooser::ImageListStore::ImageListStore(ImageChooser::ImageListStore::Columns& cols) :
  Gtk::ListStore(cols) {
}

Glib::RefPtr<ImageChooser::ImageListStore> ImageChooser::ImageListStore::create() {
  return Glib::RefPtr<ImageChooser::ImageListStore>(new ImageListStore(columns));
}

// ImageListStore impl
ImageChooser::ImageListStore::Columns::Columns() {
  add(num_points);
  add(pixbuf);
  add(name);
  add(image);
}

void ImageChooser::ImageListStore::add_entry(shared_ptr<Image> im) {
  cout << "Adding image " << im->path << endl;
  Row row = *(append());
  row[columns.num_points] = im->points.size();
  row[columns.pixbuf]     = Gdk::Pixbuf::create_from_file(im->path, 32, 32, true);
  row[columns.name]       = im->name;
  row[columns.image]      = im;
}



} // GtkGui
