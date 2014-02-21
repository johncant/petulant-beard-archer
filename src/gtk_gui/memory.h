namespace Persistence {
  class Memory {

  };
}

#include <vector>

class Image {
};

class Point2D {
  Image& im;
};

class FundamentalMatrix {
};

class Projection {
  Image& i1;
  Image& i2;
};

class Correspondence {
  std::vector<Point2D> points;
};

class Solver {
  public:
  std::vector<Image> &images;
  std::vector<Correspondence> correspondences;
  std::vector<Projection> projections;

  Solver(std::vector<Image> &_images) :images(_images) {
  }

  void solve() {
    // Build a huge matrix. 
    // Eight point algo:
    // eT y = 0 # Solve separately
    // [e1 e2 ...] T [ y1 y2 y3 ... ] = 0
    // eT y = a
    // minimise eT y (eT y)T = eT y yT e
    // Use SVD.
    //
    // (P3,1-3 X x - P1,1-3) X = 0
    //
    // (P3,1-3 x - P1,1-3) X = P1,4 - P3,4 x
    // (P3,1-3 y - P2,1-3) X = P2,4 - P3,4 x

    // A X = b
    //
    // [ A1  0   0 ]
    // [ A1  0   0 ]
    // [ 0  A2  0  ] X1
    // [ 0  A2  0  ] X2 = b
    // [ 0   0  A3 ] X3
    // [ 0   0  A3 ]
    //
    // Or, minimize s.t. constraints are met
    //
    // // Gives us
    // (A^T A) X = AT b
    // Now introduce hard constraints
    // (X1 - X2) T plane = 0
    // (X1 - X2) T line perp 1
    // (X1 - X2) T line perp 2
    // Linear and planar constraints - directionality, coplanarity, 
    //
    // Gives us circular constraints where the plane is known

    Mat<double>
    for (std::Vector<Correspondence>::iterator i = correspondences.begin(); i != correspondences.end(); i++) {
      correspondences
    }
  }
};


int main(void) {

  std::vector<Image> images;

  Solver s(images);

  // Read images (lists of points)

  s.solve();

}
