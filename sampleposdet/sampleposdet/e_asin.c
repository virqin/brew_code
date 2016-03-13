
/* @(#)e_asin.c 1.3 95/01/18 */
/*
* ====================================================
* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
*
* Developed at SunSoft, a Sun Microsystems, Inc. business.
* Permission to use, copy, modify, and distribute this
* software is freely granted, provided that this notice 
* is preserved.
* ====================================================
*/

// #ifndef lint
// static char rcsid[] = "$FreeBSD: src/lib/msun/src/e_asin.c,v 1.11 2005/02/04 18:26:05 das Exp $";
// #endif

/* __ieee754_asin(x)
* Method :                  
*	Since  asin(x) = x + x^3/6 + x^5*3/40 + x^7*15/336 + ...
*	we approximate asin(x) on [0,0.5] by
*		asin(x) = x + x*x^2*R(x^2)
*	where
*		R(x^2) is a rational approximation of (asin(x)-x)/x^3 
*	and its remez error is bounded by
*		|(asin(x)-x)/x^3 - R(x^2)| < 2^(-58.75)
*
*	For x in [0.5,1]
*		asin(x) = pi/2-2*asin(sqrt((1-x)/2))
*	Let y = (1-x), z = y/2, s := sqrt(z), and pio2_hi+pio2_lo=pi/2;
*	then for x>0.98
*		asin(x) = pi/2 - 2*(s+s*z*R(z))
*			= pio2_hi - (2*(s+s*z*R(z)) - pio2_lo)
*	For x<=0.98, let pio4_hi = pio2_hi/2, then
*		f = hi part of s;
*		c = sqrt(z) - f = (z-f*f)/(s+f) 	...f+c=sqrt(z)
*	and
*		asin(x) = pi/2 - 2*(s+s*z*R(z))
*			= pio4_hi+(pio4-2s)-(2s*z*R(z)-pio2_lo)
*			= pio4_hi+(pio4-2f)-(2s*z*R(z)-(pio2_lo+2c))
*
* Special cases:
*	if x is NaN, return x itself;
*	if |x|>1, return NaN with invalid signal.
*
*/


//#include "math.h"
//#include "math_private.h"
#include <AEEStdLib.h>


//#if __BYTE_ORDER == __LITTLE_ENDIAN  && !(defined(__arm__) && !defined(__VFP_FP__))

typedef union
{
	double value;
	struct
	{
		uint32 lsw;
		uint32 msw;
	} parts;
} ieee_double_shape_type;

//#endif

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)					\
	do {								\
	ieee_double_shape_type gh_u;					\
	gh_u.value = (d);						\
	(i) = gh_u.parts.msw;						\
	} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)					\
	do {								\
	ieee_double_shape_type gl_u;					\
	gl_u.value = (d);						\
	(i) = gl_u.parts.lsw;						\
	} while (0)

/* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)					\
	do {								\
	ieee_double_shape_type sl_u;					\
	sl_u.value = (d);						\
	sl_u.parts.lsw = (v);						\
	(d) = sl_u.value;						\
	} while (0)


#define fabs FABS
#define sqrt FSQRT

// double e_asin(double x)
// {
// 	double one =  1.00000000000000000000e+00; /* 0x3FF00000, 0x00000000 */
// 	double huge_d =  1.000e+300;
// 	double pio2_hi =  1.57079632679489655800e+00; /* 0x3FF921FB, 0x54442D18 */
// 	double pio2_lo =  6.12323399573676603587e-17; /* 0x3C91A626, 0x33145C07 */
// 	double pio4_hi =  7.85398163397448278999e-01; /* 0x3FE921FB, 0x54442D18 */
// 											/* coefficient for R(x^2) */
// 	double pS0 =  1.66666666666666657415e-01; /* 0x3FC55555, 0x55555555 */
// 	double pS1 = -3.25565818622400915405e-01; /* 0xBFD4D612, 0x03EB6F7D */
// 	double pS2 =  2.01212532134862925881e-01; /* 0x3FC9C155, 0x0E884455 */
// 	double pS3 = -4.00555345006794114027e-02; /* 0xBFA48228, 0xB5688F3B */
// 	double pS4 =  7.91534994289814532176e-04; /* 0x3F49EFE0, 0x7501B288 */
// 	double pS5 =  3.47933107596021167570e-05; /* 0x3F023DE1, 0x0DFDF709 */
// 	double qS1 = -2.40339491173441421878e+00; /* 0xC0033A27, 0x1C8A2D4B */
// 	double qS2 =  2.02094576023350569471e+00; /* 0x40002AE5, 0x9C598AC8 */
// 	double qS3 = -6.88283971605453293030e-01; /* 0xBFE6066C, 0x1B8D0159 */
// 	double qS4 =  7.70381505559019352791e-02; /* 0x3FB3B8C5, 0xB12E9282 */
// 
// 	double t=0.0,w,p,q,c,r,s;
// 	int32 hx,ix;
// 	GET_HIGH_WORD(hx,x);
// 	ix = hx&0x7fffffff;
// 	if(ix>= 0x3ff00000) {		/* |x|>= 1 */
// 		uint32 lx;
// 		GET_LOW_WORD(lx,x);
// 		if(((ix-0x3ff00000)|lx)==0)
// 			/* asin(1)=+-pi/2 with inexact */
// 			return x*pio2_hi+x*pio2_lo;	
// 		return FDIV((x-x), (x-x));		/* asin(|x|>1) is NaN */   
// 	} else if (ix<0x3fe00000) {	/* |x|<0.5 */
// 		if(ix<0x3e400000) {		/* if |x| < 2**-27 */
// 			if(huge_d+x>one) return x;/* return x with inexact if x!=0*/
// 		} else 
// 			t = x*x;
// 		p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
// 		q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
// 		w = FDIV(p,q);
// 		return x+x*w;
// 	}
// 	/* 1> |x|>= 0.5 */
// 	w = one-fabs(x);
// 	t = w*0.5;
// 	p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
// 	q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
// 	s = sqrt(t);
// 	if(ix>=0x3FEF3333) { 	/* if |x| > 0.975 */
// 		w = FDIV(p,q);
// 		t = pio2_hi-(2.0*(s+s*w)-pio2_lo);
// 	} else {
// 		w  = s;
// 		SET_LOW_WORD(w,0);
// 		c  = FDIV((t-w*w), (s+w));
// 		r  = FDIV(p,q);
// 		p  = 2.0*s*r-(pio2_lo-2.0*c);
// 		q  = pio4_hi-2.0*w;
// 		t  = pio4_hi-(p-q);
// 	}
// 
// 	if(hx>0) 
// 		return t; 
// 	else 
// 		return -t;    
// }

