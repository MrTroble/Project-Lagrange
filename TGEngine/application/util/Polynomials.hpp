#pragma once

#include <stdexcept>
#define THROW(x) throw std::runtime_error(x);

template <typename T = double> class PolynomialEntry {
public:
  static T P0functions(T x, int i) {
    if (i == 0)
      return T(1.0);
    else
      THROW("Not implemented!")
  }

  static T P0Dfunctions(T x, int i) {
    if (i == 0)
      return T(0.0);
    else
      THROW("Not implemented!")
  }

  static T P1functions(T x, int i) {
    switch (i) {
    case 0:
      return 1 - x;
    case 1:
      return x;
    default:
      THROW("Not implemented!")
    }
  }

  static T P1Dfunctions(T x, int i) {
    switch (i) {
    case 0:
      return T(-1.0);
    case 1:
      return T(1.0);
    default:
      THROW("Not implemented!")
    }
  }

  static T P2functions(T x, int i) {
    switch (i) {
    case 0:
      return (2 * x - 1) * (x - 1);
    case 1:
      return 4 * x * (1 - x);
    case 2:
      return x * (2 * x - 1);
    default:
      THROW("Not implemented!")
    }
  }

  static T P2Dfunctions(T x, int i) {
    switch (i) {
    case 0:
      return 4 * x - 3;
    case 1:
      return -8 * x + 4;
    case 2:
      return 4 * x - 1;
    default:
      THROW("Not implemented!")
    }
  }

  static T P3functions(T x, int i) {
    switch (i) {
    case 0:
      return -1 / T(2.0) * (3 * x - 1) * (3 * x - 2) * (x - 1);
    case 1:
      return 9 / T(2.0) * x * (3 * x - 2) * (x - 1);
    case 2:
      return -9 / T(2.0) * x * (3 * x - 1) * (x - 1);
    case 3:
      return 1 / T(2.0) * x * (3 * x - 1) * (3 * x - 2);
    default:
      THROW("Not implemented!")
    }
  }

  static T P3Dfunctions(T x, int i) {
    switch (i) {
    case 0:
      return -27 / T(2.0) * x * x + 18 * x - 11 / T(2.0);
    case 1:
      return 81 / T(2.0) * x * x - 45 * x + 9;
    case 2:
      return -81 / T(2.0) * x * x + 36 * x - 9 / T(2.0);
    case 3:
      return 27 / T(2.0) * x * x - 9 * x + 1;
    default:
      THROW("Not implemented!")
    }
  }

  static T P4functions(T x, int i) {
    switch (i) {
    case 0:
      return 1 / T(6.0) * (4 * x - 1) * (4 * x - 2) * (4 * x - 3) * (x - 1);
    case 1:
      return -8 / T(3.0) * x * (4 * x - 2) * (4 * x - 3) * (x - 1);
    case 2:
      return 4 * x * (4 * x - 1) * (4 * x - 3) * (x - 1);
    case 3:
      return -8 / T(3.0) * x * (4 * x - 1) * (4 * x - 2) * (x - 1);
    case 4:
      return 1 / T(6.0) * x * (4 * x - 1) * (4 * x - 2) * (4 * x - 3);
    default:
      THROW("Not implemented!")
    }
  }

  static T P4Dfunctions(T x, int i) {
    T x2 = x * x;
    T x3 = x2 * x;
    switch (i) {
    case 0:
      return 1 / T(3.0) * (128 * x3 - 240 * x2 + 140 * x - 25);
    case 1:
      return -1 / T(3.0) * (512 * x3 - 864 * x2 + 416 * x - 48);
    case 2:
      return 256 * x3 - 384 * x2 + 152 * x - 12;
    case 3:
      return -1 / T(3.0) * (512 * x3 - 672 * x2 + 224 * x - 16);
    case 4:
      return 1 / T(3.0) * (128 * x3 - 144 * x2 + 44 * x - 3);
    default:
      THROW("Not implemented!")
    }
  }

  static T P5functions(T x, int i) {
    switch (i) {
    case 0:
      return -1.0 / T(24.0) * (x - 1) * (5 * x - 4) * (5 * x - 3) *
             (5 * x - 2) * (5 * x - 1);
    case 1:
      return 25.0 / T(24.0) * x * (x - 1) * (5 * x - 4) * (5 * x - 3) *
             (5 * x - 2);
    case 2:
      return -25.0 / T(12.0) * x * (x - 1) * (5 * x - 4) * (5 * x - 3) *
             (5 * x - 1);
    case 3:
      return 25.0 / T(12.0) * x * (x - 1) * (5 * x - 4) * (5 * x - 2) *
             (5 * x - 1);
    case 4:
      return -25.0 / T(24.0) * x * (x - 1) * (5 * x - 3) * (5 * x - 2) *
             (5 * x - 1);
    case 5:
      return 1.0 / T(24.0) * x * (5 * x - 4) * (5 * x - 3) * (5 * x - 2) *
             (5 * x - 1);
    default:
      THROW("Not implemented!")
    }
  }

  static T P5Dfunctions(T x, int i) {
    T x2 = x * x;
    T x3 = x2 * x;
    T x4 = x2 * x2;
    switch (i) {
    case 0:
      return -1.0 / T(24.0) *
             (3125 * x4 - 7500 * x3 + 6375 * x2 - 2250 * x + 274);
    case 1:
      return 25.0 / T(24.0) * (625 * x4 - 1400 * x3 + 1065 * x2 - 308 * x + 24);
    case 2:
      return -25.0 / T(12.0) * (625 * x4 - 1300 * x3 + 885 * x2 - 214 * x + 12);
    case 3:
      return 25.0 / T(12.0) * (625 * x4 - 1200 * x3 + 735 * x2 - 156 * x + 8);
    case 4:
      return -25.0 / T(24.0) * (625 * x4 - 1100 * x3 + 615 * x2 - 122 * x + 6);
    case 5:
      return 1.0 / T(24.0) * (3125 * x4 - 5000 * x3 + 2625 * x2 - 500 * x + 24);
    default:
      THROW("Not implemented!")
    }
  }

  static T P6functions(T x, int i) {
    switch (i) {
    case 0:
      return 1.0 / T(10.0) * (x - 1) * (2 * x - 1) * (3 * x - 2) * (3 * x - 1) *
             (6 * x - 5) * (6 * x - 1);
    case 1:
      return -18.0 / T(5.0) * x * (x - 1) * (2 * x - 1) * (3 * x - 2) *
             (3 * x - 1) * (6 * x - 5);
    case 2:
      return 9.0 / T(2.0) * x * (x - 1) * (2 * x - 1) * (3 * x - 2) *
             (6 * x - 5) * (6 * x - 1);
    case 3:
      return -4 * x * (x - 1) * (3 * x - 2) * (3 * x - 1) * (6 * x - 5) *
             (6 * x - 1);
    case 4:
      return 9.0 / T(2.0) * x * (x - 1) * (2 * x - 1) * (3 * x - 1) *
             (6 * x - 5) * (6 * x - 1);
    case 5:
      return -18.0 / T(5.0) * x * (x - 1) * (2 * x - 1) * (3 * x - 2) *
             (3 * x - 1) * (6 * x - 1);
    case 6:
      return 1.0 / T(10.0) * x * (2 * x - 1) * (3 * x - 2) * (3 * x - 1) *
             (6 * x - 5) * (6 * x - 1);
    default:
      THROW("Not implemented!")
    }
  }

  static T P6Dfunctions(T x, int i) {
    T x2 = x * x;
    T x3 = x2 * x;
    T x4 = x2 * x2;
    T x5 = x2 * x3;
    switch (i) {
    case 0:
      return 1.0 / T(10.0) * (12 * x - 7) *
             (324 * x4 - 756 * x3 + 609 * x2 - 196 * x + 21);
    case 1:
      return -36.0 / T(5.0) *
             (324 * x5 - 900 * x4 + 930 * x3 - 435 * x2 + 87 * x - 5);
    case 2:
      return 9.0 / T(2.0) *
             (1296 * x5 - 3420 * x4 + 3288 * x3 - 1383 * x2 + 234 * x - 10);
    case 3:
      return -8 * (2 * x - 1) * (18 * x2 - 18 * x + 1) * (27 * x2 - 27 * x + 5);
    case 4:
      return 9.0 / T(2.0) *
             (1296 * x5 - 3060 * x4 + 2568 * x3 - 921 * x2 + 132 * x - 5);
    case 5:
      return -36.0 / T(5.0) *
             (324 * x5 - 720 * x4 + 570 * x3 - 195 * x2 + 27 * x - 1);
    case 6:
      return 1.0 / T(10.0) * (12 * x - 5) *
             (324 * x4 - 540 * x3 + 285 * x2 - 50 * x + 2);
    default:
      THROW("Not implemented!")
    }
  }

  static T P7functions(T x, int i) {
    switch (i) {
    case 0:
      return -1.0 / T(720.0) * (x - 1) * (7 * x - 6) * (7 * x - 5) *
             (7 * x - 4) * (7 * x - 3) * (7 * x - 2) * (7 * x - 1);
    case 1:
      return 49.0 / T(720.0) * x * (x - 1) * (7 * x - 6) * (7 * x - 5) *
             (7 * x - 4) * (7 * x - 3) * (7 * x - 2);
    case 2:
      return -49.0 / T(240.0) * x * (x - 1) * (7 * x - 6) * (7 * x - 5) *
             (7 * x - 4) * (7 * x - 3) * (7 * x - 1);
    case 3:
      return 49.0 / T(144.0) * x * (x - 1) * (7 * x - 6) * (7 * x - 5) *
             (7 * x - 4) * (7 * x - 2) * (7 * x - 1);
    case 4:
      return -49.0 / T(144.0) * x * (x - 1) * (7 * x - 6) * (7 * x - 5) *
             (7 * x - 3) * (7 * x - 2) * (7 * x - 1);
    case 5:
      return 49.0 / T(240.0) * x * (x - 1) * (7 * x - 6) * (7 * x - 4) *
             (7 * x - 3) * (7 * x - 2) * (7 * x - 1);
    case 6:
      return -49.0 / T(720.0) * x * (x - 1) * (7 * x - 5) * (7 * x - 4) *
             (7 * x - 3) * (7 * x - 2) * (7 * x - 1);
    case 7:
      return 1.0 / T(720.0) * x * (7 * x - 6) * (7 * x - 5) * (7 * x - 4) *
             (7 * x - 3) * (7 * x - 2) * (7 * x - 1);
    default:
      THROW("Not implemented!")
    }
  }

  static T P7Dfunctions(T x, int i) {
    T x2 = x * x;
    T x3 = x2 * x;
    T x4 = x2 * x2;
    T x5 = x2 * x3;
    T x6 = x3 * x3;
    switch (i) {
    case 0:
      return -1.0 / T(720.0) *
             (823543 * x6 - 2823576 * x5 + 3865610 * x4 - 2689120 * x3 +
              995043 * x2 - 183848 * x + 13068);
    case 1:
      return 49.0 / T(720.0) *
             (117649 * x6 - 388962 * x5 + 505925 * x4 - 326340 * x3 +
              107184 * x2 - 16056 * x + 720);
    case 2:
      return -49.0 / T(240.0) *
             (117649 * x6 - 374556 * x5 + 463050 * x4 - 278320 * x3 +
              82509 * x2 - 10548 * x + 360);
    case 3:
      return 49.0 / T(144.0) *
             (117649 * x6 - 360150 * x5 + 423605 * x4 - 238924 * x3 +
              65352 * x2 - 7592 * x + 240);
    case 4:
      return -49.0 / T(144.0) *
             (117649 * x6 - 345744 * x5 + 387590 * x4 - 206976 * x3 +
              53445 * x2 - 5904 * x + 180);
    case 5:
      return 49.0 / T(240.0) *
             (117649 * x6 - 331338 * x5 + 355005 * x4 - 181300 * x3 +
              45024 * x2 - 4824 * x + 144);
    case 6:
      return -49.0 / T(720.0) *
             (117649 * x6 - 316932 * x5 + 325850 * x4 - 160720 * x3 +
              38829 * x2 - 4076 * x + 120);
    case 7:
      return 1.0 / T(720.0) *
             (823543 * x6 - 2117682 * x5 + 2100875 * x4 - 1008420 * x3 +
              238728 * x2 - 24696 * x + 720);
    default:
      THROW("Not implemented!")
    }
  }

  static T P8functions(T x, int i) {
    switch (i) {
    case 0:
      return 1.0 / T(315.0) * (x - 1) * (2 * x - 1) * (4 * x - 3) *
             (4 * x - 1) * (8 * x - 7) * (8 * x - 5) * (8 * x - 3) *
             (8 * x - 1);
    case 1:
      return -64.0 / T(315.0) * x * (x - 1) * (2 * x - 1) * (4 * x - 3) *
             (4 * x - 1) * (8 * x - 7) * (8 * x - 5) * (8 * x - 3);
    case 2:
      return 16.0 / T(45.0) * x * (x - 1) * (2 * x - 1) * (4 * x - 3) *
             (8 * x - 7) * (8 * x - 5) * (8 * x - 3) * (8 * x - 1);
    case 3:
      return -64.0 / T(45.0) * x * (x - 1) * (2 * x - 1) * (4 * x - 3) *
             (4 * x - 1) * (8 * x - 7) * (8 * x - 5) * (8 * x - 1);
    case 4:
      return 4.0 / T(9.0) * x * (x - 1) * (4 * x - 3) * (4 * x - 1) *
             (8 * x - 7) * (8 * x - 5) * (8 * x - 3) * (8 * x - 1);
    case 5:
      return -64.0 / T(45.0) * x * (x - 1) * (2 * x - 1) * (4 * x - 3) *
             (4 * x - 1) * (8 * x - 7) * (8 * x - 3) * (8 * x - 1);
    case 6:
      return 16.0 / T(45.0) * x * (x - 1) * (2 * x - 1) * (4 * x - 1) *
             (8 * x - 7) * (8 * x - 5) * (8 * x - 3) * (8 * x - 1);
    case 7:
      return -64.0 / T(315.0) * x * (x - 1) * (2 * x - 1) * (4 * x - 3) *
             (4 * x - 1) * (8 * x - 5) * (8 * x - 3) * (8 * x - 1);
    case 8:
      return 1.0 / T(315.0) * x * (2 * x - 1) * (4 * x - 3) * (4 * x - 1) *
             (8 * x - 7) * (8 * x - 5) * (8 * x - 3) * (8 * x - 1);
    default:
      THROW("Not implemented!")
    }
  }

  static T P8Dfunctions(T x, int i) {
    T x2 = x * x;
    T x3 = x2 * x;
    T x4 = x2 * x2;
    T x5 = x2 * x3;
    T x6 = x3 * x3;
    T x7 = x4 * x3;
    switch (i) {
    case 0:
      return 1.0 / T(315.0) * (16 * x - 9) *
             (65536 * x6 - 221184 * x5 + 294912 * x4 - 196992 * x3 +
              68784 * x2 - 11772 * x + 761);
    case 1:
      return -64.0 / T(315.0) *
             (131072 * x7 - 501760 * x6 + 784896 * x5 - 644000 * x4 +
              294784 * x3 - 73290 * x2 + 8658 * x - 315);
    case 2:
      return 16.0 / T(45.0) *
             (262144 * x7 - 974848 * x6 + 1468416 * x5 - 1145600 * x4 +
              489248 * x3 - 110118 * x2 + 11178 * x - 315);
    case 3:
      return -64.0 / T(45.0) *
             (131072 * x7 - 473088 * x6 + 686592 * x5 - 511200 * x4 +
              205824 * x3 - 43038 * x2 + 4006 * x - 105);
    case 4:
      return 4.0 / T(9.0) * (2 * x - 1) *
             (262144 * x6 - 786432 * x5 + 890880 * x4 - 471040 * x3 +
              116256 * x2 - 11808 * x + 315);
    case 5:
      return -64.0 / T(45.0) *
             (131072 * x7 - 444416 * x6 + 600576 * x5 - 412960 * x4 +
              152704 * x3 - 29346 * x2 + 2538 * x - 63);
    case 6:
      return 16.0 / T(45.0) *
             (262144 * x7 - 860160 * x6 + 1124352 * x5 - 748800 * x4 +
              269088 * x3 - 50490 * x2 + 4286 * x - 105);
    case 7:
      return -64.0 / T(315.0) *
             (131072 * x7 - 415744 * x6 + 526848 * x5 - 341600 * x4 +
              120064 * x3 - 22134 * x2 + 1854 * x - 45);
    case 8:
      return 1.0 / T(315.0) * (16 * x - 7) *
             (65536 * x6 - 172032 * x5 + 172032 * x4 - 81536 * x3 + 18480 * x2 -
              1764 * x + 45);
    default:
      THROW("Not implemented!")
    }
  }
};

