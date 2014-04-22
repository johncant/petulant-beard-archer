#ifndef __TRANSFORM2D__
#define __TRANSFORM2D__


// Matrixless 2D stackable transformations.
// Stack them with nonlinear transformations.

// TODO - Override combinations of affine
// transformations for efficiency by building matrices.

#include "transform2d/base.h"
#include "transform2d/translation.h"
#include "transform2d/scaling.h"
#include "transform2d/bisection_unscale.h"
#include "transform2d/combination.h"
#include "transform2d/at_origin.h"
#include "transform2d/transform.h"

#endif
