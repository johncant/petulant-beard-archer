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
BOOST_AUTO_TEST_CASE( select_point )
{

  controller_setup();

  Controller::PointRef point0 = image_controller->
    add_point(Core::Point2D(0.5, 0.25));

  Controller::PointRef point1 = image_controller->
    add_point(Core::Point2D(0.5, 0.75));

  BOOST_CHECK_EQUAL(test_image->points.size(), 2);

  enter     (0, 100);
  move      (20, 100);

  // Move to point 0
  move      (50, 100);
  move      (100, 50);

  // Select point 0
  left_down (100, 50);
  left_up   (100, 100);

  // Check that point 0 is selected with no side effects
  BOOST_CHECK_EQUAL(test_image->points.size(), 2);
  BOOST_CHECK_EQUAL(point0.params().is_selected(), true);
  BOOST_CHECK_EQUAL(controller.get_selection().count(point0), 1);

  // Move to other point
  move            (100, 150);
  shift_left_down (100, 150);
  left_up         (100, 150);

  // Check no side effects
  BOOST_CHECK_EQUAL(test_image->points.size(), 2);

  // Check that point 0 is still selected
  BOOST_CHECK_EQUAL(point0.params().is_selected(), true);
  BOOST_CHECK_EQUAL(controller.get_selection().count(point0), 1);

  // Check that point 1 is selected
  BOOST_CHECK_EQUAL(point1.params().is_selected(), true);
  BOOST_CHECK_EQUAL(controller.get_selection().count(point1), 1);

}
//
//____________________________________________________________________________//