template <int degree, class T = double> class Polynomial {

public:
  constexpr double execute(T input, int i) const {
    static_assert(degree <= 8, "Degree out of range!");
    static_assert(degree >= 0, "Degree to low!");
    if constexpr (degree == 0) {
      return PolynomialEntry<T>::P0functions(input, i);
    } else if constexpr (degree == 1) {
      return PolynomialEntry<T>::P1functions(input, i);
    } else if constexpr (degree == 2) {
      return PolynomialEntry<T>::P2functions(input, i);
    } else if constexpr (degree == 3) {
      return PolynomialEntry<T>::P3functions(input, i);
    } else if constexpr (degree == 4) {
      return PolynomialEntry<T>::P4functions(input, i);
    } else if constexpr (degree == 5) {
      return PolynomialEntry<T>::P5functions(input, i);
    } else if constexpr (degree == 6) {
      return PolynomialEntry<T>::P6functions(input, i);
    } else if constexpr (degree == 6) {
      return PolynomialEntry<T>::P6functions(input, i);
    } else if constexpr (degree == 7) {
      return PolynomialEntry<T>::P7functions(input, i);
    } else if constexpr (degree == 8) {
      return PolynomialEntry<T>::P8functions(input, i);
    }
  }
};

template <typename T = double> class P0Triangle {
public:
  T P0functions(T x, int i) const {
    if (i == 0)
      return T(1.0);
    else
      THROW("Not implemented!")
  }

  T P0Dfunctions(T x, int i) const {
    if (i == 0)
      return T(0.0);
    else
      THROW("Not implemented!")
  }
};

