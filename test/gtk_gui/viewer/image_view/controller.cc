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

typedef GtkGui::Viewer::FakeControllable FakeControllable;
typedef GtkGui::Viewer::ImageView::FakeRenderer FakeRenderer;
typedef GtkGui::Viewer::ImageView::Controller<FakeControllable, FakeRenderer> Controller;
typedef Controller::ImageController ImageController;

#define controller_setup() \
  boost::shared_ptr<Core::Image> test_image(new Core::Image());\
  test_image->path = "test/files/test0.jpg";\
\
  boost::shared_ptr<FakeControllable> controllable(new FakeControllable());\
  boost::shared_ptr<ImageController> image_controller(new ImageController(test_image));\
\
  Controller controller(controllable, image_controller);



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

/*
 * The image is 100 pixels wide and 200 pixels high.
 * The viewport is 200 pixels wide and 200 pixels high.
 */

//____________________________________________________________________________//
//
BOOST_AUTO_TEST_CASE( create_point )
{

  controller_setup();

  BOOST_CHECK_EQUAL(test_image->points.size(), 0);

  enter     (0, 100);
  move      (20, 100);
  move      (50, 100);
  left_down (50, 100);
  left_up   (50, 100);

  BOOST_CHECK_EQUAL(test_image->points.size(), 1);
  // TODO - check position

}

//____________________________________________________________________________//
//
//BOOST_AUTO_TEST_CASE( drag_existing_point )
//{
//
//  controller_setup();
//
//  Controller::PointRef point = image_controller->
//    add_point(Core::Point2D(0.5, 0.25));
//  BOOST_CHECK_EQUAL(test_image->points.size(), 1);
//
//  enter     (0, 100);
//  move      (20, 100);
//  move      (50, 100);
//  left_down (50, 100);
//  // Drag
//  move      (100, 100);
//  left_up   (100, 100);
//
//  BOOST_CHECK_EQUAL(test_image->points.size(), 1);
//  BOOST_CHECK_CLOSE(point->x, 0.5, 1e-2);
//  BOOST_CHECK_CLOSE(point->y, 0.5, 1e-2);
//  // TODO - check position
//
//}
//
//____________________________________________________________________________//

