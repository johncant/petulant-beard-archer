#include "renderer.h"
#include <opencv2/opencv.hpp>
#include <boost/tuple/tuple.hpp>

// External names
using Core::Image;
using Core::Point2D;

namespace TF {
  using namespace Core::Transform2D;
  using namespace GtkGui::Viewer::ImageView::Transforms;
};

using GtkGui::Viewer::ImageView::PointViewParams;

// Get in context
namespace GtkGui { namespace Viewer { namespace ImageView {

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

Renderer::Renderer(boost::shared_ptr<Core::Image> im) :
  GtkGui::Viewer::Renderer(),
  image(im),
  pixels(im->pixels()),
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

  if (pixels->cols > pixels->rows) {
    // Landscape
    sigma_x = 0.2*double(pixels->rows)/double(pixels->cols);
    sigma_y = 0.2;
  } else {
    sigma_x = 0.2;
    sigma_y = 0.2*double(pixels->cols)/double(pixels->rows);
    // Portrait
  }

}

Renderer::~Renderer() {
  if (gl_tex_id) {
    GL_CHECK(glDeleteTextures(1, &gl_tex_id));
    // TODO
  }
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

void Renderer::configure(unsigned int width, unsigned int height) {

  vp_width = double(width);
  vp_height = double(height);
  GL_CHECK(glViewport(0, 0, width, height));

}

void Renderer::draw(
  std::vector<tuple_pt_ud> const & points,
  bool allow_rectangle_select,
  const Core::Point2D &rectangle_start,
  const Core::Point2D &rectangle_end
) {
  // Background
  GL_CHECK(glClearColor(0, 0, 0, 1));
  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
  GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));

//  if (vp_height > 0.5 && vp_width > 0.5) {
    draw_image(
      allow_rectangle_select,
      rectangle_start,
      rectangle_end
    );
    draw_points(points);
//  }
}

void Renderer::draw_image(
  bool allow_rectangle_select,
  const Core::Point2D &rectangle_start,
  const Core::Point2D &rectangle_end
) {
  GLuint vbo[4], vao[4];

  GL_CHECK(glUseProgram(shader_program));

  //TF::Transform<ImageToViewport> sp_trans(double(pixels->rows)/double(pixels->cols), vp_height/vp_width);
  TF::ImageToViewport sp_trans = get_image_to_viewport_transform();

  Point2D p1 = sp_trans.t(0.0, 0.0);
  Point2D p2 = sp_trans.t(0.0, 1.0);
  Point2D p3 = sp_trans.t(1.0, 1.0);
  Point2D p4 = sp_trans.t(1.0, 0.0);

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

  const float highlights[] = {
    0.0,
    0.0,
    0.0,
    0.0
  };

  const float *selectednesses = highlights;

  // Generate buffers
  GL_CHECK(glGenBuffers(4, vbo));
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

  // Set vertex highlight
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo[2]));
  GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat), highlights, GL_STATIC_DRAW));

  GL_CHECK(glEnableVertexAttribArray(2));
  GL_CHECK(glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0));

  // Set vertex selectednesses
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo[3]));
  GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat), selectednesses, GL_STATIC_DRAW));

  GL_CHECK(glEnableVertexAttribArray(3));
  GL_CHECK(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0));

  // Zoom params
  GLint zoom_loc, zoom_center_loc, sigma_di_loc;
  GLfloat im_zoom = this->zoom;
  GLfloat im_zoom_center[2] = {
    this->zoom_center.x, this->zoom_center.y
  };
  GLfloat sigma_di[2];

  sigma_di[0] = 1/sigma_x;
  sigma_di[1] = 1/sigma_y;

  GL_CHECK(zoom_loc = glGetUniformLocation(shader_program, "zoom"));
  GL_CHECK(zoom_center_loc = glGetUniformLocation(shader_program, "zoom_center"));
  GL_CHECK(sigma_di_loc = glGetUniformLocation(shader_program, "sigma_diag_inverse"));

  GL_CHECK(glUniform1fARB(zoom_loc, im_zoom));
  GL_CHECK(glUniform2fARB(zoom_center_loc, im_zoom_center[0], im_zoom_center[1]));
  GL_CHECK(glUniform2fARB(sigma_di_loc, sigma_di[0], sigma_di[1]));

  // Rectangle select
  GLint allow_rect_display_loc, rect_coords_loc;
  GLfloat allow_rect_display = allow_rectangle_select ? 1.0 : 0.0;
  GLfloat rect_coords[4];

  rect_coords[0] = rectangle_start.x;
  rect_coords[1] = rectangle_start.y;
  rect_coords[2] = rectangle_end.x;
  rect_coords[3] = rectangle_end.y;

  std::cout << allow_rectangle_select
     << " " << rect_coords[0]
     << " " << rect_coords[1]
     << " " << rect_coords[2]
     << " " << rect_coords[3]
            << std::endl;

  GL_CHECK(allow_rect_display_loc = glGetUniformLocation(shader_program, "allow_rect_display"));
  GL_CHECK(rect_coords_loc = glGetUniformLocation(shader_program, "rect_coords"));

  GL_CHECK(glUniform1fARB(allow_rect_display_loc, allow_rect_display));
  GL_CHECK(glUniform4fARB(rect_coords_loc, rect_coords[0], rect_coords[1], rect_coords[2], rect_coords[3]));

  // Image
  GL_CHECK(glDrawArrays(GL_QUADS, 0, 4));

}