template <typename T = double> class P1Triangle {
public:
  T P1functions(T x, int i) const {
    switch (i) {
    case 0:
      return T(1.0);
    case 1:
      return x;
    default:
      THROW("Not implemented!")
    }
  }

  T P1Dfunctions(T x, int i) const {
    switch (i) {
    case 0:
      return T(0.0);
    case 1:
      return T(1.0);
    default:
      THROW("Not implemented!")
    }
  }
};

template <typename T = double> class P2Triangle {
public:
  T P2functions(T x, int i) const {
    switch (i) {
    case 0:
      return T(1.0);
    case 1:
      return 2 * x;
    case 2:
      return x * (2 * x - 1);
    default:
      THROW("Not implemented!")
    }
  }

  T P2Dfunctions(T x, int i) const {
    switch (i) {
    case 0:
      return T(0.0);
    case 1:
      return T(2.0);
    case 2:
      return 4 * x - 1;
    default:
      THROW("Not implemented!")
    }
  }
};

template <typename T = double> class P3xTriangle {
public:
  T P3functions(T x, int i) const {
    switch (i) {
    case 0:
      return T(1.0);
    case 1:
      return 3 * x;
    case 2:
      return 3 / T(2.0) * x * (3 * x - 1);
    case 3:
      return 1 / T(2.0) * x * (3 * x - 1) * (3 * x - 2);
    default:
      THROW("Not implemented!")
    }
  }

  T P3Dfunctions(T x, int i) const {
    switch (i) {
    case 0:
      return T(0.0);
    case 1:
      return T(3.0);
    case 2:
      return 9 * x - 3 / T(2.0);
    case 3:
      return 27 / T(2.0) * x * x - 9 * x + 1;
    default:
      THROW("Not implemented!")
    }
  }
};

