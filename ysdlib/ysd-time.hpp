#include <sys/time.h>

namespace ysd{	// ---> ysd


double gettimeofday_sec(){
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1e-6;
}



}			// <--- ysd
