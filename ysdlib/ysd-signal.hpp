#include <signal.h>
//#include <stdlib.h>

namespace ysd{

bool gShutOff = false;

void ctrlC( int aStatus )
{
    signal( SIGINT, NULL );
    gShutOff = true;
}

// Ctrl-C による正常終了を設定
inline void setSigInt()
{
    signal( SIGINT, ctrlC );
}





}
