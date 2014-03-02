#include "image_view_renderer.h"
//#include <boost/phoenix.hpp>
//#include <boost/function.hpp>
//#include <boost/mpl/if.hpp>
//#include <boost/mpl/equal.hpp>
#include <string>

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092

//namespace OpenGLSafety {

//  struct ProxyGenerator {
//    inline void check_error() {
//      GLenum error = glGetError();
//      if (error != GL_NO_ERROR) {
//        std::cout << "Error on line " << __LINE__ << ": " << error << std::endl;
//      }
//    }
//
//    template <typename lambda_type>
//    struct OpenGLProxyBase {
//      lambda_type func;
//
//      OpenGLProxyBase(OpenGLSafety::ProxyGenerator &self, lambda_type &lambda) : func(build(self, lambda)) {}
//
//      private:
//      static lambda_type build(OpenGLSafety::ProxyGenerator& self, lambda_type lambda) {
//        using namespace boost::phoenix::local_names;
//        boost::phoenix::let(_a = boost::phoenix::val(lambda))[
//          boost::phoenix::bind(&OpenGLSafety::ProxyGenerator::check_error, &self),
//          _a
//        ];
//      }
//    };
//
//    template <typename RT>
//    struct OpenGLProxy0 : public OpenGLProxyBase <boost::function<RT()> > {
////      OpenGLProxy0(OpenGLSafety::ProxyGenerator& self, lambda_type lambda) : OpenGLProxy0(self, lambda) {}
//      RT operator()() { return val(this->func)()(); }
//    };
//
//    public:
//    template <typename RT>
//    inline OpenGLProxy0<boost::function<RT()> > operator[](RT(*func)()) {
//      return OpenGLProxy0<boost::function<RT()> >(*this, boost::phoenix::bind(*func));
//    }
//
//
//    template <typename RT, typename T1, typename T2>
//    inline boost::function<RT(T1, T2)> operator[](RT(*func)(T1, T2)) {
//      using boost::mpl::if_;
//      using boost::mpl::equal;
//      return if_<equal<RT, void>,
//        OpenGLProxy<boost::function<RT(T1, T2)> >,
//        OpenGLProxy<boost::function<RT(T1, T2)> >
//      >::type::build(*this, boost::phoenix::bind(*func, T1, T2));
//    }
//
//    template <typename RT, typename T1, typename T2, typename T3>
//    inline boost::function<RT(T1, T2, T3)> operator[](RT(*func)(T1, T2, T3)) {
//      using boost::mpl::if_;
//      using boost::mpl::equal;
//      return if_<equal<RT, void>,
//        OpenGLProxy<boost::function<RT(T1, T2, T3)> >,
//        OpenGLProxy<boost::function<RT(T1, T2, T3)> >
//      >::type::build(*this, boost::phoenix::bind(*func, T1, T2, T3));
//    }
//
//    template <typename RT, typename T1, typename T2, typename T3, typename T4>
//    inline boost::function<RT(T1, T2, T3, T4)> operator[](RT(*func)(T1, T2, T3, T4)) {
//      using boost::mpl::if_;
//      using boost::mpl::equal;
//      return if_<equal<RT, void>,
//        OpenGLProxy<boost::function<RT(T1, T2, T3, T4)> >,
//        OpenGLProxy<boost::function<RT(T1, T2, T3, T4)> >
//      >::type::build(*this, boost::phoenix::bind(func));
//    }
//
//    template <typename RT, typename T1, typename T2, typename T3, typename T4, typename T5>
//    inline boost::function<RT(T1, T2, T3, T4, T5)> operator[](RT(*func)(T1, T2, T3, T4, T5)) {
//      using boost::mpl::if_;
//      using boost::mpl::equal;
//      return if_<equal<RT, void>,
//        OpenGLProxy<boost::function<RT(T1, T2, T3, T4, T5)> >,
//        OpenGLProxy<boost::function<RT(T1, T2, T3, T4, T5)> >
//      >::type::build(*this, boost::phoenix::bind(func));
//    }
//
//    template <typename RT, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
//    inline boost::function<RT(T1, T2, T3, T4, T5, T6)> operator[](RT(*func)(T1, T2, T3, T4, T5, T6)) {
//      using boost::mpl::if_;
//      using boost::mpl::equal;
//      return if_<equal<RT, void>,
//        OpenGLProxy<boost::function<RT(T1, T2, T3, T4, T5, T6)> >,
//        OpenGLProxy<boost::function<RT(T1, T2, T3, T4, T5, T6)> >
//      >::type::build(*this, boost::phoenix::bind(func));
//    }

