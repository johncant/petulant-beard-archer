#include <iostream>
#include <boost/shared_ptr.hpp>
#include "../core/scene.h"

namespace Importers {

  extern std::ostream null_logger;

  class Base {
    public:
    std::istream &file;
    std::ostream &logger;


    Base(std::istream &_file, std::ostream &_logger=Importers::null_logger);

    virtual boost::shared_ptr<Core::Scene> import()=0;
  };
}
