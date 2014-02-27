#ifndef __GTK_GUI_VIEWER_H__
#define __GTK_GUI_VIEWER_H__

#include <gtkmm/drawingarea.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <boost/shared_ptr.hpp>

#include <gdk/gdkx.h>
#include "gtk_gui.h"
#include "renderer.h"

namespace GtkGui {
  class Viewer : public Gtk::DrawingArea {

    // This class doesn't use GtkGlExt or GtkGlarea since they don't support GTK+-3.0.
    // Leaks memory like your grandmother.
    // I consider myself lucky to have got this to work! :)
    // https://www.mail-archive.com/gtk-devel-list@gnome.org/msg16579.html

    public:

    ~Viewer();
    Viewer(GtkDrawingArea *gobj, Glib::RefPtr<Gtk::Builder> builder);
    Viewer();

    void on_realize2();
    bool on_configure2(GdkEventConfigure* const&);
    bool on_expose_gtk3(const Cairo::RefPtr<Cairo::Context>&);
    bool on_expose_gtk2(GdkEventExpose* evt);
    bool on_expose1();

    private:
    GLXContext context;
    Colormap xcolormap;
    XVisualInfo *xvisual;
    GdkVisual *visual;
    boost::shared_ptr<GtkGui::Renderer> renderer;
  };
}

#endif // __GTK_GUI_VIEWER_H__