void Renderer::draw_points(const std::vector<tuple_pt_ud>& points) {
  GLuint vbo[4], vao[4];

  GL_CHECK(glUseProgram(shader_program));

  float positions[8*points.size()];
  float tex_coords[8*points.size()];
  float highlights[4*points.size()];
  float selectednesses[4*points.size()];

  double sprite_hw =
    double(sprite_pixels->cols)/(2.0*vp_width);
  double sprite_hh =
    double(sprite_pixels->rows)/(2.0*vp_height);

  TF::Combination<
    TF::Distortion::Inverse,
    TF::ImageToViewport
  >::type tform(
    get_distortion_transform().inverse(),
    get_image_to_viewport_transform()
  );

  for(int i=0; i<points.size(); i++) {
    positions[8*i+0] = tform.t(points[i].get<0>()).x-sprite_hw;
    positions[8*i+1] = tform.t(points[i].get<0>()).y-sprite_hh;
    positions[8*i+2] = tform.t(points[i].get<0>()).x-sprite_hw;
    positions[8*i+3] = tform.t(points[i].get<0>()).y+sprite_hh;
    positions[8*i+4] = tform.t(points[i].get<0>()).x+sprite_hw;
    positions[8*i+5] = tform.t(points[i].get<0>()).y+sprite_hh;
    positions[8*i+6] = tform.t(points[i].get<0>()).x+sprite_hw;
    positions[8*i+7] = tform.t(points[i].get<0>()).y-sprite_hh;

    tex_coords[8*i+0] = 0.0;
    tex_coords[8*i+1] = 0.0;
    tex_coords[8*i+2] = 0.0;
    tex_coords[8*i+3] = 1.0;
    tex_coords[8*i+4] = 1.0;
    tex_coords[8*i+5] = 1.0;
    tex_coords[8*i+6] = 1.0;
    tex_coords[8*i+7] = 0.0;

    float hl = (points[i].get<1>().is_highlighted()) ? 0.5 : 0.0;
    float sl = (points[i].get<1>().is_selected()) ? 1.0 : 0.0;

    for(int j=0;j<4;j++) {
      highlights[4*i+j] = hl;
      selectednesses[4*i+j] = sl;
    }

  }

  // Generate buffers
  GL_CHECK(glGenBuffers(4, vbo));
  GL_CHECK(glGenVertexArrays(2, vao));

  // Set vertex position data
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo[0]));
  GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat)*points.size(), positions, GL_STATIC_DRAW));
  GL_CHECK(glBindVertexArray(vao[0]));

  GL_CHECK(glEnableVertexAttribArray(0));
  GL_CHECK(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0));

  // Set vertex texture data
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo[1]));
  GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat)*points.size(), tex_coords, GL_STATIC_DRAW));

  GL_CHECK(glEnableVertexAttribArray(1));
  GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0));

  // Set vertex highlight
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo[2]));
  GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat)*points.size(), highlights, GL_STATIC_DRAW));

  GL_CHECK(glEnableVertexAttribArray(2));
  GL_CHECK(glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0));

  // Set vertex selectednesses
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo[3]));
  GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat)*points.size(), selectednesses, GL_STATIC_DRAW));

  GL_CHECK(glEnableVertexAttribArray(3));
  GL_CHECK(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0));

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
  GL_CHECK(glDrawArrays(GL_QUADS, 0, 4*points.size()));

}

