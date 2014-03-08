#include "viewer.h"
#include "image_view_controller.h"
#include <GL/gl.h>
#include <GL/glx.h>
#include <gdk/gdkx.h>

namespace GtkGui {
  class ViewerImpl {
    friend class GtkGui::Viewer;
    GLXContext context;
    Colormap xcolormap;
    XVisualInfo *xvisual;
    GdkVisual *visual;
    ViewerImpl() :
      xvisual(0),
      visual(0)
    { }
  };
}


// TODO - remove me. I am for testing only:
#include "../core/image.h"
static boost::shared_ptr<Core::Image> test_image() {
  boost::shared_ptr<Core::Image> img(new Core::Image());
  img->path = "test.jpg";
  return img;
}

// Method called on GdkDrawable in 2.x and GdkWindow in 3.x
#if !GTK_CHECK_VERSION(3,0,0)
  #warning "Does opencv support GTK3 yet?"
  #define gdk_x11_window_get_xid(window) gdk_x11_drawable_get_xid(GDK_DRAWABLE(window))
#endif


GtkGui::Viewer::~Viewer() {
  GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
  Display *display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));

  glXDestroyContext(display, impl.context);
  XFreeColormap(display, impl.xcolormap);
  XFree(impl.xvisual);
  g_object_unref(G_OBJECT(impl.visual));

  delete &impl;
}

GtkGui::Viewer::Viewer(GtkDrawingArea *gobj, Glib::RefPtr<Gtk::Builder> builder) :
  Gtk::DrawingArea(gobj),
  impl(*new GtkGui::ViewerImpl()),
  controller(new GtkGui::ImageViewController(test_image()))
{
  this->set_events(Gdk::ALL_EVENTS_MASK);
}

GtkGui::Viewer::Viewer() :
  impl(*new GtkGui::ViewerImpl()),
  controller(new GtkGui::ImageViewController(test_image())) {
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

  // Set up OpenGL to work with X11
  window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
  std::cout << "window" << window << std::endl;

  if (!window) return;

  gtk_widget_set_double_buffered(GTK_WIDGET(this->gobj()), FALSE);

  display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));

  id = gdk_x11_window_get_xid(window);

  xscreen = DefaultScreen(display);
  impl.xvisual = glXChooseVisual(display, xscreen, attributes);

  screen = gdk_screen_get_default();
  impl.visual = gdk_x11_screen_lookup_visual(screen, impl.xvisual->visualid);
  root = RootWindow(display, xscreen);
  impl.xcolormap = XCreateColormap(display, root, impl.xvisual->visual, AllocNone);
  gtk_widget_set_visual(GTK_WIDGET(this->gobj()), impl.visual);
  impl.context = glXCreateContext(display, impl.xvisual, NULL, TRUE);

  free(impl.xvisual);

  gtk_widget_set_size_request(GTK_WIDGET(this->gobj()), 100, 100);

  // Hack to bind signal

  this->signal_button_press_event().connect(
    sigc::mem_fun(
      *boost::static_pointer_cast<GtkGui::ImageViewController>(controller),
      &GtkGui::ImageViewController::on_button_press_event
    )
  );

  this->signal_motion_notify_event().connect(
    sigc::mem_fun(
      *boost::static_pointer_cast<GtkGui::ImageViewController>(controller),
      &GtkGui::ImageViewController::on_motion_notify_event
    )
  );

  this->signal_scroll_event().connect(
    sigc::mem_fun(
      *boost::static_pointer_cast<GtkGui::ImageViewController>(controller),
      &GtkGui::ImageViewController::on_scroll
    )
  );

  if (glXMakeCurrent(display, id, impl.context) == TRUE) {
    std::cout << "context " << impl.context << std::endl;
    controller->realize(window);
  }

}



bool GtkGui::Viewer::on_configure2(GdkEventConfigure* const&) {

  GtkAllocation allocation;
  GdkWindow *window;
  Display *display;
  int id;

  std::cout << "configure event" << std::endl;
  std::cout << "visual" << impl.visual << std::endl;

  if (impl.visual) {

    window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
    display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));

    id = gdk_x11_window_get_xid(window);

    if (glXMakeCurrent(display, id, impl.context) == TRUE) {
      gtk_widget_get_allocation(GTK_WIDGET(this->gobj()), &allocation);
      controller->configure(allocation.width, allocation.height, window);
    }

  }

}

// Compatability wrappers
bool GtkGui::Viewer::on_expose_gtk2(GdkEventExpose* evt) {
  return on_expose1();
}

bool GtkGui::Viewer::on_expose_gtk3(const Cairo::RefPtr<Cairo::Context>&) {
  return on_expose1();
}

bool GtkGui::Viewer::on_expose1() {
  GdkWindow *window;
  Display *display;
  int id;

  std::cout << "expose event" << std::endl;
  if (impl.visual) {
    window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
    display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));

    id = gdk_x11_window_get_xid(window);

    if (glXMakeCurrent(display, id, impl.context) == TRUE) {
      controller->draw(window);
      glXSwapBuffers(display, id);
    }
  }

}


