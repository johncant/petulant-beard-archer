#include "renderer.h"
#include <opencv2/opencv.hpp>
#include <boost/tuple/tuple.hpp>

// External names
using Core::Image;
using Core::Point2D;

// Get in context
namespace GtkGui { namespace Viewer { namespace CorrespondenceView {

// Opengl Helpers
static void HandleOpenGLError(const char* stmt, const char* fname, int line)
{
  GLenum err = GL_NO_ERROR;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cout << "OpenGL error " << err << " from function " << stmt << " called from "<< fname << ":" << line << std::endl;
  }
}

#define GL_CHECK(stmt) stmt; HandleOpenGLError(#stmt, __FILE__, __LINE__);

// Shaders
const std::string vertex_shader_source = ""
"#version 130\n"
"\n"
"precision mediump float;\n"
"in vec2 corners;\n"
"in vec2 texture_uv;\n"
"in float vertex_highlight;\n"
"in float vertex_selectedness;\n"
"\n"
"mat4 ortho = mat4( 2.0,  0.0, 0.0,  0.0,\n"
"                   0.0, -2.0, 0.0,  0.0,\n"
"                   0.0,  0.0, 0.0,  0.0,\n"
"                  -1.0,  1.0, 0.0,  1.0);\n"
"varying vec2 uv;\n"
"varying float highlight;\n"
"varying float selectedness;\n"
"\n"
"\n"
"void main() {\n"
"  gl_Position = ortho*vec4(corners[0], corners[1], 0.0, 1.0);\n"
"  uv = texture_uv;\n"
"  highlight = vertex_highlight;\n"
"  selectedness = vertex_selectedness;\n"
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
"\n"
"precision mediump float;\n"
"\n"
"varying vec2 uv;\n"
"varying float highlight;\n"
"varying float selectedness;\n"
"\n"
"uniform sampler2D image;\n"
"uniform float zoom;\n"
"uniform vec2 zoom_center;\n"
"uniform vec2 sigma_diag_inverse;\n"
"struct SelectRect {\n"
"  float x0;\n"
"  float y0;\n"
"  float x1;\n"
"  float y1;\n"
"};\n"
"uniform vec4 rect_coords;\n"
"uniform float allow_rect_display;\n"
"\n"
"void main() {\n"
"  vec2 normalized_x = (zoom_center-uv)*sigma_diag_inverse;\n"
"  float local_zoom = 1.0+(zoom-1.0)*exp(-0.5*dot(normalized_x, normalized_x));\n"
"  vec2 distorted_uv = zoom_center + (uv - zoom_center)/local_zoom;\n"
"  vec4 image_col;\n"
"\n"
"  float sfx, sfy;\n"
"  float sftx, sfty;\n"
"  if (allow_rect_display > 0.5) {\n"
"    sfx = (distorted_uv[0]-rect_coords[0])/(rect_coords[2]-rect_coords[0]);\n"
"    sfy = (distorted_uv[1]-rect_coords[1])/(rect_coords[3]-rect_coords[1]);\n"
"    sftx = (0.01/sigma_diag_inverse[0])/abs(rect_coords[2]-rect_coords[0])/local_zoom;\n"
"    sfty = (0.01/sigma_diag_inverse[1])/abs(rect_coords[3]-rect_coords[1])/local_zoom;\n"
"  }\n"
"\n"
"  float rect_strength = 0.0;\n"
"  if (allow_rect_display > 0.5) {\n"
"    vec2 nearest_rect_pt;\n"
"\n"
"    // Think of this as testing x*y > 0 with a 45 degree rotation. Determine quadrant\n"
"    if (((sfy-0.5)-(sfx-0.5))*((sfx-0.5)+(sfy-0.5)) > 0.0) {\n"
"      // top or bottom\n"
"      if (sfx < 0.0) {"
"        nearest_rect_pt[0] = rect_coords[0];\n"
"      } else if (sfx > 1.0) {\n"
"        nearest_rect_pt[0] = rect_coords[2];\n"
"      } else {\n"
"        nearest_rect_pt[0] = distorted_uv[0];\n"
"      }\n"
"\n"
"      if (sfy < 0.5) {\n"
"        nearest_rect_pt[1] = rect_coords[1];\n"
"      } else {\n"
"        nearest_rect_pt[1] = rect_coords[3];\n"
"      }\n"
"    } else {\n"
"      // left or right\n"
"      if (sfy < 0.0) {"
"        nearest_rect_pt[1] = rect_coords[1];\n"
"      } else if (sfy > 1.0) {\n"
"        nearest_rect_pt[1] = rect_coords[3];\n"
"      } else {\n"
"        nearest_rect_pt[1] = distorted_uv[1];\n"
"      }\n"
"\n"
"      if (sfx < 0.5) {\n"
"        nearest_rect_pt[0] = rect_coords[0];\n"
"      } else {\n"
"        nearest_rect_pt[0] = rect_coords[2];\n"
"      }\n"
"    }\n"
"\n"
"    vec2 n_rect_dist = 100*(distorted_uv - nearest_rect_pt)*sigma_diag_inverse*local_zoom;\n"
"    rect_strength = exp(-0.5*dot(n_rect_dist, n_rect_dist));\n"
"  }\n"
"  vec4 rect_col = vec4(0.0, 0.0, 1.0, 1.0);\n"
"\n"
"  if (distorted_uv[0] < 0.0 || distorted_uv[1] < 0.0 || distorted_uv[0] > 1.0 || distorted_uv[1] > 1.0) {\n"
"    image_col = vec4(0.0, 0.0, 0.0, 1.0);\n"
"  } else if (abs(length(normalized_x) - 0.5) <= 0.01) {\n"
"    // Zoom distortion movement circle\n"
"    image_col = vec4(0.0, 1.0, 1.0, 0.5);"
"  } else {\n"
"    vec4 tex_col = texture2D(image, distorted_uv);\n"
"    vec4 flip_col = vec4(1.0, 1.0, 1.0, 0.0);\n"
"    vec4 scale_col = vec4(1.0-highlight, 1.0-highlight, 1.0-highlight, 1.0);\n"
"    vec4 select_tex_col = vec4(\n"
"           tex_col[0]*(1.0-selectedness)+tex_col[2]*selectedness,\n"
"           tex_col[1],\n"
"           tex_col[2]*(1.0-selectedness)+tex_col[0]*selectedness,\n"
"           tex_col[3]\n"
"    );\n"
"    image_col = flip_col-(flip_col-select_tex_col)*scale_col; // TODO - don't touch the alpha\n"
"  }\n"
"//  gl_FragColor = 0.5*image_col+0.5*zoom_heat;\n"
"    gl_FragColor = image_col*(1-rect_strength)+rect_strength*rect_col;\n"
"//  gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);\n"
"}\n";

// Renderer impl
GLuint Renderer::compile(const std::string& source, GLenum type) {
  const GLchar* c_source = source.c_str();
  return compile(c_source, type);
}

GLuint Renderer::compile(const char* c_source, GLenum type) {
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

void Renderer::init_shaders() {
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
  GL_CHECK(glBindAttribLocation(shader_program, 2, "vertex_highlight"));
  GL_CHECK(glBindAttribLocation(shader_program, 3, "vertex_selectedness"));

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

Renderer::Renderer() :
  GtkGui::Viewer::Renderer(),
  vp_height(1),
  vp_width(1),
  shader_program(0),
  fragment_shader(0),
  vertex_shader(0) {
}

Renderer::~Renderer() {
}

void Renderer::realize() {

  // Set up shaders
  init_shaders();

  // Set up openGL
  GL_CHECK(glDisable(GL_DEPTH_TEST));
  GL_CHECK(glDepthFunc(GL_LEQUAL));
  GL_CHECK(glDisable(GL_DITHER));
  GL_CHECK(glShadeModel(GL_FLAT));
  GL_CHECK(glEnable(GL_BLEND))
  GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  // Set up texture - this object can't be created without the image
}

void Renderer::configure(unsigned int width, unsigned int height) {

  vp_width = double(width);
  vp_height = double(height);
  GL_CHECK(glViewport(0, 0, width, height));

}

void Renderer::draw(
  const std::vector<
    boost::tuple<
      std::vector<GtkGui::Viewer::ImageView::Renderer::tuple_pt_ud>,
      bool,
      Core::Point2D,
      Core::Point2D
    >
  >& image_draw_args
) {


  // Background
  GL_CHECK(glClearColor(1, 0, 1, 1));
  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
  GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));
//
// use sub_renderers to render textures

//  render_to_texture_image(
//    image_draw_args[0]
//  );

//  render_to_texture_image(
//    image_draw_args[1]
//  );


}

}}}