//    template <typename RT, typename T1>
//    inline boost::function<RT(T1)> operator[](RT(&func)(T1)) {
//      namespace phx = boost::phoenix::placeholders;
//      using boost::mpl::if_;
//      using boost::mpl::equal;
//      return if_<equal<RT, void>,
//        OpenGLProxy<boost::function<RT(T1)> >,
//        OpenGLProxy<boost::function<RT(T1)> >
//      >::type::build(*this, boost::phoenix::bind(func, phx::_1));
//    }
//
//    template <typename RT, typename T1, typename T2>
//    inline boost::function<RT(T1, T2)> operator[](RT(&func)(T1, T2)) {
//      namespace phx = boost::phoenix::placeholders;
//      using boost::mpl::if_;
//      using boost::mpl::equal;
//      return if_<equal<RT, void>,
//        OpenGLProxy<boost::function<RT(T1, T2)> >,
//        OpenGLProxy<boost::function<RT(T1, T2)> >
//      >::type::build(*this, boost::phoenix::bind(func, phx::_1, phx::_2));
//    }
//
//    template <typename RT, typename T1, typename T2, typename T3>
//    inline boost::function<RT(T1, T2, T3)> operator[](RT(&func)(T1, T2, T3)) {
//      using namespace boost::phoenix::placeholders;
//      using boost::mpl::if_;
//      using boost::mpl::equal;
//      return if_<equal<RT, void>,
//        OpenGLProxy<boost::function<RT(T1, T2, T3)> >,
//        OpenGLProxy<boost::function<RT(T1, T2, T3)> >
//      >::type::build(*this, boost::phoenix::bind(func, _1, _2, _3));
//    }
//
//    template <typename RT, typename T1, typename T2, typename T3, typename T4>
//    inline boost::function<RT(T1, T2, T3, T4)> operator[](RT(&func)(T1, T2, T3, T4)) {
//      using namespace boost::phoenix::placeholders;
//      using boost::mpl::if_;
//      using boost::mpl::equal;
//      return if_<equal<RT, void>,
//        OpenGLProxy<boost::function<RT(T1, T2, T3, T4)> >,
//        OpenGLProxy<boost::function<RT(T1, T2, T3, T4)> >
//      >::type::build(*this, boost::phoenix::bind(func, _1, _2, _3, _4));
//    }

//  };

//  ProxyGenerator safe;

//}

//using OpenGLSafety::safe;

 const std::string vertex_shader_source = ""
"#version 130\n"
"\n"
"precision mediump float;\n"
"in vec2 corners;\n"
"mat4 ortho = mat4( 2.0,  0.0, 0.0,  0.0,\n"
"                   0.0, -2.0, 0.0,  0.0,\n"
"                   0.0,  0.0, 0.0,  0.0,\n"
"                  -1.0,  1.0, 0.0,  1.0);\n"
"varying vec2 uv;\n"
"\n"
"\n"
"void main() {\n"
"  gl_Position = ortho*vec4(corners[0], corners[1], 0.0, 1.0);\n"
"  uv = vec2(corners[0], corners[1]);\n"
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
"  //FragColor = vec4(0.0, 1.0, 1.0, 1.0);\n"
"}\n";

GLuint GtkGui::ImageViewRenderer::compile(const std::string& source, GLenum type) {
  const GLchar* c_source = source.c_str();
  return compile(c_source, type);
}

GLuint GtkGui::ImageViewRenderer::compile(const char* c_source, GLenum type) {
  GLuint shader = glCreateShader(type);

  glShaderSource(shader, 1, &c_source, 0);

  glCompileShader(shader);

  GLint success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::cout << "Shader compilation failed" << std::endl;

    GLchar error[2048];
    GLsizei size;
    glGetShaderInfoLog(shader, 2048, &size, error);

    std::cout << error << std::endl;
    //exit(1); // TODO - need better error handling
  }

  return shader;
}

