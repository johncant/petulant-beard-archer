#define BOOST_TEST_MODULE gtk_gui_image_view_controller
#include <boost/test/included/unit_test.hpp>
#include <src/gtk_gui/viewer/image_view/controller.h>
#include <src/core/image.h>
#include <gtkmm/drawingarea.h>
#include <src/gtk_gui/viewer/image_view/image_controller.h>
#include <test/support/gtk_gui/viewer/image_view/fake_renderer.h>
#include <test/support/gtk_gui/viewer/fake_controllable.h>


/*
 * We set up a controller in a window with 
 *
 */


#define controller_setup() \
  boost::shared_ptr<Core::Image> test_image(new Core::Image());\
  test_image->path = "test/files/test0.png";\
\
  boost::shared_ptr<\
    GtkGui::Viewer::FakeControllable\
  > controllable(new GtkGui::Viewer::FakeControllable());\
\
\
  boost::shared_ptr<\
    GtkGui::Viewer::ImageView::Controller<\
      GtkGui::Viewer::FakeControllable,\
      GtkGui::Viewer::ImageView::FakeRenderer\
    >::ImageController\
  > \
    image_controller(new GtkGui::Viewer::ImageView::Controller<GtkGui::Viewer::FakeControllable>::ImageController(test_image));\
\
\
  GtkGui::Viewer::ImageView::Controller<\
    GtkGui::Viewer::FakeControllable,\
    GtkGui::Viewer::ImageView::FakeRenderer\
  > controller(controllable, image_controller);\
\
  controller.realize();\
  controller.configure(200, 200);\



#define enter(xi, yi) {\
  GdkEventCrossing evt;\
  evt.type = GDK_ENTER_NOTIFY;\
  evt.time = 1000;\
  evt.x = xi;\
  evt.y = yi;\
  evt.state = 0;\
  controllable->signal_enter_notify_event().\
    emit(&evt);\
}

#define move(xi, yi) {\
  GdkEventMotion evt;\
  evt.type = GDK_MOTION_NOTIFY;\
  evt.time = 1000;\
  evt.x = xi;\
  evt.y = yi;\
  evt.state = 0;\
  controllable->signal_motion_notify_event().\
    emit(&evt);\
}

#define left_down(xi, yi) {\
  GdkEventButton evt;\
  evt.type = GDK_BUTTON_PRESS;\
  evt.time = 1000;\
  evt.x = xi;\
  evt.y = yi;\
  evt.state = 0;\
  evt.button = 1;\
  controllable->signal_button_press_event().\
    emit(&evt);\
}

#define left_up(xi, yi) {\
  GdkEventButton evt;\
  evt.type = GDK_BUTTON_RELEASE;\
  evt.time = 1000;\
  evt.x = xi;\
  evt.y = yi;\
  evt.state = 0;\
  evt.button = 1;\
  controllable->signal_button_release_event().\
    emit(&evt);\
}

