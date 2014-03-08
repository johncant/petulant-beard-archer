#include "image_view_renderer.h"
//#include <boost/phoenix.hpp>
//#include <boost/function.hpp>
//#include <boost/mpl/if.hpp>
//#include <boost/mpl/equal.hpp>

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092

#include <opencv2/opencv.hpp>

// Gave up on operators and templates and used macro
static void HandleOpenGLError(const char* stmt, const char* fname, int line)
{
  GLenum err = GL_NO_ERROR;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cout << "OpenGL error " << err << " from function " << stmt << " called from "<< fname << ":" << line << std::endl;
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
"\n"
"precision mediump float;\n"
"\n"
"varying vec2 uv;\n"
"\n"
"uniform sampler2D image;\n"
"uniform float zoom;\n"
"uniform vec2 zoom_center;\n"
"\n"
"void main() {\n"
"  float distance = length(zoom_center-uv);\n"
"  float sigma = 0.2, nd = distance/sigma;\n"
"  float local_zoom = 1.0+(zoom-1.0)*exp(-0.5*nd*nd);\n"
"  vec2 distorted_uv = zoom_center + (uv - zoom_center)/local_zoom;\n"
"//  vec4 zoom_heat = vec4(2-local_zoom, local_zoom-1, 0, 1);\n"
"  vec4 image_col;\n"
"  if (distorted_uv[0] < 0.0 || distorted_uv[1] < 0.0 || distorted_uv[0] > 1.0 || distorted_uv[1] > 1.0) {\n"
"    image_col = vec4(0.0, 0.0, 0.0, 1.0);\n"
"  } else {\n"
"    image_col = texture2D(image, distorted_uv);\n"
"  }\n"
"//  gl_FragColor = 0.5*image_col+0.5*zoom_heat;\n"
"    gl_FragColor = image_col;\n"
"//  gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);\n"
"}\n";

namespace Tr2 = Core::Transform2D;

// Renderer
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
  sprite_pixels(
    boost::shared_ptr<cv::Mat>(
      new cv::Mat(cv::imread("/home/john/src/photogrammetry/assets/point.png", CV_LOAD_IMAGE_UNCHANGED))
    )
  ),
  vp_height(1),
  vp_width(1),
  gl_tex_id(0),
  shader_program(0),
  fragment_shader(0),
  vertex_shader(0),
  zoom(1.0),
  zoom_center(0.5, 0.5) {
  
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
  GL_CHECK(glEnable(GL_BLEND))
  GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  // Set up texture - this object can't be created without the image
  GL_CHECK(glGenTextures(1, &gl_tex_id));
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, gl_tex_id));

  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  // TODO - CONFIG Same as background colour

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

  // Set up point sprite texture
  GL_CHECK(glGenTextures(1, &gl_sprite_tex_id));
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, gl_sprite_tex_id));

  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  // TODO - CONFIG Same as background colour

  //   Assume RGB
  GL_CHECK(glTexImage2D(GL_TEXTURE_2D,
                        0,
                        GL_RGBA,
                        sprite_pixels->cols,
                        sprite_pixels->rows,
                        0,
                        GL_BGRA,
                        GL_UNSIGNED_BYTE,  // Image data type
                        sprite_pixels->ptr()
  ));

}

void GtkGui::ImageViewRenderer::configure(unsigned int width, unsigned int height) {

  vp_width = double(width);
  vp_height = double(height);
  GL_CHECK(glViewport(0, 0, width, height));

}

void GtkGui::ImageViewRenderer::draw() {
  // Background
  GL_CHECK(glClearColor(0, 0, 0, 1));
  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
  GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));

  draw_image();
  draw_points();
}

void GtkGui::ImageViewRenderer::draw_image() {
  GLuint vbo[2], vao[2];

  GL_CHECK(glUseProgram(shader_program));

  //Tr2::Transformation<ImageToViewport> sp_trans(double(pixels->rows)/double(pixels->cols), vp_height/vp_width);
  Tr2::Transformation<ImageToViewport> sp_trans = get_image_to_viewport_transform();

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

  // Zoom params
  GLint zoom_loc, zoom_center_loc;
  GLfloat im_zoom = this->zoom;
  GLfloat im_zoom_center[2] = {
    this->zoom_center.x, this->zoom_center.y
  };

  GL_CHECK(zoom_loc = glGetUniformLocation(shader_program, "zoom"));
  GL_CHECK(zoom_center_loc = glGetUniformLocation(shader_program, "zoom_center"));

  GL_CHECK(glUniform1fARB(zoom_loc, im_zoom));
  GL_CHECK(glUniform2fARB(zoom_center_loc, im_zoom_center[0], im_zoom_center[1]));

  // Image
  GL_CHECK(glDrawArrays(GL_QUADS, 0, 4));

}

