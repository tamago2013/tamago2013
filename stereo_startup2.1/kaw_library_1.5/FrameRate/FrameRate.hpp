#ifndef TIMECONTROL_HPP
#define TIMECONTROL_HPP

inline double timetof( struct timespec t );
double getCurrentTime( void );
//void printFrameRate();
void getFrameRate(double *result);

#endif // TIMECONTROL_HPP
