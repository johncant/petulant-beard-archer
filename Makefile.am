AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = photogrammetry
#lib_LTLIBRARIES = libphotogrammetry.la
photogrammetry_SOURCES = \
												 src/main.cc \
												 src/gtk_gui.cc \
												 src/photogrammetry_window.cc \
												 src/viewer.cc


#libphotogrammetry_la_SOURCES = src/gtk_gui.cc

AM_CXXFLAGS = $(gl_CFLAGS) $(GTKMM_CFLAGS)
LIBS = $(gl_LIBS) $(GTKMM_LIBS) -lX11