void GtkGui::ImageViewRenderer::draw_points() {
  GLuint vbo[2], vao[2];

  GL_CHECK(glUseProgram(shader_program));

  //Tr2::Transformation<ImageToViewport> sp_trans(double(pixels->rows)/double(pixels->cols), vp_height/vp_width);

//  Tr2::Transformation<
//    Tr2::CombinationBase<
//      Distortion,
//      ImageToViewport
//    >
//  > tform(combine(
//    get_distortion_transform(),
//    get_image_to_viewport_transform()
//  ));

//  Tr2::Transformation<ImageToViewport> tform = get_image_to_viewport_transform();

  float positions[8*image->points.size()];
  float tex_coords[8*image->points.size()];

  double sprite_hw =
    double(sprite_pixels->cols)/(2.0*vp_width);
  double sprite_hh =
    double(sprite_pixels->rows)/(2.0*vp_height);

  std::cout << "width: " << sprite_pixels->cols << ", height: " << sprite_pixels->rows << std::endl;
  std::cout << "width: " << sprite_hw << ", height: " << sprite_hh << std::endl;

  struct tform_t {
    GtkGui::ImageViewRenderer &r;
    tform_t(GtkGui::ImageViewRenderer &_r) : r(_r) {}

    Core::Point2D t(Core::Point2D pt) {
      return Tr2::combine(
        r.get_distortion_transform().inverse(),
        r.get_image_to_viewport_transform()
      ).t(pt);
    }
  };

  tform_t tform(*this);

  for(int i=0; i<image->points.size(); i++) {
    positions[8*i+0] = tform.t(image->points[i]).x-sprite_hw;
    positions[8*i+1] = tform.t(image->points[i]).y-sprite_hh;
    positions[8*i+2] = tform.t(image->points[i]).x-sprite_hw;
    positions[8*i+3] = tform.t(image->points[i]).y+sprite_hh;
    positions[8*i+4] = tform.t(image->points[i]).x+sprite_hw;
    positions[8*i+5] = tform.t(image->points[i]).y+sprite_hh;
    positions[8*i+6] = tform.t(image->points[i]).x+sprite_hw;
    positions[8*i+7] = tform.t(image->points[i]).y-sprite_hh;

    tex_coords[8*i+0] = 0.0;
    tex_coords[8*i+1] = 0.0;
    tex_coords[8*i+2] = 0.0;
    tex_coords[8*i+3] = 1.0;
    tex_coords[8*i+4] = 1.0;
    tex_coords[8*i+5] = 1.0;
    tex_coords[8*i+6] = 1.0;
    tex_coords[8*i+7] = 0.0;
  }

  // Generate buffers
  GL_CHECK(glGenBuffers(2, vbo));
  GL_CHECK(glGenVertexArrays(2, vao));

  // Set vertex position data
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo[0]));
  GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat)*image->points.size(), positions, GL_STATIC_DRAW));
  GL_CHECK(glBindVertexArray(vao[0]));

  GL_CHECK(glEnableVertexAttribArray(0));
  GL_CHECK(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0));

  // Set vertex texture data
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo[1]));
  GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat)*image->points.size(), tex_coords, GL_STATIC_DRAW));

  GL_CHECK(glEnableVertexAttribArray(1));
  GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0));

  // Texture
  GLint pt_img_loc;
  GL_CHECK(pt_img_loc = glGetUniformLocation(shader_program, "image"));
  GL_CHECK(glActiveTextureARB(GL_TEXTURE0));
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, gl_sprite_tex_id));
  GL_CHECK(glEnable(GL_TEXTURE_2D));
  GL_CHECK(glUniform1iARB(pt_img_loc, 0));

  // Zoom params
  GLint zoom_loc, zoom_center_loc;
  GLfloat sp_zoom = 1.0;
  GLfloat sp_zoom_center[2] = {
    this->zoom_center.x, this->zoom_center.y
  };

  GL_CHECK(zoom_loc = glGetUniformLocation(shader_program, "zoom"));
  GL_CHECK(zoom_center_loc = glGetUniformLocation(shader_program, "zoom_center"));

  GL_CHECK(glUniform1fARB(zoom_loc, sp_zoom));
  GL_CHECK(glUniform2fARB(zoom_center_loc, sp_zoom_center[0], sp_zoom_center[1]));

  // Image
  GL_CHECK(glDrawArrays(GL_QUADS, 0, 4*image->points.size()));

}

void GtkGui::ImageViewRenderer::set_zoom(double _zoom) {
  zoom = _zoom;
}

GtkGui::ImageViewRenderer::Distortion GtkGui::ImageViewRenderer::get_distortion_transform() {
  return GtkGui::ImageViewRenderer::Distortion(zoom_center, zoom, 0.2);
}

GtkGui::ImageViewRenderer::ImageToViewport GtkGui::ImageViewRenderer::get_image_to_viewport_transform() {
  return GtkGui::ImageViewRenderer::ImageToViewport(double(pixels->rows)/double(pixels->cols), vp_height/vp_width);
}

void GtkGui::ImageViewRenderer::set_zoom_center(Core::Point2D _zoom_center) {
  zoom_center = get_image_to_viewport_transform().inverse().t(_zoom_center);
}

Core::Point2D GtkGui::ImageViewRenderer::as_image_coords(Core::Point2D pt) {
  // Fwd transform defined as
  // Position in image part of viewport -> position on actual image
  //
  // Our question is, for a point on the image display space, 
  return Tr2::combine(
    get_image_to_viewport_transform().inverse(),
    get_distortion_transform()
  ).t(pt);
}
