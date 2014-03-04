#include "image_view_renderer.h"
//#include <boost/phoenix.hpp>
//#include <boost/function.hpp>
//#include <boost/mpl/if.hpp>
//#include <boost/mpl/equal.hpp>
#include <string>
#include "../core/transform2d.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092

// Gave up on operators and templates and used macro
static void HandleOpenGLError(const char* stmt, const char* fname, int line)
{
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cout << "OpenGL error " << err << " from function " << stmt << " called from "<< line << ":" << fname << std::endl;
  }
}

#define GL_CHECK(stmt) stmt; HandleOpenGLError(#stmt, __FILE__, __LINE__);

const std::string vertex_shader_source = ""
"#version 130\n"
"\n"
"precision mediump float;\n"
"in vec2 corners;\n"
"in vec2 texture_uv;\n"
"mat4 ortho = mat4( 2.0,  0.0, 0.0,  0.0,\n"
"                   0.0, -2.0, 0.0,  0.0,\n"
"                   0.0,  0.0, 0.0,  0.0,\n"
"                  -1.0,  1.0, 0.0,  1.0);\n"
"varying vec2 uv;\n"
"\n"
"\n"
"void main() {\n"
"  gl_Position = ortho*vec4(corners[0], corners[1], 0.0, 1.0);\n"
"  uv = texture_uv;\n"
"//  gl_PointSize = 5;\n"
"//  gl_ClipDistance[0] = 10.0;\n"
"//  gl_ClipDistance[1] = 10.0;\n"
"}\n"
"\n";
// Distortion rolloff function
// original texture position: uv
// display position for texture point: uv = (uv'-cz)*z(uv)+cz
// uv' = (uv-cz)/z(uv) + cz
static const std::string fragment_shader_source = ""
"#version 130\n"
"varying vec2 uv;\n"
"precision mediump float;\n"
"uniform sampler2D image;\n"
"\n"
"vec2 distortion_point = vec2(0.45, 0.4);\n"
"float zoom = 2.0;\n"
"\n"
"void main() {\n"
"  float distance = length(distortion_point-uv);\n"
"  float sigma = 0.1;\n"
"  float local_zoom = 1+(zoom-1)*exp(-0.5*dot(distance/sigma, distance/sigma));\n"
"  vec2 distorted_uv = distortion_point + (uv - distortion_point)/local_zoom;\n"
"  vec4 zoom_heat = vec4(2-local_zoom, local_zoom-1, 0, 1);\n"
"  vec4 image = texture2D(image, distorted_uv);\n"
"  gl_FragColor = 0.5*image+0.5*zoom_heat;\n"
"//  gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);\n"
"}\n";

namespace Tr2 = Core::Transform2D;

// The transformation between image 0-1 coordinates and viewport 0-1 coordinates
class ImageToViewport :
  public Tr2::Transformation<Tr2::AtOrigin<Tr2::Scaling> > {
  public:
  inline ImageToViewport(double img_aspect, double vp_aspect) :
    Transformation<AtOrigin<Tr2::Scaling> >(
      0.5, 0.5,
      Transformation<Tr2::Scaling>(
        img_aspect > vp_aspect ? vp_aspect  / img_aspect : 1.0,
        img_aspect > vp_aspect ? 1.0                     : img_aspect /  vp_aspect
      )
    )
  {
  }
};

// TODO - Use a combination of transformations
class OriginDistortion : public Tr2::Base {
  public:
  double zoom, sigma;
  OriginDistortion(double _zoom, double _sigma) : zoom(_zoom), sigma(_sigma) {
  }

  Core::Point2D t(Core::Point2D input) {
    double ndist = sqrt(input.x*input.x + input.y*input.y)/sigma;

    double warp = 1.0 + (zoom-1.0)*exp(-0.5*ndist*ndist);

    return Core::Point2D(
      input.x*warp,
      input.y*warp
    );
  }
};

class Distortion : public Tr2::AtOrigin<OriginDistortion> {
  public:
  Distortion(double ox, double oy, double zoom, double sigma) :
    Tr2::AtOrigin<OriginDistortion>(ox, oy, OriginDistortion(zoom, sigma))
  {
  }
};

GLuint GtkGui::ImageViewRenderer::compile(const std::string& source, GLenum type) {
  const GLchar* c_source = source.c_str();
  return compile(c_source, type);
}

GLuint GtkGui::ImageViewRenderer::compile(const char* c_source, GLenum type) {
  GLuint shader;
  GL_CHECK(shader = glCreateShader(type));

  GL_CHECK(glShaderSource(shader, 1, &c_source, 0));

  GL_CHECK(glCompileShader(shader));

  GLint success = 0;
  GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
  if (!success) {
    std::cout << "Shader compilation failed" << std::endl;

    GLchar error[2048];
    GLsizei size;
    GL_CHECK(glGetShaderInfoLog(shader, 2048, &size, error));

    std::cout << error << std::endl;
    //exit(1); // TODO - need better error handling
  }

  return shader;
}

