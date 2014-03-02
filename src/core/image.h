#ifndef __CORE__IMAGE_H__
#define __CORE__IMAGE_H__

#include <vector>
#include <string>
#include "point2d.h"
#include <opencv2/opencv.hpp>

namespace Core {
  class Image {
    public:
    template <class point_class>
    void add_point(point_class const& pt) {
      points.push_back(Core::Point2D(pt));
    }

    std::string path;

    boost::shared_ptr<cv::Mat> pixels() {
      return boost::shared_ptr<cv::Mat>(new cv::Mat(cv::imread(path, CV_LOAD_IMAGE_COLOR)));
    }

    std::vector<Core::Point2D> points;

  };
}

#endif
