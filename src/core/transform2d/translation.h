#ifndef __TRANSFORM2D_TRANSLATION_H__
#define __TRANSFORM2D_TRANSLATION_H__

#include "base.h"
#include "transform.h"

namespace Core {
  namespace Transform2D {

    class TranslationBase;

    class TranslationBase : public Base {
      public:
      typedef Transform<TranslationBase> Inverse;

      double tx;
      double ty;

      inline TranslationBase(double _tx, double _ty) : tx(_tx), ty(_ty) {
      }

      inline Core::Point2D t(Core::Point2D input) {
        return Core::Point2D(input.x+tx, input.y+ty);
      }

      inline Inverse inverse() {
        return Inverse(-tx, -ty);
      }
    };

    typedef Transform<TranslationBase> Translation;

  }
}

#endif
