#ifndef __POINT2D_H__
#define __POINT2D_H__

namespace Core {
  class Point2D {
    public:
    double x;
    double y;
    Point2D(double x_, double y_) : x(x_), y(y_) {
    }
  };
}

#endif
