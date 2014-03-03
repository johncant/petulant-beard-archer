#include "viewer.h"
#include "image_view_controller.h"
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
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

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);


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
  #warning "Does GtkGLext or GtkGLArea support GTK3 yet?"
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
  controller(new GtkGui::ImageViewController(test_image())) {
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
//  impl.context = glXCreateContext(display, impl.xvisual, NULL, TRUE);

  // Create context, specifying required OpenGL version:

  static glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
  glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
  glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB");

  // FBConfigs were added in GLX version 1.3.
  int glx_major, glx_minor;
  if ( !glXQueryVersion( display, &glx_major, &glx_minor ) || 
       ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
  {
    std::cout << "GLX Version >= 1.4 required" << std::endl;
    // TODO
    exit(1);
  }

  // Check for function glXCreateContextAttribsARB
  if (!glXCreateContextAttribsARB) {
    std::cout << "glXCreateContextAttribsARB not found." << std::endl;
    std::cout << "Your machine does not support GLX >= 1.4" << std::endl;

    // TODO
    exit(1);
  }

  static int visual_attribs[] = {
    GLX_X_RENDERABLE    , True,
    GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
    GLX_RENDER_TYPE     , GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
    GLX_RED_SIZE        , 8,
    GLX_GREEN_SIZE      , 8,
    GLX_BLUE_SIZE       , 8,
    GLX_ALPHA_SIZE      , 8,
    GLX_DOUBLEBUFFER    , True,
    None
  };


  // Set up glXCreateContextAttribsARB
  int fb_count;
  GLXFBConfig *fbc = glXChooseFBConfig(display, xscreen, visual_attribs, &fb_count);
  std::cout << "Found " << fb_count << " matching FBconfigs" << std::endl;

  GLXFBConfig *chosen_fbc = 0;

  printf( "Getting XVisualInfos\n" );
  if (fb_count == 0) {
    std::cout << "Found no valid FBConfig" << std::endl;
    exit(1);
  }

  int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

  int i;
  for (i=0; i<fb_count; ++i)
  {
    XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
    if ( vi )
    {
      int samp_buf, samples;
      glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
      glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );

      printf( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
              " SAMPLES = %d\n", 
              i, vi -> visualid, samp_buf, samples );

      if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
        best_fbc = i, best_num_samp = samples;
      if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
        worst_fbc = i, worst_num_samp = samples;
    }
    XFree( vi );
  }


  int attribs[] = {
    GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
    GLX_CONTEXT_MINOR_VERSION_ARB, 2,
    0
  };

  std::cout << "using FBConfig " << best_fbc << std::endl;
  impl.context = glXCreateContextAttribsARB(display, fbc[best_fbc], NULL, true, attribs);

  XSync(display, False);
  if (!impl.context) {
    std::cout << "Your machine does not support OpenGL 3.2" << std::endl;

    // TODO
    exit(0);
  }

  free(impl.xvisual);

  gtk_widget_set_size_request(GTK_WIDGET(this->gobj()), 100, 100);


  if (glXMakeCurrent(display, id, impl.context) == TRUE) {
    std::cout << "context " << impl.context << std::endl;
    controller->realize();
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
      controller->configure(allocation.width, allocation.height);
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
      controller->draw();
      glXSwapBuffers(display, id);
    }
  }

}


