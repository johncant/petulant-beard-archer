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
    typedef sigc::signal<void, boost::shared_ptr<Core::Image> > ImageSelectedSignal;
    ImageSelectedSignal m_signal_image_chosen;

    public:

    ImageChooser(GtkIconView* cobj, Glib::RefPtr<Gtk::Builder> builder);
    ~ImageChooser();
    void set_scene(boost::shared_ptr<Core::Scene> scene);
    void on_item_activated(const Gtk::TreeModel::Path& path);

    ImageSelectedSignal signal_image_chosen();

    class ImageListStore : public Gtk::ListStore {

      public:
      void add_entry(boost::shared_ptr<Core::Image> im);

      class Columns : public Gtk::TreeModel::ColumnRecord {
        public:

        Columns();

        Gtk::TreeModelColumn<int>  num_points;
        Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > pixbuf;
        Gtk::TreeModelColumn<Glib::ustring> name;
        Gtk::TreeModelColumn<boost::shared_ptr<Core::Image> > image;
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
