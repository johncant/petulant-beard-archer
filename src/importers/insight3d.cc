#define FUSION_MAX_VECTOR_SIZE 44
#define BOOST_SPIRIT_USE_PHOENIX_V3 1
//#define FUSION_MAX_VECTOR_SIZE 56
#include <iostream>
#include "insight3d.h"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/phoenix.hpp>
#include <boost/function.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_stream.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include "../core/scene.h"
#include "../core/image.h"
#include "../core/point2d.h"

using boost::spirit::qi::double_;
using boost::spirit::qi::uint_;
using boost::spirit::qi::char_;
using boost::spirit::qi::lit;
using boost::spirit::qi::eol;
using boost::spirit::qi::phrase_parse;
using boost::spirit::qi::eps;
using boost::spirit::as_string;
using boost::spirit::repeat;
namespace qi = boost::spirit::qi;

using boost::spirit::locals;
namespace local {
  using boost::spirit::_a;
  using boost::spirit::_b;
  using boost::spirit::_c;
  using boost::spirit::_d;
}

using namespace boost::phoenix;
using namespace boost::phoenix::local_names;
namespace phx = boost::phoenix;

using boost::shared_ptr;
using boost::weak_ptr;

typedef boost::spirit::line_pos_iterator<boost::spirit::istream_iterator> iterator_type;


namespace Importer { namespace Insight3dImporter {

  struct Point {
    double x;
    double y;
    unsigned int vertex_id;

    Point(double px, double py, unsigned int vid): x(px), y(py), vertex_id(vid) { }
  };


  namespace Parsers {

    // Parse file name char
    static qi::rule<iterator_type, char()> file_name_char =
      char_("A-Za-z_0-9-_.") | lit("\\ ");


    // Parse file name
    static qi::rule<iterator_type, std::string()> file_name =
      as_string[+file_name_char];


    // Parse path
    static qi::rule<iterator_type, std::string()> path =
      repeat[lit("/") >> file_name];


    // Parse 3d vertices (but really, ignore them.)
    qi::rule<iterator_type, qi::blank_type> vertex =
      repeat(5)[uint_] >> eol;


    // Parse 2d image points
    static qi::rule<iterator_type, shared_ptr<Point>(), locals<double, double, unsigned int>, qi::blank_type> point =
      double_[local::_a = qi::_1] >> // x
      double_[local::_b = qi::_1] >> // y
      uint_[local::_c = qi::_1] >>   // vertex id
      eol[
        qi::_val =
        // Build the pointer
       phx::construct<shared_ptr<Point> >(
          new_<Point>(local::_a, local::_b, local::_c)
        )
      ];


    // Parse Projection matrix
    static qi::rule<iterator_type, void(), qi::blank_type> projection_matrix = 
    repeat(16)[double_];


    // Parse image
    typedef qi::rule<
      iterator_type,
      shared_ptr<Core::Image>(),
      qi::blank_type,
      locals<
        shared_ptr<Core::Image>, // _a image
        int                      // _b point count
      >
    > ImageParseRule;


    static ImageParseRule image =

      // Image line
      eps[
        // New image. Need one for holding onto points. Deleted on failure.
        local::_a = construct<shared_ptr<Core::Image> >(new_<Core::Image>()),
        local::_b = 0
      ] >>

      // 6 unknown numbers
      repeat(6)[uint_] >>

      // Path to image file
      path[
        bind(&Core::Image::path, *local::_a) = construct<std::string>(qi::_1),
        ref(std::cout) << qi::_1 << std::endl
      ] >>

      // Unknown number
      uint_ >>

      // File name
      file_name[
        ref(std::cout) << qi::_1 << std::endl
      ] >>

      // Projection matrix
      projection_matrix >> eol >>

      // Points count
      "points" >> uint_[
        local::_b = qi::_1,
        ref(std::cout) << "Image contains " << qi::_1 << " points" << std::endl
      ] >> eol >>

      // Points
      repeat(local::_b)[
        point[bind(&Core::Image::add_point<Point>, *local::_a, *qi::_1)]
      ] >>

      eps[qi::_val = local::_a];
    // End of imaage parser


    // Parse Insight3d project
    typedef qi::rule<
      iterator_type,
      shared_ptr<Core::Scene>(),
      qi::blank_type,
      locals<
        shared_ptr<Core::Scene>,
        int, // _b = vertex count
        int, // _c = polygon count
        int  // _d = image_count
      >
    > ProjectParseRule;



    static const ProjectParseRule project =
      eps[local::_a = construct<shared_ptr<Core::Scene> >(new_<Core::Scene>())] >> // Result
      eps[local::_b = 0] >> // Vertex count
      eps[local::_c = 0] >> // Polygon count (0)
      eps[local::_d = 0] >> // Image count
      lit("insight3d data file") >> eol >>

      // Vertex count
      lit("vertices") >> uint_[
        local::_b = qi::_1,
        ref(std::cout) << "File contains " << qi::_1 << " vertices" << std::endl
      ] >> eol >>

      // Vertices
      repeat(local::_b)[vertex] >>

      // Polygon count
      lit("polygons") >> uint_[
        local::_c = qi::_1,
        ref(std::cout) << "File contains " << qi::_1 << " polygons" << std::endl
      ] >> eol >>
      /* TODO - we haven't seen any polygons yet*/

      // Image count
      lit("shots") >> uint_[
        local::_d = qi::_1,
        ref(std::cout) << "File contains " << qi::_1 << " shots" << std::endl
      ] >> eol >>

      // Images
      repeat(local::_d)[
        image[bind(&Core::Scene::add_image, *local::_a, qi::_1)]
      ] >>

      // Checksum
      lit("checksum") >> uint_[
        ref(std::cout) << "Checksum was " << qi::_1 << std::endl
      ] >>

      // Set the return value
      eps[qi::_val = local::_a];

  }

} }



using namespace Importer::Insight3dImporter;

boost::shared_ptr<Core::Scene> Importers::Insight3dImporter::import() {

  file.unsetf(std::ios::skipws);

  // Set the iterators up
  boost::spirit::istream_iterator s_begin(file);
  boost::spirit::istream_iterator s_current(file);
  boost::spirit::istream_iterator s_end;

  //   This does not work - always returns line 1 col 1
  iterator_type begin(s_begin);
  iterator_type current(s_current);
  iterator_type end(s_end);

  shared_ptr<Core::Scene> result;

  bool r = boost::spirit::qi::phrase_parse(current, end, Parsers::project[phx::ref(result) = construct<shared_ptr<Core::Scene> >(qi::_1)], qi::blank);

  if (!r && current != end) {
    std::cerr << "parse error on line " << boost::spirit::get_line(current) << ':' << get_column(begin, current, 1);
  }

  return result;
}
