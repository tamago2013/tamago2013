#ifndef STKMATH_HPP_
#define STKMATH_HPP_

#include <cmath>

inline double deg2rad(double th){return th / 180.0 * M_PI;}
inline double rad2deg(double th){return th * 180.0 / M_PI;}
inline double circularTheta( double th )
{
	while( th > M_PI )
		th -= 2 * M_PI;
	while( th < -M_PI )
		th += 2 * M_PI;
	return th;
}


#endif //STKMATH_HPP_
