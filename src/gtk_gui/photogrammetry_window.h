#ifndef __PHOTOGRAMMETRY_WINDOW_H__
#define __PHOTOGRAMMETRY_WINDOW_H__

#include <gtk/gtk.h>
#include <gtkmm/window.h>
#include <gtkmm/builder.h>




namespace GtkGui {
  class PhotogrammetryWindow : public Gtk::Window {
    static GType gtype;
    //static Glib::ObjectBase *wrap_new (GObject *o);
    public:

    PhotogrammetryWindow();
//    PhotogrammetryWindow(GtkWindow *gobj);
    PhotogrammetryWindow(GtkWindow *gobj, Glib::RefPtr<Gtk::Builder> builder);

//    void register_type ();
  };
}


#endif
