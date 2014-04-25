#ifndef __PHOTOGRAMMETRY_WINDOW_H__
#define __PHOTOGRAMMETRY_WINDOW_H__

#include <gtk/gtk.h>
#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include "viewer_widget.h"




namespace GtkGui {
  class PhotogrammetryWindow : public Gtk::Window {
    static GType gtype;
    //static Glib::ObjectBase *wrap_new (GObject *o);
    protected:

    Glib::RefPtr<GtkGui::ViewerWidget> viewer;
    void show_correspondence(
      boost::shared_ptr<Core::Image> im1,
      boost::shared_ptr<Core::Image> im2
    );
    void show_image(boost::shared_ptr<Core::Image> im);
    public:

    PhotogrammetryWindow();
//    PhotogrammetryWindow(GtkWindow *gobj);
    PhotogrammetryWindow(GtkWindow *gobj, Glib::RefPtr<Gtk::Builder> builder);

    Glib::RefPtr<GtkGui::ViewerWidget> get_viewer_widget();
//    void register_type ();
  };
}


#endif
