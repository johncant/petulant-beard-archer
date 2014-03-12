#ifndef __IMAGE_CHOOSER_H__
#define __IMAGE_CHOOSER_H__

#include <gtkmm/iconview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/builder.h>
#include <gdkmm/pixbuf.h>
#include <boost/shared_ptr.hpp>
#include "../core/scene.h"
#include "../core/image.h"


namespace GtkGui {
  class ImageChooser : public Gtk::IconView {
    public:
    ImageChooser(GtkIconView* cobj, Glib::RefPtr<Gtk::Builder> builder);
    ~ImageChooser();

    void set_scene(boost::shared_ptr<Core::Scene> scene);

    class ImageListStore : public Gtk::ListStore {

      public:
      void add_entry(boost::shared_ptr<Core::Image> im);

      class Columns : public Gtk::TreeModel::ColumnRecord {
        public:

        Columns();

        Gtk::TreeModelColumn<int>  num_points;
        Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > pixbuf;
        Gtk::TreeModelColumn<Glib::ustring> name;
      };

      public: // Less public
      typedef Gtk::ListStore::Row Row;

      static Glib::RefPtr<ImageListStore> create();
      ImageListStore(ImageListStore::Columns &cols);


      static Columns columns;
    };

  };
}

#endif
