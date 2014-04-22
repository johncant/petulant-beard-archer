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

  Controller::PointRef point_outside = image_controller->
    add_point(Core::Point2D(0.5, 0.25));

  Controller::PointRef point_inside = image_controller->
    add_point(Core::Point2D(0.5, 0.75));

  BOOST_CHECK_EQUAL(test_image->points.size(), 2);

  enter     (0, 100);
  move      (20, 100);

  // Select point_outside
  move      (100, 50);
  left_down (100, 50);
  left_up   (100, 50);

  // Check that point_outside is selected with no side effects
  BOOST_CHECK_EQUAL(test_image->points.size(), 2);
  BOOST_CHECK_EQUAL(point_outside.params().is_selected(), true);
  BOOST_CHECK_EQUAL(controller.get_selection().count(point_outside), 1);
  // Now start the test

  // Move to bottom left
  move      (70, 180);
  left_down (70, 180);

  // Move to top right
  move      (130, 120);
  left_up   (130, 120);

  // Check no side effects
  BOOST_CHECK_EQUAL(test_image->points.size(), 2);

  // Check that point 0 is deselected
  BOOST_CHECK_EQUAL(point_outside.params().is_selected(), false);
  BOOST_CHECK_EQUAL(controller.get_selection().count(point_outside), 0);

  // Check that point 1 is selected
  BOOST_CHECK_EQUAL(point_inside.params().is_selected(), true);
  BOOST_CHECK_EQUAL(controller.get_selection().count(point_inside), 1);

}
//
//____________________________________________________________________________//

