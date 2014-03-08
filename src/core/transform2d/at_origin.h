#ifndef __TRANSFORM2D_AT_ORIGIN_H__
#define __TRANSFORM2D_AT_ORIGIN_H__

#include "base.h"
#include "translation.h"
#include "transform.h"

#define TRANSFORM2D_AT_ORIGIN_CONSTRUCT_BASE(_ox, _oy, ...) \
CombinationBase<\
  CombinationBase<\
    Translation,\
    other\
  >,\
  Translation\
>(\
  CombinationBase<Translation, other> (\
    Translation(-_ox, -_oy),\
    other(__VA_ARGS__)\
  ),\
  Translation(_ox, _oy)\
)\

namespace Core {

  namespace Transform2D {

    template <class other>
    class AtOriginBase;

    template <class other>
    struct AtOrigin;

    template <class other>
    class AtOriginBase : public CombinationBase<CombinationBase<Translation, other>, Translation> {
      public:

      typedef Transform<AtOriginBase<
        typename other::Inverse
      > > Inverse;

      inline Inverse inverse() {
        return Inverse(this->t2.tx, this->t2.ty, this->t1.t2.inverse());
      }

      template <class CA1>
      inline AtOriginBase(double _ox, double _oy, CA1 ca1) :
      TRANSFORM2D_AT_ORIGIN_CONSTRUCT_BASE(_ox, _oy, ca1) {
      }

      template <class CA1, class CA2>
      inline AtOriginBase(double _ox, double _oy, CA1 ca1, CA2 ca2) :
      TRANSFORM2D_AT_ORIGIN_CONSTRUCT_BASE(_ox, _oy, ca1) {
      }

      template <class CA1, class CA2, class CA3>
      inline AtOriginBase(double _ox, double _oy, CA1 ca1, CA2 ca2, CA3 ca3) :
      TRANSFORM2D_AT_ORIGIN_CONSTRUCT_BASE(_ox, _oy, ca1, ca2, ca3) {
      }

      template <class CA1, class CA2, class CA3, class CA4>
      inline AtOriginBase(double _ox, double _oy, CA1 ca1, CA2 ca2, CA3 ca3, CA4 ca4) :
      TRANSFORM2D_AT_ORIGIN_CONSTRUCT_BASE(_ox, _oy, ca1, ca2, ca3, ca4) {
      }

      template <class CA1>
      inline AtOriginBase(Core::Point2D pt, CA1 ca1) :
      TRANSFORM2D_AT_ORIGIN_CONSTRUCT_BASE(pt.x, pt.y, ca1) {
      }

      template <class CA1, class CA2>
      inline AtOriginBase(Core::Point2D pt, CA1 ca1, CA2 ca2) :
      TRANSFORM2D_AT_ORIGIN_CONSTRUCT_BASE(pt.x, pt.y, ca1, ca2) {
      }

      template <class CA1, class CA2, class CA3>
      inline AtOriginBase(Core::Point2D pt, CA1 ca1, CA2 ca2, CA3 ca3) :
      TRANSFORM2D_AT_ORIGIN_CONSTRUCT_BASE(pt.x, pt.y, ca1, ca2, ca3) {
      }

      template <class CA1, class CA2, class CA3, class CA4>
      inline AtOriginBase(Core::Point2D pt, CA1 ca1, CA2 ca2, CA3 ca3, CA4 ca4) :
      TRANSFORM2D_AT_ORIGIN_CONSTRUCT_BASE(pt.x, pt.y, ca1, ca2, ca3, ca4) {
      }

    };

    template <class other>
    struct AtOrigin {
      typedef Transform<AtOriginBase<other> > type;
    };
  }
}

#undef __TRANSFORM2D_AT_ORIGIN_CONSTRUCT_BASE__

#endif

