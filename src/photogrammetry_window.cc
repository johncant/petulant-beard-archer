#include "photogrammetry_window.h"
#include <gtkmm/builder.h>

// TODO if we want this to be usable in glade
//Glib::ObjectBase GtkGui::PhotogrammetryWindow::*wrap_new (GObject *o) {
//  if (gtk_widget_is_toplevel(GTK_WIDGET(o))) {
//    return new GtkGui::PhotogrammetryWindow(GTK_WINDOW(o));
//  } else {
//    return Gtk::manage(new GtkGui::PhotogrammetryWindow (GTK_WINDOW(o)));
//  }
//}

GtkGui::PhotogrammetryWindow::PhotogrammetryWindow() : Glib::ObjectBase("photogrammetry_window"), Gtk::Window() {
}

//GtkGui::PhotogrammetryWindow::PhotogrammetryWindow(GtkWindow *gobj) : Gtk::Window(gobj) {
//}

GtkGui::PhotogrammetryWindow::PhotogrammetryWindow(GtkWindow *gobj, Glib::RefPtr<Gtk::Builder> builder) : Gtk::Window(gobj) {
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
