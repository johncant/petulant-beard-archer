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

boost::shared_ptr<Core::Scene> Importers::Insight3dImporter::import() {
  using boost::spirit::qi::double_;
  using boost::spirit::qi::uint_;
  using boost::spirit::qi::char_;
  using boost::spirit::qi::lit;
  using boost::spirit::qi::eol;
  using boost::spirit::qi::phrase_parse;
  using boost::spirit::qi::eps;
  using boost::spirit::ascii::space;
  using boost::spirit::repeat;
  using boost::spirit::as_string;
  namespace qi = boost::spirit::qi;
  using namespace boost::phoenix;
  using namespace boost::phoenix::local_names;
  using boost::shared_ptr;
  using boost::weak_ptr;
  using boost::spirit::locals;

  // Set the iterators up
  typedef boost::spirit::line_pos_iterator<boost::spirit::istream_iterator> iterator_type;

  boost::spirit::istream_iterator s_begin(file);
  boost::spirit::istream_iterator s_current(file);
  boost::spirit::istream_iterator s_end;

  // This does not work - always returns line 1 col 1
  iterator_type begin(s_begin);
  iterator_type current(s_current);
  iterator_type end(s_end);

  file.unsetf(std::ios::skipws);

  // TODO - This can't quite be right. Don't put obscure chars in your filenames
  const qi::rule<iterator_type, std::string()> file_name =
    as_string[
      repeat[
        char_("A-Za-z_0-9./") | lit("\\ ")
      ]
    ][qi::_val = qi::_1];

  // Parse 2d image points
  const qi::rule<iterator_type, shared_ptr<Core::Point2D>(), locals<double, double, unsigned int> > point(
    double_[boost::spirit::_a = qi::_1] >> +space >> // x
    double_[boost::spirit::_b = qi::_1] >> +space >> // y
    uint_[boost::spirit::_c = qi::_1] >>             // vertex id
    eol[
      qi::_val =
      // Build the pointer
      construct<shared_ptr<Core::Point2D> >(
        new_<Core::Point2D>(boost::spirit::_a, boost::spirit::_b)
      )
    ]
  );

  // Parse Images.
  const qi::rule<iterator_type, shared_ptr<Core::Image>(),
    locals<
      shared_ptr<Core::Image>, // _a image
      int                      // _b point count
    >
  > image = (
    eps[
      // New image. Need one for holding onto points. Deleted on failure.
      boost::spirit::_a = construct<shared_ptr<Core::Image> >(new_<Core::Image>()),
      boost::spirit::_b = 0
    ] >>
    // 6 unknown numbers
    repeat(6)[
      uint_ >> +space
    ] >>
    // Path to image file
    file_name[
      bind(&Core::Image::path, *boost::spirit::_a) = construct<std::string>(qi::_1),
      ref(std::cout) << qi::_1 << std::endl
    ] >> +space >>
    // Unknown number
    uint_ >> +space >>
    // File name
    as_string[repeat(1,255)[char_("A-Za-z0-9.") | "\\ "]][
      ref(std::cout) << qi::_1 << std::endl
    ] >> +space >>
    // Projection matrix?
    repeat(16)[double_ >> *(" ")] >> eol >>
    // Points count
    "points" >> +space >> uint_[
      boost::spirit::_b = qi::_1,
      ref(std::cout) << "Image contains " << qi::_1 << " points" << std::endl
    ] >> eol >>
    // Points
    repeat(boost::spirit::_b)[
      point[bind(&Core::Image::add_point, *boost::spirit::_a, *qi::_1)]
    ] >>
    eps[qi::_val = boost::spirit::_a]
  );

  // Parse Insight3d project
  qi::rule<iterator_type, shared_ptr<Core::Scene>(), locals<
    shared_ptr<Core::Scene>,
    int, // _a = vertex count
    int, // _b = polygon count
    int  // _c = image_count
  > > scene = (
    eps[boost::spirit::_a = construct<shared_ptr<Core::Scene> >(new_<Core::Scene>())] >> // Result
    eps[boost::spirit::_b = 0] >> // Vertex count
    eps[boost::spirit::_c = 0] >> // Polygon count (0)
    eps[boost::spirit::_d = 0] >> // Image count
    lit("insight3d data file") >> eol >>
    lit("vertices") >> +space >> uint_[boost::spirit::_b = qi::_1, ref(std::cout) << "File contains " << qi::_1 << " vertices" << std::endl] >> eol >>
    repeat(boost::spirit::_b)[
      uint_ >> +space >>
      uint_ >> +space >>
      uint_ >> +space >>
      uint_ >> +space >>
      uint_ >> eol] >>
    lit("polygons") >> +space >> uint_[boost::spirit::_c = qi::_1, ref(std::cout) << "File contains " << qi::_1 << " polygons" << std::endl] >> eol >>
    /* TODO */
    lit("shots") >> +space >> uint_[boost::spirit::_d = qi::_1, ref(std::cout) << "File contains " << qi::_1 << " shots" << std::endl] >> eol >>
    repeat(boost::spirit::_d)[
      image[bind(&Core::Scene::add_image, *boost::spirit::_a, qi::_1)]
    ] >>
    lit("checksum") >> +space >> uint_[
      ref(std::cout) << "Checksum was " << qi::_1 << std::endl
    ] >>
    eps[qi::_val = boost::spirit::_a]
  );

  shared_ptr<Core::Scene> result;

  bool r = boost::spirit::qi::parse(current, end, scene[boost::phoenix::ref(result) = construct<shared_ptr<Core::Scene> >(qi::_1)]);

  if (!r && current != end) {
    std::cerr << "parse error on line " << boost::spirit::get_line(current) << ':' << get_column(begin, current, 1);
  }

  return result;
}
