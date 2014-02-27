#ifndef __CORE_PROJECT_H__
#define __CORE_PROJECT_H__

#include <vector>
#include <boost/shared_ptr.hpp>
#include "image.h"

namespace Core {
  class Scene {
    public:
    std::vector<boost::shared_ptr<Core::Image> > images;

    void add_image(boost::shared_ptr<Core::Image> image) {
      images.push_back(image);
    }
  };
}

#endif
