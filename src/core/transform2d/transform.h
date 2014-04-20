#ifndef __TRANSFORM2D_TRANSFORM_H__
#define __TRANSFORM2D_TRANSFORM_H__

namespace Core {
  namespace Transform2D {

    template <class backend>
    class Transform : public backend {
      public:
      inline Core::Point2D t(double x, double y) {
        return backend::t(Point2D(x, y));
      }

      inline Core::Point2D t(Point2D pt) {
        return backend::t(pt);
      }

      inline Transform() : backend() {
      }

      typedef typename backend::Inverse Inverse;

      template <class CA1>
      inline Transform(CA1 ca1) : backend(ca1) {
      }

      template <class CA1, class CA2>
      inline Transform(CA1 ca1, CA2 ca2) : backend(ca1, ca2) {
      }

      template <class CA1, class CA2, class CA3>
      inline Transform(CA1 ca1, CA2 ca2, CA3 ca3) : backend(ca1, ca2, ca3) {
      }

      template <class CA1, class CA2, class CA3, class CA4>
      inline Transform(CA1 ca1, CA2 ca2, CA3 ca3, CA4 ca4) : backend(ca1, ca2, ca3, ca4) {
      }

      template <class CA1, class CA2, class CA3, class CA4, class CA5>
      inline Transform(CA1 ca1, CA2 ca2, CA3 ca3, CA4 ca4, CA5 ca5) : backend(ca1, ca2, ca3, ca4, ca5) {
      }

    };

  }

}

#endif
