#include "viewer_widget.h"
#include "viewer/image_view/controller.h"
#include "viewer/image_view/image_controller.h"
#include "viewer/widget_controllable.h"
#include <GL/gl.h>
#include <GL/glx.h>
#include <gdk/gdkx.h>

namespace GtkGui {
  class ViewerWidgetImpl {
    friend class GtkGui::ViewerWidget;
    GLXContext context;
    Colormap xcolormap;
    XVisualInfo *xvisual;
    GdkVisual *visual;
    bool realized, configured, exposed;
    ViewerWidgetImpl() :
      xvisual(0),
      visual(0),
      realized(false),
      configured(false),
      exposed(false)
    { }
  };
}


// Method called on GdkDrawable in 2.x and GdkWindow in 3.x
#if !GTK_CHECK_VERSION(3,0,0)
  #warning "Does opencv support GTK3 yet?"
  #define gdk_x11_window_get_xid(window) gdk_x11_drawable_get_xid(GDK_DRAWABLE(window))
#endif


GtkGui::ViewerWidget::~ViewerWidget() {
  GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
  Display *display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));

  glXDestroyContext(display, impl.context);
  XFreeColormap(display, impl.xcolormap);
  XFree(impl.xvisual);
  g_object_unref(G_OBJECT(impl.visual));

  delete &impl;
}

GtkGui::ViewerWidget::ViewerWidget(GtkDrawingArea *gobj, Glib::RefPtr<Gtk::Builder> builder) :
  Gtk::DrawingArea(gobj),
  impl(*new GtkGui::ViewerWidgetImpl())
{
  initialize();
}

GtkGui::ViewerWidget::ViewerWidget() :
  impl(*new GtkGui::ViewerWidgetImpl())
{
  initialize();
}

void GtkGui::ViewerWidget::initialize() {

  set_events(Gdk::ALL_EVENTS_MASK);

  // Signal gets renamed in Gtk+-3.0
  #if GTK_CHECK_VERSION(3,0,0)
  signal_draw().connect(sigc::mem_fun(*this, &ViewerWidget::on_expose_gtk3));
  #else
  signal_expose_event().connect(sigc::mem_fun(*this, &ViewerWidget::on_expose_gtk2));
  #endif
  signal_configure_event().connect(sigc::mem_fun(*this, &ViewerWidget::on_configure2));
  signal_realize().connect(sigc::mem_fun(*this, &ViewerWidget::on_realize2));

}

void GtkGui::ViewerWidget::on_realize2() {
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

  impl.realized = true;
  if (controller && glXMakeCurrent(display, id, impl.context) == TRUE) {
    controller->realize();
  }

}



bool GtkGui::ViewerWidget::on_configure2(GdkEventConfigure* const&) {

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

    impl.configured = true;
    if (controller && glXMakeCurrent(display, id, impl.context) == TRUE) {
      gtk_widget_get_allocation(GTK_WIDGET(this->gobj()), &allocation);
      controller->configure(allocation.width, allocation.height);
    }

  }

}

// Compatability wrappers
bool GtkGui::ViewerWidget::on_expose_gtk2(GdkEventExpose* evt) {
  return on_expose1();
}

bool GtkGui::ViewerWidget::on_expose_gtk3(const Cairo::RefPtr<Cairo::Context>&) {
  return on_expose1();
}

bool GtkGui::ViewerWidget::on_expose1() {
  GdkWindow *window;
  Display *display;
  int id;

  if (impl.visual) {
    window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
    display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));

    id = gdk_x11_window_get_xid(window);

    impl.exposed = true;
    if (controller && glXMakeCurrent(display, id, impl.context) == TRUE) {
      controller->draw();
      glXSwapBuffers(display, id);
    }
  }

}


void GtkGui::ViewerWidget::show_image(boost::shared_ptr<Core::Image> im) {
  typedef GtkGui::Viewer::WidgetControllable WidgetControllable;
  typedef GtkGui::Viewer::ImageView::Controller<WidgetControllable> Controller;
  typedef Controller::ImageController ImageController;

  boost::shared_ptr<WidgetControllable> wc(new WidgetControllable(*this));
  boost::shared_ptr<ImageController> ic(new ImageController(im));
  controller = boost::shared_ptr<Controller>(new Controller(wc, ic));

  // Wait until we're told to set up a context.
  if (!impl.realized) return;

  GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(this->gobj()));
  Display *display = gdk_x11_display_get_xdisplay(gdk_window_get_display(window));
  int id = gdk_x11_window_get_xid(window);

  if (glXMakeCurrent(display, id, impl.context) == TRUE) {

    // Bring controller up to date with our current state
    controller->realize();

    if (impl.configured) {
      int width, height;
      GtkAllocation allocation;

      gtk_widget_get_allocation(GTK_WIDGET(this->gobj()), &allocation);
      controller->configure(allocation.width, allocation.height);
    }

    if (impl.exposed) {
      controller->draw();
      glXSwapBuffers(display, id);
    }
  }
}

