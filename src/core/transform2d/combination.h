#ifndef __TRANSFORM2D_COMBINATION_BASE_H__
#define __TRANSFORM2D_COMBINATION_BASE_H__

#include "base.h"
#include "transform.h"

namespace Core {

  namespace Transform2D {

    template <class T1, class T2>
    class CombinationBase;

    template <class T1, class T2>
    class CombinationBase : public Base {
      public:
      typedef Transform<CombinationBase<typename T2::Inverse, typename T1::Inverse> > Inverse;
      T1 t1;
      T2 t2;

      inline Core::Point2D t(Core::Point2D input) {
        return t2.t(t1.t(input));
      }

      inline CombinationBase(T1 _transform1, T2 _transform2) :
        t1(_transform1), t2(_transform2) {
      }

      inline Inverse inverse() {
        return Inverse(t2.inverse(), t1.inverse());
      }
    };

    template <typename T1, typename T2>
    struct Combination {
      typedef Transform<CombinationBase<T1, T2> > type;
    };

    template <class T1, class T2> Transform<CombinationBase<T1, T2> > combine(T1 t1, T2 t2) {
      return typename Combination<T1, T2>::type(t1, t2);
    }

  }
}

#endif
