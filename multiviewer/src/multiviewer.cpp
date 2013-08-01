//============================================================================
// Name        : multiviewer.cpp
// Author      : shikina
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <csignal>
#include <stdexcept>
#include <cstdlib>


#include <ssm.hpp>

#include <GL/glut.h>
#include "GLcommon.hpp"
#include "GLdraw.hpp"

using namespace std;


bool gShutOff = false;
bool gIsVerbose = false;

void ctrlC( int aStatus )
{
	// exit(aStatus); // デストラクタが呼ばれないのでダメ
	gShutOff = true;
	myExit();
	signal( SIGINT, NULL );
}

void setSigInt(  )
{
	signal(SIGINT, ctrlC);
}

class MyParam
{
	void init()
	{
	}
public:

	MyParam(){init();}
	bool optAnalyze(int aArgc, char **aArgv)
	{
		initGL(&aArgc, aArgv);
		//オプション解析
		int opt;
		while((opt = getopt(aArgc, aArgv, "d:D:m:M:r:R:gh")) != -1)
		{
			switch(opt)
			{
			case 'm' : gDrawHightMin = atof(optarg); break;
			case 'M' : gDrawHightMax = atof(optarg); break;
			case 'd' : gDrawDistMin = atoi(optarg); break;
			case 'D' : gDrawDistMax = atoi(optarg); break;
			case 'r' : gDrawReflectMin = atof(optarg); break;
			case 'R' : gDrawReflectMax = atof(optarg); break;
			case 'h' :
			{
				cerr
					<< "HELP" << endl
					<< "\t-h         : print this help" << endl
					<< "\t-g         : NOT use gyrodometry" << endl
					<< "\t-m HIGHT   : [m] set draw min hight to HIGHT" << endl
					<< "\t-M HIGHT   : [m] set draw max hight to HIGHT" << endl
					<< "\t-d DIST    : [mm] set draw min distance of urgFS to DIST" << endl
					<< "\t-D DIST    : [mm] set draw max distance of urgFS to DIST" << endl
					<< "\t-r REFLECT : set draw max reflection intensity of urg to REFLECT" << endl
					<< "\t-R REFLECT : set draw max reflection intensity of urg to REFLECT" << endl
					<< "ex)" << endl << "\t$ "<< aArgv[0] << "-m 1.5 -M 1.8" << endl
					//<< "\t" << endl
					<< endl;
				return false;
			}break;
			}
		}
		return true;
	}
};


int main( int aArgc, char **aArgv )
{
	MyParam param;
	if(!param.optAnalyze( aArgc, aArgv))
		return -1;
	setSigInt(  );
	try
	{
		if( !initSSM() ){ throw std::runtime_error("main : ssm init");}
		myInit();
		setEventCallback();

		// 処理の開始
		cerr << "start" << endl;

		// openGLのメインループ
		glutMainLoop();

	}
	catch(std::exception &exception)
	{
		cerr << endl << "EXCEPTION : " << exception.what() << endl;
	}
	catch(...)
	{
		cerr << endl << "EXCEPTION : unknown exception." << endl;
	}
	unsetEventCallback();

	cerr << endl;
	cerr <<"end" << endl;

	return 0;
}