double e_asinex(double x, double *dst)
{
	double one =  1.00000000000000000000e+00; /* 0x3FF00000, 0x00000000 */
	double huge_d =  1.000e+300;
	double pio2_hi =  1.57079632679489655800e+00; /* 0x3FF921FB, 0x54442D18 */
	double pio2_lo =  6.12323399573676603587e-17; /* 0x3C91A626, 0x33145C07 */
	double pio4_hi =  7.85398163397448278999e-01; /* 0x3FE921FB, 0x54442D18 */
												  /* coefficient for R(x^2) */
	double pS0 =  1.66666666666666657415e-01; /* 0x3FC55555, 0x55555555 */
	double pS1 = -3.25565818622400915405e-01; /* 0xBFD4D612, 0x03EB6F7D */
	double pS2 =  2.01212532134862925881e-01; /* 0x3FC9C155, 0x0E884455 */
	double pS3 = -4.00555345006794114027e-02; /* 0xBFA48228, 0xB5688F3B */
	double pS4 =  7.91534994289814532176e-04; /* 0x3F49EFE0, 0x7501B288 */
	double pS5 =  3.47933107596021167570e-05; /* 0x3F023DE1, 0x0DFDF709 */
	double qS1 = -2.40339491173441421878e+00; /* 0xC0033A27, 0x1C8A2D4B */
	double qS2 =  2.02094576023350569471e+00; /* 0x40002AE5, 0x9C598AC8 */
	double qS3 = -6.88283971605453293030e-01; /* 0xBFE6066C, 0x1B8D0159 */
	double qS4 =  7.70381505559019352791e-02; /* 0x3FB3B8C5, 0xB12E9282 */

	double t=0.0,w,p,q,c,r,s;
	int32 hx,ix;
	GET_HIGH_WORD(hx,x);
	ix = hx&0x7fffffff;
	if(ix>= 0x3ff00000) {		/* |x|>= 1 */
		uint32 lx;
		GET_LOW_WORD(lx,x);
		if(((ix-0x3ff00000)|lx)==0)
		{
			/* asin(1)=+-pi/2 with inexact */
			*dst = FADD( FMUL(x,pio2_hi), FMUL(x,pio2_lo) );
			return FADD( FMUL(x,pio2_hi), FMUL(x,pio2_lo) );
		}
		*dst = FDIV( FSUB(x,x),FSUB(x,x) );
		return FDIV( FSUB(x,x),FSUB(x,x) );		/* asin(|x|>1) is NaN */   
	} else if (ix < 0x3fe00000) {	/* |x|<0.5 */
		if(ix<0x3e400000) {		/* if |x| < 2**-27 */
			if(FCMP_G( FADD(huge_d, x), one )) 
			{
				*dst = x;
				return x;/* return x with inexact if x!=0*/
			}
		} else 
			t = FMUL(x,x);

		//p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
		p = FMUL(t, FADD( pS0, FMUL( t, FADD( pS1, FMUL( t, FADD( pS2, FMUL( t, FADD( pS3, FMUL( t, FADD( pS4, FMUL(t, pS5)))))))))));
		//q = one + t*(qS1+t*(qS2+t*(qS3+t*qS4)));
		q = FADD( one, FMUL( t, FADD( qS1, FMUL( t, FADD( qS2, FMUL( t, FADD(qS3, FMUL(t, qS4))))))));
		w = FDIV(p,q);

		*dst = FADD(x, FMUL(x, w));
		return FADD(x, FMUL(x, w));;
	}

	/* 1> |x|>= 0.5 */
	w = FSUB(one, fabs(x));
	t = FMUL(w, 0.5);
	//p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
	p = FMUL(t, FADD( pS0, FMUL( t, FADD( pS1, FMUL( t, FADD( pS2, FMUL( t, FADD( pS3, FMUL( t, FADD( pS4, FMUL(t, pS5)))))))))));
	//q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
	q = FADD( one, FMUL( t, FADD( qS1, FMUL( t, FADD( qS2, FMUL( t, FADD(qS3, FMUL(t, qS4))))))));
	s = sqrt(t);
	if(ix>=0x3FEF3333) { 	/* if |x| > 0.975 */
		w = FDIV(p,q);
		//t = pio2_hi-(2.0*(s+s*w)-pio2_lo);
		t = FSUB(pio2_hi, FSUB(FMUL(2.0, FADD(s, FMUL(s, w))), pio2_lo));
	} else {
		w  = s;
		SET_LOW_WORD(w,0);
		c  = FDIV((FSUB(t, FMUL(w,w))), FADD(s,w));
		r  = FDIV(p, q);
		p  = FSUB( FMUL(FMUL(2.0,s) ,r), FSUB(pio2_lo, FMUL(2.0, c)) );
		q  = FSUB(pio4_hi, FMUL(2.0, w));
		t  = FSUB(pio4_hi, FSUB(p,q));
	}

	if(hx>0) 
	{
		*dst = t;
		return t; 
	}
	else 
		return -t;    
}