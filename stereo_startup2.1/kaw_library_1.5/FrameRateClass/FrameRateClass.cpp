//============================================================================
// Name        : timeControl.cpp
// Author      : Shun Kawamoto (KAW)
// Version     : 0.10
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include <iostream>
#include <sys/time.h>

#include "FrameRateClass.hpp"

using namespace std;

//doubleへの変換
inline double timetof( struct timespec t )
{
    return ( double )t.tv_sec + ( double )t.tv_nsec / 1000000000.0;
}

//時間取得
 double getCurrentTime( void )
{
    struct timespec current;

	clock_gettime( CLOCK_REALTIME, &current );
	return timetof( current );
}

 FrameRate::FrameRate(void)
 {
     isFirst = true;
     previous = 0.0;
     present = 0.0;
     result = 0.0;
 }

 double FrameRate::getFrameRate(void)
 {
     present = getCurrentTime();

     if(isFirst){
         isFirst = false;
         result = -1.0;
     }else{
         result =  1.0 / (present - previous);
     }

     previous = present;

     return result;
 }
/*
 void printFrameRate(){

 }
*/
