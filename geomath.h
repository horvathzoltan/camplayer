#ifndef GEOMATH_H
#define GEOMATH_H

#include <QtMath>

class GeoMath
{
public:
    static void uIranyszogXY(double p0x, double p0y, double p1x, double p1y, double *a, double *t);
    static double uSzog(double A1, double B1);
    static double uSzogNormalize(double d);
};

#endif // GEOMATH_H