void GtkGui::ImageViewRenderer::init_shaders() {
  std::cout << "OpenGL version is: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  GLenum glew_status;
  GL_CHECK(glew_status = glewInit());
  if (glew_status != GLEW_OK) {
    std::cout << "glew failed to initialize" << std::endl;
    exit(1);
  }

  GL_CHECK(shader_program = glCreateProgram());

  vertex_shader = compile(vertex_shader_source, GL_VERTEX_SHADER);
  std::cout << "Vertex shader compiled" << std::endl;

  fragment_shader = compile(fragment_shader_source, GL_FRAGMENT_SHADER);
  std::cout << "Fragment shader compiled" << std::endl;

  GL_CHECK(glAttachShader(shader_program, vertex_shader));
  GL_CHECK(glAttachShader(shader_program, fragment_shader));


  GL_CHECK(glBindAttribLocation(shader_program, 0, "corners"));
  GL_CHECK(glBindAttribLocation(shader_program, 1, "texture_uv"));

  GL_CHECK(glLinkProgram(shader_program));

  GLint link_success = 0;
  GL_CHECK(glGetProgramiv(shader_program, GL_LINK_STATUS, &link_success));

  if (!link_success) {
    std::cout << "Program linking failed" << std::endl;

    GLchar error[2048];
    GLsizei size;
    GL_CHECK(glGetProgramInfoLog(shader_program, 2048, &size, error));

    std::cout << error << std::endl;
    exit(1); // TODO - need better error handling

  }

  std::cout << "Shader program linked successfully" << std::endl;

}

GtkGui::ImageViewRenderer::ImageViewRenderer(boost::shared_ptr<Core::Image> im) :
  Renderer(),
  image(im),
  pixels(image->pixels()),
  vp_height(1),
  vp_width(1),
  gl_tex_id(0),
  shader_program(0),
  fragment_shader(0),
  vertex_shader(0) {
}

GtkGui::ImageViewRenderer::~ImageViewRenderer() {
  if (gl_tex_id) {
    GL_CHECK(glDeleteTextures(1, &gl_tex_id));
    // TODO
  }
}

void GtkGui::ImageViewRenderer::realize() {

  // Set up shaders
  init_shaders();

  // Set up openGL
  GL_CHECK(glDisable(GL_DEPTH_TEST));
  GL_CHECK(glDepthFunc(GL_LEQUAL));
  GL_CHECK(glDisable(GL_DITHER));
  GL_CHECK(glShadeModel(GL_FLAT));

  // Set up texture - this object can't be created without the image
  GL_CHECK(glGenTextures(1, &gl_tex_id));
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, gl_tex_id));

  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

  //   Assume RGB
  GL_CHECK(glTexImage2D(GL_TEXTURE_2D,
                        0,
                        GL_RGB,
                        pixels->cols,
                        pixels->rows,
                        0,
                        GL_BGR,
                        GL_UNSIGNED_BYTE,  // Image data type
                        pixels->ptr()
  ));
}

void GtkGui::ImageViewRenderer::configure(unsigned int width, unsigned int height) {

  vp_width = double(width);
  vp_height = double(height);
  GL_CHECK(glViewport(0, 0, width, height));

}

void GtkGui::ImageViewRenderer::draw() {

  GLuint vbo[2], vao[2];

  GL_CHECK(glUseProgram(shader_program));
  GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));

  Tr2::Transformation<ImageToViewport> sp_trans(double(pixels->rows)/double(pixels->cols), vp_height/vp_width);

  Core::Point2D p1 = sp_trans.t(0.0, 0.0);
  Core::Point2D p2 = sp_trans.t(0.0, 1.0);
  Core::Point2D p3 = sp_trans.t(1.0, 1.0);
  Core::Point2D p4 = sp_trans.t(1.0, 0.0);

  const float positions[] = {
    p1.x, p1.y,
    p2.x, p2.y,
    p3.x, p3.y,
    p4.x, p4.y
  };

  const float tex_coords[] = {
    0.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0
  };

  // Generate buffers
  GL_CHECK(glGenBuffers(2, vbo));
  GL_CHECK(glGenVertexArrays(2, vao));

  // Set vertex position data
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo[0]));
  GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), positions, GL_STATIC_DRAW));
  GL_CHECK(glBindVertexArray(vao[0]));

  GL_CHECK(glEnableVertexAttribArray(0));
  GL_CHECK(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0));

  // Set vertex texture data
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo[1]));
  GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tex_coords, GL_STATIC_DRAW));

  GL_CHECK(glEnableVertexAttribArray(1));
  GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0));

  // Texture
  GLint img_loc;
  GL_CHECK(img_loc = glGetUniformLocation(shader_program, "image"));
  GL_CHECK(glActiveTextureARB(GL_TEXTURE0));
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, gl_tex_id));
  GL_CHECK(glEnable(GL_TEXTURE_2D));
  GL_CHECK(glUniform1iARB(img_loc, 0));

  // Background
  GL_CHECK(glClearColor(0, 0, 0, 1));
  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

  // Image
  GL_CHECK(glDrawArrays(GL_QUADS, 0, 4));

}

Core::Point2D GtkGui::ImageViewRenderer::image_position_from_cursor(double x, double y) {
  // Transform point to image coords
  return Core::Point2D(x, y);
}
