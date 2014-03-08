#ifndef __GTK_GUI_VIEWER_H__
#define __GTK_GUI_VIEWER_H__

#include <gtkmm/drawingarea.h>
#include <gtkmm/builder.h>
#include <gtk/gtk.h>

// This can go - TODO - move Controller into impl
#include <stdlib.h>
#include <boost/shared_ptr.hpp>

#include "gtk_gui.h"
#include "viewer/renderer.h"
#include "viewer/controller.h"

namespace GtkGui {
  class ViewerWidgetImpl;

  class ViewerWidget : public Gtk::DrawingArea {

    // This class doesn't use GtkGlExt or GtkGlarea since they don't support GTK+-3.0.
    // Leaks memory like your grandmother.
    // I consider myself lucky to have got this to work! :)
    // https://www.mail-archive.com/gtk-devel-list@gnome.org/msg16579.html

    public:

    ~ViewerWidget();
    ViewerWidget(GtkDrawingArea *gobj, Glib::RefPtr<Gtk::Builder> builder);
    ViewerWidget();

    void on_realize2();
    bool on_configure2(GdkEventConfigure* const&);
    bool on_expose_gtk3(const Cairo::RefPtr<Cairo::Context>&);
    bool on_expose_gtk2(GdkEventExpose* evt);
    bool on_expose1();

    private:
    GtkGui::ViewerWidgetImpl& impl;
    boost::shared_ptr<GtkGui::Viewer::Controller> controller;
  };
}

#endif // __GTK_GUI_VIEWER_H__