#ifndef STKUTILS_HPP_
#define STKUTILS_HPP_

#include <csignal>


#ifdef  __MAIN__
#define GLOBAL
#else
#define GLOBAL  extern
#endif


GLOBAL bool gShutOff;
GLOBAL bool gIsVerbose;

void ctrlC( int aStatus );

inline void setSigInt(  )
{
	gShutOff = false;
	signal(SIGINT, ctrlC);
}

#undef  GLOBAL

#ifdef  __MAIN__

void ctrlC( int aStatus )
{
	// exit(aStatus); // デストラクタが呼ばれないのでダメ
	gShutOff = true;
	signal( SIGINT, NULL );
}

#endif


#endif //STKUTILS_HPP_
