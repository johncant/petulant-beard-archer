//#include "gtk_gui.h"
//#include "cli.h"
#include "gtk_gui/gtk_gui.h"
#include <vector>
#include <string>
#include "importers/insight3d.h"
#include <fstream>
#include <iostream>

#include <boost/phoenix.hpp>
#include <algorithm>

using namespace std;
using namespace boost::phoenix;
using namespace boost::phoenix::arg_names;

int main(int argc, char** argv) {

  // TODO - boost will try to free cout and file. oops.
  std::ifstream file("enta4.i3d", std::fstream::in);

  Importers::Insight3dImporter i(file, std::cout);

  boost::shared_ptr<Core::Scene> s = i.import();

  cout << s->images.size() << "images" << endl;

  cout << "START Images:" << endl;

  for_each(s->images.begin(), s->images.end(), (
    ref(cout) << bind(&Core::Image::path, *_1),
    ref(cout) << " ",
    ref(cout) << bind(&std::vector<Core::Point2D>::size, bind(&Core::Image::points, *_1)),
    ref(cout) << endl
  ));

  cout << "END Images" << endl;

  GtkGui::main(argc, argv);

}
