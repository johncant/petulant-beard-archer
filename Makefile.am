AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = photogrammetry
#lib_LTLIBRARIES = libphotogrammetry.la
photogrammetry_SOURCES = \
												 src/main.cc \
												 src/gtk_gui/gtk_gui.cc \
												 src/gtk_gui/photogrammetry_window.cc \
												 src/gtk_gui/viewer.cc \
												 src/gtk_gui/blank_renderer.cc \
												 src/gtk_gui/image_view_renderer.cc \
												 src/gtk_gui/renderer.cc \
												 src/importers/base.cc \
												 src/importers/insight3d.cc


#libphotogrammetry_la_SOURCES = src/gtk_gui.cc

AM_CXXFLAGS = $(gl_CFLAGS) $(GTKMM_CFLAGS) -fno-builtin-malloc -fno-builtin-free -g
LIBS = $(gl_LIBS) $(GTKMM_LIBS) -lX11