template <typename T = double> class P4xTriangle {
public:
  T P4functions(T x, int i) const {
    switch (i) {
    case 0:
      return T(1.0);
    case 1:
      return 4 * x;
    case 2:
      return 4 / T(2.0) * x * (4 * x - 1);
    case 3:
      return 4 / T(6.0) * x * (4 * x - 1) * (4 * x - 2);
    case 4:
      return 1 / T(6.0) * x * (4 * x - 1) * (4 * x - 2) * (4 * x - 3);
    default:
      THROW("Not implemented!")
    }
  }

  T P4Dfunctions(T x, int i) const {
    T x2 = x * x;
    T x3 = x2 * x;
    switch (i) {
    case 0:
      return T(0.0);
    case 1:
      return T(4.0);
    case 2:
      return 16 * x - 2;
    case 3:
      return 32 * x * x - 16 * x + 4 / T(3.0);
    case 4:
      return 128 / T(3.0) * x3 - 48 * x2 + 44 / T(3.0) * x - 1;
    default:
      THROW("Not implemented!")
    }
  }
};

template <typename T = double> class P5xTriangle {
public:
  T P5functions(T x, int i) const {
    switch (i) {
    case 0:
      return T(1.0);
    case 1:
      return 5 * x;
    case 2:
      return 5 / T(2.0) * x * (5 * x - 1);
    case 3:
      return 5 / T(6.0) * x * (5 * x - 1) * (5 * x - 2);
    case 4:
      return 5 / T(24.0) * x * (5 * x - 1) * (5 * x - 2) * (5 * x - 3);
    case 5:
      return 1 / T(24.0) * x * (5 * x - 1) * (5 * x - 2) * (5 * x - 3) *
             (5 * x - 4);
    default:
      THROW("Not implemented!")
    }
  }

  T P5Dfunctions(T x, int i) const {
    T x2 = x * x;
    T x3 = x2 * x;
    T x4 = x2 * x2;
    switch (i) {
    case 0:
      return T(0.0);
    case 1:
      return T(5.0);
    case 2:
      return 25 * x - 5 / T(2.0);
    case 3:
      return 125 / T(2.0) * x2 - 25 * x + 5 / T(3.0);
    case 4:
      return (1250 * x3 - 1125 * x2 + 275 * x - 15) / T(12.0);
    case 5:
      return 3125.0 / T(24.0) * x4 - 625.0 / T(3.0) * x3 + 875.0 / T(8.0) * x2 -
             125.0 / T(6.0) * x + 1.0;
    default:
      THROW("Not implemented!")
    }
  }
};

