#define BOOST_TEST_MODULE insight3d
#include <boost/test/included/unit_test.hpp>
#include <src/importers/insight3d.h>
#include <iostream>

//____________________________________________________________________________//
//
BOOST_AUTO_TEST_CASE( insight3d_importer )
{

  std::fstream file("test/files/test0.i3d");
  Importers::Insight3dImporter test_importer(file, std::cout);

  boost::shared_ptr<Core::Scene> scene;

  scene = test_importer.import();

  BOOST_CHECK_EQUAL( scene->images.size(), 2 );
  BOOST_CHECK_EQUAL( scene->images[0]->path, "/path/to/image0.jpg" );
  BOOST_CHECK_EQUAL( scene->images[0]->name, "image_name0.jpg" );
  BOOST_CHECK_EQUAL( scene->images[0]->points.size(), 3);
  BOOST_CHECK_CLOSE( scene->images[0]->points[2].x, 0.25, 1e-6 );
  BOOST_CHECK_CLOSE( scene->images[0]->points[2].y, 0.6, 1e-6 );

}

//____________________________________________________________________________//