// Accessors
void Renderer::set_zoom(double _zoom) {
  zoom = _zoom;
}

TF::ImageToViewport Renderer::get_image_to_viewport_transform() {
  return TF::ImageToViewport(double(pixels->rows)/double(pixels->cols), vp_height/vp_width);
}

TF::Distortion Renderer::get_distortion_transform() {
  return TF::Distortion(zoom_center, zoom, sigma_x, sigma_y);
}

void Renderer::set_zoom_center(Point2D _zoom_center) {
  // _zoom_center is in viewport coords. Need in texture coords
  Point2D text_zoom_center = get_image_to_viewport_transform().inverse().t(_zoom_center);

  // Standardized distance
  // - Lines of constant s_distance appear as circles
  // at s_distance standard deviations from zoom_center
  double s_distance = sqrt(
    (text_zoom_center.x-zoom_center.x)*(text_zoom_center.x-zoom_center.x)/(sigma_x*sigma_x) +
    (text_zoom_center.y-zoom_center.y)*(text_zoom_center.y-zoom_center.y)/(sigma_y*sigma_y)
  );

  // Distance in texture coords
  double distance = sqrt(
    (text_zoom_center.x-zoom_center.x)*(text_zoom_center.x-zoom_center.x) +
    (text_zoom_center.y-zoom_center.y)*(text_zoom_center.y-zoom_center.y)
  );

  if (s_distance >= 0.5) {
    Point2D edge_point(
      zoom_center.x + sigma_x*(text_zoom_center.x - zoom_center.x)*(s_distance-0.5)/distance,
      zoom_center.y + sigma_y*(text_zoom_center.y - zoom_center.y)*(s_distance-0.5)/distance
    );
    // edge_point is in texture_coordinates
    zoom_center = edge_point;
  }
}

Point2D Renderer::as_image_coords(Point2D pt) {
  // Fwd distortion transform defined as
  // Position in image part of viewport -> position on actual image
  //
  // Our question is, for a point on the image display space,
  // where does it lie in the image
  return TF::combine(
    get_image_to_viewport_transform().inverse(),
    get_distortion_transform()
  ).t(pt);
}

Point2D Renderer::as_viewport_coords(Point2D pt) {
  // For a point on the image, where does it lie in the viewport
  return TF::combine(
    get_distortion_transform().inverse(),
    get_image_to_viewport_transform()
  ).t(pt);
}

/*
boost::geometry::model::box<
  boost::geometry::model::point<
    double, 2, boost::geometry::cs::cartesian
  >
>*/
boost::geometry::model::polygon<
  boost::geometry::model::point<
    double, 2, boost::geometry::cs::cartesian
  >
>
Renderer::get_reverse_marker_bounds(const Core::Point2D &point) {

  // assume that pt is in viewport coordinates.
  // output in image coordinates

  typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> pt_t;

  double rx = sqrt(2)*double(sprite_pixels->cols)*0.5/vp_width;
  double ry = sqrt(2)*double(sprite_pixels->rows)*0.5/vp_height;

  boost::geometry::model::polygon<pt_t> result;

  for(int i=0;i<4;i++) {
    double theta = -M_PI*0.25+i*M_PI*0.5;

    Core::Point2D box_pt(
      point.x + rx*cos(theta),
      point.y + ry*sin(theta)
    );

    Core::Point2D tf_box_pt(as_image_coords(box_pt));

    pt_t boost_pt(tf_box_pt.x, tf_box_pt.y);

    result.outer().push_back(boost_pt);
  }

  return result;
}

}}}
