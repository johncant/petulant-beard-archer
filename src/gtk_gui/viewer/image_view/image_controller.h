#ifndef __GTK_GUI_VIEWER_IMAGE_VIEW_IMAGE_CONTROLLER_H__
#define __GTK_GUI_VIEWER_IMAGE_VIEW_IMAGE_CONTROLLER_H__

#include <boost/shared_ptr.hpp>
#include "image_point_index_mixin.h"
#include "../../../core/image.h"

namespace GtkGui {
namespace Viewer {
namespace ImageView {


class ImageControllerBase {
  public:
  boost::shared_ptr<Core::Image> image;
  ImageControllerBase(boost::shared_ptr<Core::Image> im) :
    image(im) {
  }
};

// Mixins for abstracting access to image data and making it efficient
template <typename point_user_data_t>
class ImageController :
  public ImagePointIndexMixin<ImageControllerBase, point_user_data_t> {
  public:
  ImageController(boost::shared_ptr<Core::Image> im) :
    ImagePointIndexMixin<ImageControllerBase, point_user_data_t>(im)
  {
  }
};


}}}


#endif