template <typename T = double> class P6xTriangle {
public:
  T P6functions(T x, int i) const {
    switch (i) {
    case 0:
      return T(1.0);
    case 1:
      return 6 * x;
    case 2:
      return 3 * x * (6 * x - 1);
    case 3:
      return x * (6 * x - 1) * (6 * x - 2);
    case 4:
      return 1 / T(4.0) * x * (6 * x - 1) * (6 * x - 2) * (6 * x - 3);
    case 5:
      return 1 / T(20.0) * x * (6 * x - 1) * (6 * x - 2) * (6 * x - 3) *
             (6 * x - 4);
    case 6:
      return 1 / T(120.0) * x * (6 * x - 1) * (6 * x - 2) * (6 * x - 3) *
             (6 * x - 4) * (6 * x - 5);
    default:
      THROW("Not implemented!")
    }
  }

  T P6Dfunctions(T x, int i) const {
    T x2 = x * x;
    T x3 = x2 * x;
    T x4 = x2 * x2;
    T x5 = x2 * x3;
    switch (i) {
    case 0:
      return T(0.0);
    case 1:
      return T(6.0);
    case 2:
      return 36 * x - 3;
    case 3:
      return 108 * x * x - 36 * x + 2;
    case 4:
      return 1 / T(2.0) * (432 * x3 - 324 * x * x + 66 * x - 3);
    case 5:
      return 1 / T(5.0) * (1620 * x4 - 2160 * x3 + 945 * x * x - 150 * x + 6);
    case 6:
      return 1 / T(10.0) *
             (3888 * x5 - 8100 * x4 + 6120 * x3 - 2025 * x * x + 274 * x - 10);
    default:
      THROW("Not implemented!")
    }
  }
};

