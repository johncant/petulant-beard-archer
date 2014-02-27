#include "viewer.h"
#include "blank_renderer.h"

// Method called on GdkDrawable in 2.x and GdkWindow in 3.x
#if !GTK_CHECK_VERSION(3,0,0)
#error "Whoops"
#define gdk_x11_window_get_xid(window) gdk_x11_drawable_get_xid(GDK_GRAWABLE(window))
#endif


GtkGui::Viewer::~Viewer() {
  GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
  Display *display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));

  glXDestroyContext(display, context);
  XFreeColormap(display, xcolormap);
  XFree(xvisual);
  g_object_unref(G_OBJECT(visual));
}

GtkGui::Viewer::Viewer(GtkDrawingArea *gobj, Glib::RefPtr<Gtk::Builder> builder) : Gtk::DrawingArea(gobj), xvisual(0), visual(0), renderer(new GtkGui::BlankRenderer()) {
}

GtkGui::Viewer::Viewer() : xvisual(0), visual(0), renderer(new GtkGui::BlankRenderer()) {
}

void GtkGui::Viewer::on_realize2() {
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
    renderer->realize();
  }

}



bool GtkGui::Viewer::on_configure2(GdkEventConfigure* const&) {

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
      renderer->configure(allocation.width, allocation.height);
    }

  }

}


bool GtkGui::Viewer::on_expose2(const Cairo::RefPtr<Cairo::Context>&) {
  GdkWindow *window;
  Display *display;
  int id;

  std::cout << "expose event" << std::endl;
  if (visual) {
    window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
    display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));

    id = gdk_x11_window_get_xid(window);

    if (glXMakeCurrent(display, id, context) == TRUE) {
      renderer->draw();
      glXSwapBuffers(display, id);
    }
  }

}

