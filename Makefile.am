AUTOMAKE_OPTIONS = subdir-objects

TESTS = insight3d_test \
				gtk_gui_viewer_image_view_controller_create_point_test \
			  gtk_gui_viewer_image_view_controller_drag_point_test \
		    gtk_gui_viewer_image_view_controller_select_point_test \
				gtk_gui_viewer_image_view_controller_select_another_point_test \
				gtk_gui_viewer_image_view_controller_select_rectangle_test

check_PROGRAMS = insight3d_test\
		  gtk_gui_viewer_image_view_controller_create_point_test \
			gtk_gui_viewer_image_view_controller_drag_point_test \
		  gtk_gui_viewer_image_view_controller_select_point_test \
		  gtk_gui_viewer_image_view_controller_select_another_point_test \
		  gtk_gui_viewer_image_view_controller_select_rectangle_test


insight3d_test_SOURCES = \
							 test/importers/insight3d.cc \
							 src/importers/base.cc \
							 src/importers/insight3d.cc

# Required to test controller.cc
#							 src/gtk_gui/viewer/controller.cc
#							 src/gtk_gui/viewer/renderer.cc
#							 src/gtk_gui/viewer/image_view/renderer.cc
#							 test/support/gtk_gui/viewer/fake_controllable.cc
#							 test/support/gtk_gui/viewer/image_view/fake_renderer.cc

gtk_gui_viewer_image_view_controller_create_point_test_SOURCES = \
							 src/gtk_gui/viewer/controller.cc\
							 src/gtk_gui/viewer/renderer.cc\
							 src/gtk_gui/viewer/image_view/renderer.cc \
							 test/support/gtk_gui/viewer/fake_controllable.cc\
							 test/support/gtk_gui/viewer/image_view/fake_renderer.cc\
							 test/gtk_gui/viewer/image_view/controller_create_point.cc

gtk_gui_viewer_image_view_controller_drag_point_test_SOURCES = \
							 src/gtk_gui/viewer/controller.cc\
							 src/gtk_gui/viewer/renderer.cc\
							 src/gtk_gui/viewer/image_view/renderer.cc \
							 test/support/gtk_gui/viewer/fake_controllable.cc\
							 test/support/gtk_gui/viewer/image_view/fake_renderer.cc\
							 test/gtk_gui/viewer/image_view/controller_drag_point.cc

gtk_gui_viewer_image_view_controller_select_point_test_SOURCES = \
							 src/gtk_gui/viewer/controller.cc\
							 src/gtk_gui/viewer/renderer.cc\
							 src/gtk_gui/viewer/image_view/renderer.cc \
							 test/support/gtk_gui/viewer/fake_controllable.cc\
							 test/support/gtk_gui/viewer/image_view/fake_renderer.cc\
							 test/gtk_gui/viewer/image_view/controller_select_point.cc

gtk_gui_viewer_image_view_controller_select_another_point_test_SOURCES = \
							 src/gtk_gui/viewer/controller.cc\
							 src/gtk_gui/viewer/renderer.cc\
							 src/gtk_gui/viewer/image_view/renderer.cc \
							 test/support/gtk_gui/viewer/fake_controllable.cc\
							 test/support/gtk_gui/viewer/image_view/fake_renderer.cc\
							 test/gtk_gui/viewer/image_view/controller_select_another_point.cc

gtk_gui_viewer_image_view_controller_select_rectangle_test_SOURCES = \
							 src/gtk_gui/viewer/controller.cc\
							 src/gtk_gui/viewer/renderer.cc\
							 src/gtk_gui/viewer/image_view/renderer.cc \
							 test/support/gtk_gui/viewer/fake_controllable.cc\
							 test/support/gtk_gui/viewer/image_view/fake_renderer.cc\
							 test/gtk_gui/viewer/image_view/controller_select_rectangle.cc


bin_PROGRAMS = photogrammetry
#lib_LTLIBRARIES = libphotogrammetry.la
photogrammetry_SOURCES = \
												 src/main.cc \
												 src/gtk_gui/gtk_gui.cc \
												 src/gtk_gui/photogrammetry_window.cc \
												 src/gtk_gui/image_chooser.cc \
												 src/gtk_gui/viewer_widget.cc \
												 src/gtk_gui/viewer/widget_controllable.cc \
												 src/gtk_gui/viewer/controller.cc \
												 src/gtk_gui/viewer/renderer.cc \
												 src/gtk_gui/viewer/image_view/renderer.cc \
												 src/gtk_gui/viewer/blank_renderer.cc \
												 src/importers/base.cc \
												 src/importers/insight3d.cc


#libphotogrammetry_la_SOURCES = src/gtk_gui.cc

AM_CXXFLAGS = $(gl_CFLAGS) $(glew_CFLAGS) $(GTKMM_CFLAGS) $(opencv_CFLAGS) -fno-builtin-malloc -fno-builtin-free -g
LIBS = $(gl_LIBS) $(glew_LIBS) $(GTKMM_LIBS) $(opencv_LIBS) -lX11 -g


get-deps:
	pkg-config --list-all
