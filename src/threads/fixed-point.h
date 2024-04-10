#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H
#include <stdint.h>
#define F 16384

typedef int32_t fixed_point;

// n represents an int number, x and y represents fixed point numbers
fixed_point int_to_fxp(int n);
int fxp_to_int_round_toward_zero(fixed_point x);
int fxp_to_int_round_nearest(fixed_point x);
int fxp_plus_int(fixed_point x,int n);
int fxp_minus_int(fixed_point x , int n);
int mul_fxp(fixed_point x , fixed_point y);
int div_fxp(fixed_point x , fixed_point y);

#endif /* threads/fixed-point.h */
