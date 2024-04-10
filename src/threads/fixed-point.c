#include "threads/fixed-point.h"
#include <stdint.h>

#define F 16384

typedef int32_t fixed_point;


fixed_point int_to_fxp(int n)
{
	return n*F;
}

int fxp_to_int_round_toward_zero(fixed_point x){
	return x/F;
}

int fxp_to_int_round_nearest(fixed_point x){
	if( x>=0 ){
	return (x+F/2)/F;
	}
	else{
	return (x-F/2)/F;
}
}

//add x and y return x+y

//subtract y from x return x-y

int fxp_plus_int(fixed_point x,int n){
	return x+n*F;
}

int fxp_minus_int(fixed_point x , int n){
	return x-n*F;
}

int mul_fxp(fixed_point x , fixed_point y){
	return ((int64_t) x) * y / F;
}

//multiply x by n return x*n

int div_fxp(fixed_point x , fixed_point y){
	return ((int64_t) x) * F / y;
}

//divide x by n return x/n
