#include "base.h"
#include "../core/scene.h"

namespace Importers {
  std::ostream null_logger(0);
}

Importers::Base::Base(std::istream &_file, std::ostream &_logger) : file(_file), logger(_logger) {
}

// TODO - Is this an instance of the "bad" example in the docs?

boost::shared_ptr<Core::Scene> Importers::Base::import() {
};
