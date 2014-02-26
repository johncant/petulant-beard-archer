#ifndef __CORE__IMAGE_H__
#define __CORE__IMAGE_H__

#include <vector>
#include <string>
#include "point2d.h"

namespace Core {
  class Image {
    public:
    void add_point(Point2D const& pt) {
      points.push_back(pt);
    }

    std::string path;

    bool is_projection_matrix_known();

    std::vector<Core::Point2D> points;
    private:
    bool _is_projection_matrix_known;
//    cv::Image _image// Or other
//    cv::Mat<double> _projection_matrix;

  };
}

#endif