template <typename T = double> class P7xTriangle {
public:
  T P7functions(T x, int i) const {
    T x2 = x * x;
    T x3 = x2 * x;
    T x4 = x2 * x2;
    T x5 = x2 * x3;
    T x6 = x3 * x3;
    switch (i) {
    case 0:
      return T(0.0);
    case 1:
      return T(7.0);
    case 2:
      return 1 / T(2.0) * (98 * x - 7);
    case 3:
      return 1 / T(6.0) * (1029 * x * x - 294 * x + 14);
    case 4:
      return 1 / T(12.0) * (4802 * x3 - 3087 * x * x + 539 * x - 21);
    case 5:
      return 1 / T(120.0) *
             (84035 * x4 - 96040 * x3 + 36015 * x * x - 4900 * x + 168);
    case 6:
      return 1 / T(720.0) *
             (705894 * x5 - 1260525 * x4 + 816340 * x3 - 231525 * x * x +
              26852 * x - 840);
    case 7:
      return 1 / T(720.0) *
             (823543 * x6 - 2117682 * x5 + 2100875 * x4 - 1008420 * x3 +
              238728 * x * x - 24696 * x + 720);
    default:
      THROW("Not implemented!")
    }
  }

  T P7Dfunctions(T x, int i) const { THROW("Not implemented!") }
};

