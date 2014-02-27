#include "image_view_renderer.h"

#include <GL/gl.h>

GtkGui::ImageViewRenderer::ImageViewRenderer(boost::shared_ptr<Core::Image> im) : Renderer(), image(im) { }

GtkGui::ImageViewRenderer::~ImageViewRenderer() {}

void GtkGui::ImageViewRenderer::realize() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDisable(GL_DITHER);
  glShadeModel(GL_SMOOTH);
}

void GtkGui::ImageViewRenderer::configure(unsigned int width, unsigned int height) {
  glViewport(0, 0, width, height);
}

void GtkGui::ImageViewRenderer::draw() {
  glClear(GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 100, 100, 0, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0, 1, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLES);
  glVertex2f(10, 10);
  glVertex2f(10, 90);
  glVertex2f(90, 90);
  glEnd();
}

