#include <iostream>
#include <boost/shared_ptr.hpp>
#include "base.h"

namespace Importers {
  class Insight3dImporter : public Importers::Base {
    public:
    Insight3dImporter(std::istream &_file, std::ostream &_logger) : Base(_file, _logger) { }
    virtual boost::shared_ptr<Core::Scene> import();
  };
}
