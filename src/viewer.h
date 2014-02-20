#ifndef __GTK_GUI_VIEWER_H__
#define __GTK_GUI_VIEWER_H__

#include <gtkmm/drawingarea.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <gdk/gdkx.h>
#include "gtk_gui.h"

namespace GtkGui {
  class Viewer : public Gtk::DrawingArea {

    // This class doesn't use GtkGlExt or GtkGlarea since they don't support GTK+-3.0.
    // Leaks memory like your grandmother.
    // I consider myself lucky to have got this to work! :)
    // https://www.mail-archive.com/gtk-devel-list@gnome.org/msg16579.html

    public:
    GLXContext context;
    Colormap xcolormap;
    XVisualInfo *xvisual;
    GdkVisual *visual;

    ~Viewer() {
      GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
      Display *display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));

      glXDestroyContext(display, context);
      XFreeColormap(display, xcolormap);
      XFree(xvisual);
      g_object_unref(G_OBJECT(visual));

    }

    Viewer(GtkDrawingArea *gobj, Glib::RefPtr<Gtk::Builder> builder) : Gtk::DrawingArea(gobj), xvisual(0), visual(0) {
    }

    Viewer() : xvisual(0), visual(0) {
    }

    void on_realize2() {
      //Gtk::DrawingAreaon_realize(this);
      GtkWidget *root_window;
      GdkWindow *window;
      GdkScreen *screen;
      Display *display;
      Window root;
      int xscreen;
      int id;
      int attributes[] = { GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1,
          GLX_BLUE_SIZE, 1, GLX_DOUBLEBUFFER, True, GLX_DEPTH_SIZE, 12,
          None
      };

      std::cout << "Realize event" << std::endl;

      window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
      std::cout << "window" << window << std::endl;

      if (!window) return;

      gtk_widget_set_double_buffered(GTK_WIDGET(this->gobj()), FALSE);

      display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));
      id = gdk_x11_window_get_xid(window);

      xscreen = DefaultScreen(display);
      xvisual = glXChooseVisual(display, xscreen, attributes);

      screen = gdk_screen_get_default();
      visual = gdk_x11_screen_lookup_visual(screen, xvisual->visualid);
      root = RootWindow(display, xscreen);
      xcolormap = XCreateColormap(display, root, xvisual->visual, AllocNone);
      gtk_widget_set_visual(GTK_WIDGET(this->gobj()), visual);
      context = glXCreateContext(display, xvisual, NULL, TRUE);

      free(xvisual);

      gtk_widget_set_size_request(GTK_WIDGET(this->gobj()), 100, 100);


      if (glXMakeCurrent(display, id, context) == TRUE) {
          glEnable(GL_DEPTH_TEST);
          glDepthFunc(GL_LEQUAL);
          glEnable(GL_CULL_FACE);
          glCullFace(GL_BACK);
          glDisable(GL_DITHER);
          glShadeModel(GL_SMOOTH);
      }

    }


    bool on_configure2(GdkEventConfigure* const&) {

      GtkAllocation allocation;
      GdkWindow *window;
      Display *display;
      int id;

      std::cout << "configure event" << std::endl;
      std::cout << "visual" << visual << std::endl;

      if (visual) {

        window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
        display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));
        id = gdk_x11_window_get_xid(window);

        if (glXMakeCurrent(display, id, context) == TRUE) {
            gtk_widget_get_allocation(GTK_WIDGET(this->gobj()), &allocation);
            glViewport(0, 0, allocation.width, allocation.height);
        }

      }

    }


    bool on_expose2(const Cairo::RefPtr<Cairo::Context>&) {
      GdkWindow *window;
      Display *display;
      int id;

      std::cout << "expose event" << std::endl;
      if (visual) {
        window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
        display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));
        id = gdk_x11_window_get_xid(window);

        if (glXMakeCurrent(display, id, context) == TRUE) {
            glClear(GL_DEPTH_BUFFER_BIT);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, 100, 100, 0, -1, 1);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            glColor3f(1, 1, 1);
            glBegin(GL_TRIANGLES);
            glVertex2f(10, 10);
            glVertex2f(10, 90);
            glVertex2f(90, 90);
            glEnd();

            glXSwapBuffers(display, id);
        }
      }
    }

  };

}

#endif // __GTK_GUI_VIEWER_H__