void GtkGui::ImageViewRenderer::init_shaders() {
  std::cout << "OpenGL version is: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

//  "out vec3 color;\n"
//  "out vec3 color;\n"
//  "uniform sampler2D image_sampler;\n"
//\
//void main() {\
//\
//  color = texture( image_sampler, UV ).rgb;\
//}\
//";

//  GLenum glew_status = safe[glewInit]();
//  if (glew_status != GLEW_OK) {
//    std::cout << "glew failed to initialize" << std::endl;
//    exit(1);
//  }
//
//  shader_program = safe[glCreateProgram]();
//  vertex_shader = compile(vertex_shader_source, GL_VERTEX_SHADER);
//  std::cout << "Vertex shader compiled" << std::endl;
//
//  fragment_shader = compile(fragment_shader_source, GL_FRAGMENT_SHADER);
//  std::cout << "Fragment shader compiled" << std::endl;
//
//  safe[glAttachShader](shader_program, vertex_shader);
//  safe[glAttachShader](shader_program, fragment_shader);
//
//  safe[glBindAttribLocation](shader_program, 0, "corners");
//
//  safe[glLinkProgram](shader_program);
//
//  GLint link_success = 0;
//  safe[glGetProgramiv](shader_program, GL_LINK_STATUS, &link_success);
//
//  if (!link_success) {
//    std::cout << "Program linking failed" << std::endl;
//
//    GLchar error[2048];
//    GLsizei size;
//    safe[glGetProgramInfoLog](shader_program, 2048, &size, error);
//
//    std::cout << error << std::endl;
//    exit(1); // TODO - need better error handling
//
//  }
//
//  std::cout << "Shader program linked successfully" << std::endl;


  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK) {
    std::cout << "glew failed to initialize" << std::endl;
    exit(1);
  }

  shader_program = glCreateProgram();
  vertex_shader = compile(vertex_shader_source, GL_VERTEX_SHADER);
  std::cout << "Vertex shader compiled" << std::endl;

  fragment_shader = compile(fragment_shader_source, GL_FRAGMENT_SHADER);
  std::cout << "Fragment shader compiled" << std::endl;

  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);


  glBindAttribLocation(shader_program, 0, "corners");

  glLinkProgram(shader_program);

  GLint link_success = 0;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &link_success);

  if (!link_success) {
    std::cout << "Program linking failed" << std::endl;

    GLchar error[2048];
    GLsizei size;
    glGetProgramInfoLog(shader_program, 2048, &size, error);

    std::cout << error << std::endl;
    exit(1); // TODO - need better error handling

  }

  std::cout << "Shader program linked successfully" << std::endl;

}

GtkGui::ImageViewRenderer::ImageViewRenderer(boost::shared_ptr<Core::Image> im) :
  Renderer(),
  image(im),
  pixels(image->pixels()),
  gl_tex_id(0),
  shader_program(0),
  fragment_shader(0),
  vertex_shader(0) {
}

GtkGui::ImageViewRenderer::~ImageViewRenderer() {
  if (gl_tex_id) {
    glDeleteTextures(1, &gl_tex_id);
    // TODO
  }
}

void GtkGui::ImageViewRenderer::realize() {
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) std::cout << "Error on line " << __LINE__ << ": " << error << std::endl;

  glDisable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
//  glEnable(GL_CULL_FACE);
//  glCullFace(GL_BACK);
  glDisable(GL_DITHER);
  glShadeModel(GL_FLAT);

  error = glGetError();
  if (error != GL_NO_ERROR) std::cout << "Error on line " << __LINE__ << ": " << error << std::endl;

  init_shaders();
  error = glGetError();
  if (error != GL_NO_ERROR) std::cout << "Error on line " << __LINE__ << ": " << error << std::endl;

  if (!pixels) { return; };

  glGenTextures(1, &gl_tex_id);
  glBindTexture(GL_TEXTURE_2D, gl_tex_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Assume RGB
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGB,
               pixels->cols,
               pixels->rows,
               0,
               GL_BGR,
               GL_UNSIGNED_BYTE,  // Image data type
               pixels->ptr()
  );
}

void GtkGui::ImageViewRenderer::configure(unsigned int width, unsigned int height) {
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) std::cout << "Error on line " << __LINE__ << ": " << error << std::endl;

  glViewport(0, 0, width, height);

  error = glGetError();
  if (error != GL_NO_ERROR) std::cout << "Error on line " << __LINE__ << ": " << error << std::endl;

}

void GtkGui::ImageViewRenderer::draw() {
  GLuint vbo = 0, vao = 0;

  // Yoink. Not a clue how to bind things.
  // https://www.opengl.org/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) std::cout << "Error on line " << __LINE__ << ": " << error << std::endl;

  glUseProgram(shader_program);
  glClear(GL_DEPTH_BUFFER_BIT);

  const float positions[] = {
    0.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0
  };


  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0.0, 1.0, 1.0, 0.0, -2, 2);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();


  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

//  glVertexPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glBindVertexArray(vao);

  glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), positions, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // Texture
  GLint img_loc = glGetUniformLocation(shader_program, "image");
  glActiveTextureARB(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gl_tex_id);
  glEnable(GL_TEXTURE_2D);
  glUniform1iARB(img_loc, 0);


  error = glGetError();
  if (error != GL_NO_ERROR) std::cout << "Error on line " << __LINE__ << ": " << error << std::endl;


  error = glGetError();
  if (error != GL_NO_ERROR) std::cout << "Error on line " << __LINE__ << ": " << error << std::endl;

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawArrays(GL_QUADS, 0, 4);

//  glColor3f(0, 1, 0);
//  glBegin(GL_QUADS);
//    glVertex2f(0.3, 0.2);
//    glVertex2f(0.2, 0.8);
//    glVertex2f(0.8, 0.8);
//    glVertex2f(0.7, 0.2);
//  glEnd();

  error = glGetError();
  if (error != GL_NO_ERROR) std::cout << "Error on line " << __LINE__ << ": " << error << std::endl;
}
