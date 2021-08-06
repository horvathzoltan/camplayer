#include "geomath.h"


void GeoMath::uIranyszogXY(double p0x, double p0y, double p1x, double p1y, double *a, double *t){
    double A, B;

    A=p1y-p0y; // ez most irányvektor!!!
    B=p1x-p0x;
    *a=uSzog(A, B)-M_PI_2;
    *t=hypot(A, B);
}

#define M_2PI (M_PI+M_PI)
#define M_15PI (M_PI+M_PI_2)

double GeoMath::uSzog(double A1, double B1){
    if(A1==0 && B1>0) return 0;
    if(A1>0 && B1==0) return M_PI_2;
    if(A1==0 && B1<0) return M_PI;
    if(A1<0 && B1==0) return M_15PI;
    if(A1>0 && B1>0) return atan(A1/B1);
    if(B1<0 && A1!=0) return M_PI+atan(A1/B1);
    return M_2PI+atan(A1/B1);
}

double GeoMath::uSzogNormalize(double d)
{
    auto a = fmod(d, M_2PI);
    if(a<-M_PI) return M_2PI-a;
    if(a>M_PI) return a-M_2PI;
    return a;
}
