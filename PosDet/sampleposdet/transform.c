#ifdef AEE_SIMULATOR
#include <math.h>
#endif

#include <AEESTDLIB.H>

#include "transform.h"

#define M_PI 3.14159265358979323846
#define M_SQRT2 1.41421356237309504880

#ifndef AEE_SIMULATOR
#define fabs FABS
#define sin FSIN
#define sqrt FSQRT
#define cos FCOS
//#define asin e_asin
#endif

extern double e_asinex(double x, double *dst);

// 1 - cos(x) == 2 sin^2(x/2)
double oneMinusCos(double x)
{
	double s = sin(FDIV(x,2));
	return FMUL(FMUL(s,s), 2);
}


double distance(double latA, double lngA, double latB, double lngB) {
	double earthR = 6371000;
	double tmp = 0;
	double test = 0;
	double dlat, dlng;
	double a, b, c, d, e, f;
// 
// #ifdef AEE_SIMULATOR
	
// 	latA *= M_PI/180;
// 	latB *= M_PI/180;
// 	lngA *= M_PI/180;
// 	lngB *= M_PI/180;
// 	tmp = sqrt(oneMinusCos(latA-latB) + cos(latA)*cos(latB)*(oneMinusCos(lngA - lngB)));
// 	return 2*earthR*asin(tmp/M_SQRT2);
// 
// #else

	latA = FMUL(latA, FDIV(M_PI,180));
	latB = FMUL(latB, FDIV(M_PI,180));
	lngA = FMUL(lngA, FDIV(M_PI,180));
	lngB = FMUL(lngB, FDIV(M_PI,180));
 	
	//tmp = sqrt(oneMinusCos(latA-latB) + cos(latA)*cos(latB)*(oneMinusCos(lngA - lngB)));

	tmp = sqrt(FADD(oneMinusCos(FSUB(latA, latB)), FMUL(FMUL(cos(latA), cos(latB)), oneMinusCos(FSUB(lngA, lngB)))));

// 	dlat = FSUB(latA, latB);
// 	dlng = FSUB(lngA, lngB);
// 	
// 	a = oneMinusCos(dlat);
// 	b = FMUL(cos(latA), cos(latB));
// 	c = FMUL(b, dlng);
// 	d = FADD(a, c);
//  	tmp = sqrt(d);

 	e_asinex(FDIV(tmp, M_SQRT2), &test);
	return FMUL(FMUL(2,earthR), test);
//#endif
}



