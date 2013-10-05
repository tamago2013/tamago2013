#include <sys/time.h>
#include<ssm.hpp>

namespace ysd{	// ---> ysd
namespace time {    // ---> time

double gettimeofday_sec(){
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1e-6;
}

double time_end;
double time_start;
double interval_timerSSM(double interval)
{
    if(time_start != 0.0)
    {
        time_end = ysd::time::gettimeofday_sec();
        //        fprintf(stderr, "time_end: %lf\n", time_end);
        //        fprintf(stderr, "time_start: %lf\n", time_start);
        if( interval > (time_end - time_start) )
        {
            usleepSSM( (int)(1000000.0*(interval - time_end + time_start)) );
            time_start = ysd::time::gettimeofday_sec();
        }
        else
        {
            fprintf(stderr, "もう限界いい！ interval: %lf\n", interval);
//                        cerr << "もう限界いい！" << endl;
            time_start = ysd::time::gettimeofday_sec();
        }
    }
    else
    {
        time_start = ysd::time::gettimeofday_sec();
        fprintf(stderr, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }

}



}   // ---> time
}			// <--- ysd
