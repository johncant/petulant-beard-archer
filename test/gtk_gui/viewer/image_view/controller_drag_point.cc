#define BOOST_TEST_MODULE gtk_gui_image_view_controller
#include <boost/test/included/unit_test.hpp>
#include <src/gtk_gui/viewer/image_view/controller.h>
#include <src/core/image.h>
#include <gtkmm/drawingarea.h>
#include <src/gtk_gui/viewer/image_view/image_controller.h>
#include <test/support/gtk_gui/viewer/image_view/controller_test_helpers.h>
#include <test/support/gtk_gui/viewer/image_view/fake_renderer.h>
#include <test/support/gtk_gui/viewer/fake_controllable.h>


typedef GtkGui::Viewer::FakeControllable FakeControllable;
typedef GtkGui::Viewer::ImageView::FakeRenderer FakeRenderer;
typedef GtkGui::Viewer::ImageView::Controller<FakeControllable, FakeRenderer> Controller;
typedef Controller::ImageController ImageController;

/*
 * The image is 100 pixels wide and 200 pixels high.
 * The viewport is 200 pixels wide and 200 pixels high.
 */


//____________________________________________________________________________//
//
BOOST_AUTO_TEST_CASE( drag_existing_point )
{

  controller_setup();

  Controller::PointRef point = image_controller->
    add_point(Core::Point2D(0.5, 0.25));
  BOOST_CHECK_EQUAL(test_image->points.size(), 1);

  enter     (0, 100);
  move      (20, 100);

  // Move to point
  move      (50, 100);

  // Drag
  left_down (100, 50);
  move      (100, 100);
  left_up   (100, 100);

  // Move away
  move      (100, 150);

  BOOST_CHECK_EQUAL(test_image->points.size(), 1);
  BOOST_CHECK_CLOSE(point->x, 0.5, 1e-2);
  BOOST_CHECK_CLOSE(point->y, 0.5, 1e-2);
  // TODO - check position

}
//
//____________________________________________________________________________//

