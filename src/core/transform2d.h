#ifndef __TRANSFORMATION2D__
#define __TRANSFORMATION2D__

// Matrixless 2D stackable transformations.
// Stack them with nonlinear transformations.

// TODO - Override combinations of affine
// transformations for efficiency by building matrices.

namespace Core {

  namespace Transform2D {

    class Base;

    template <class T1, class T2>
    class CombinationBase;

    class TranslationBase;
    class ScalingBase;

    template <class other>
    class AtOriginBase;

    template <class backend>
    class Transformation;

    class Base {
      // ... Not needed yet
    };

    template <class backend>
    class Transformation : public backend {
      public:
      inline Core::Point2D t(double x, double y) {
        return backend::t(Point2D(x, y));
      }

      inline Core::Point2D t(Point2D pt) {
        return backend::t(pt);
      }

      inline Transformation() : backend() {
      }

      typedef typename backend::Inverse Inverse;

      template <class CA1>
      inline Transformation(CA1 ca1) : backend(ca1) {
      }

      template <class CA1, class CA2>
      inline Transformation(CA1 ca1, CA2 ca2) : backend(ca1, ca2) {
      }

      template <class CA1, class CA2, class CA3>
      inline Transformation(CA1 ca1, CA2 ca2, CA3 ca3) : backend(ca1, ca2, ca3) {
      }

      template <class CA1, class CA2, class CA3, class CA4>
      inline Transformation(CA1 ca1, CA2 ca2, CA3 ca3, CA4 ca4) : backend(ca1, ca2, ca3, ca4) {
      }
    };

    template <class T1, class T2>
    class CombinationBase : public Base {
      public:
      typedef Transformation<CombinationBase<typename T2::Inverse, typename T1::Inverse> > Inverse;
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

    template <class T1, class T2> Transformation<CombinationBase<T1, T2> > combine(T1 t1, T2 t2) {
      return CombinationBase<T1, T2>(t1, t2);
    }

    class TranslationBase : public Base {
      public:
      typedef Transformation<TranslationBase> Inverse;

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

    typedef Transformation<TranslationBase> Translation;

    class ScalingBase : public Base {
      public:
      typedef Transformation<ScalingBase> Inverse;

      double scale_x;
      double scale_y;

      inline ScalingBase(const double _scale_x, const double _scale_y) : scale_x(_scale_x), scale_y(_scale_y) {
      }

      inline Core::Point2D t(Core::Point2D input) {
        return Core::Point2D(scale_x*input.x, scale_y*input.y);
      }

      inline Inverse inverse() {
        return Inverse(1.0/scale_x, 1.0/scale_y);
      }
    };

    typedef Transformation<ScalingBase> Scaling;

    template <class other>
    class AtOrigin : public CombinationBase<CombinationBase<Translation, other>, Translation> {
      public:

      typedef Transformation<AtOrigin<
        typename other::Inverse
      > > Inverse;

      inline AtOrigin(double _ox, double _oy, other _other) :
        CombinationBase<
          CombinationBase<
            Translation,
            other
          >,
          Translation
        >(
          CombinationBase<Translation, other> (
            Translation(-_ox, -_oy),
            _other
          ),
          Translation(_ox, _oy)
        ) {
      }

      inline Inverse inverse() {
        return Inverse(this->t2.tx, this->t2.ty, this->t1.t2.inverse());
      }
    };

  }
}

#endif