template <typename T = double> class P8xTriangle {
public:
  T P8functions(T x, int i) const {
    T x2 = x * x;
    T x3 = x2 * x;
    T x4 = x2 * x2;
    T x5 = x2 * x3;
    T x6 = x3 * x3;
    T x7 = x3 * x4;
    switch (i) {
    case 0:
      return T(0.0);
    case 1:
      return T(8.0);
    case 2:
      return 64 * x - 4;
    case 3:
      return 1 / T(3.0) * (768 * x * x - 192 * x + 8);
    case 4:
      return 1 / T(3.0) * (2048 * x3 - 1152 * x * x + 176 * x - 6);
    case 5:
      return 1 / T(15.0) *
             (20480 * x4 - 20480 * x3 + 6720 * x * x - 800 * x + 24);
    case 6:
      return 1 / T(45.0) *
             (98304 * x5 - 153600 * x4 + 87040 * x3 - 21600 * x * x + 2192 * x -
              60);
    case 7:
      return 1 / T(315.0) *
             (917504 * x6 - 2064384 * x5 + 1792000 * x4 - 752640 * x3 +
              155904 * x * x - 14112 * x + 360);
    case 8:
      return 1 / T(315.0) *
             (1048576 * x7 - 3211264 * x6 + 3956736 * x5 - 2508800 * x4 +
              866432 * x3 - 157584 * x * x + 13068 * x - 315);
    }
    THROW("Not implemented!")
  }

  T P8Dfunctions(T x, int i) const { THROW("Not implemented!") }
};
