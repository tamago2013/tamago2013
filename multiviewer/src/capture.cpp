/*
 * capture.cpp
 *
 *  Created on: 2011/12/02
 *      Author: shikina
 */


#include <iostream>
#include <string>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>

#include "GLcommon.hpp"

//cv::VideoWriter gVideoWriter;
//cv::Mat gCapFrame, gVideoFrame;
bool gIsVideoOpen = false;

GLsizei gCapSize[2];

using namespace std;

bool captureInit( string fileName, int fps )
{
/*	gCapSize[0] = gWindowSize[0];
	gCapSize[1] = gWindowSize[1];
	gVideoWriter.open( "out.avi", CV_FOURCC('M','J','P','G'),fps, cv::Size(gCapSize[0],gCapSize[1]));
	gCapFrame.create( gCapSize[0], gCapSize[1], CV_8UC3 );
	gVideoFrame.create( gCapSize[0], gCapSize[1], CV_8UC3 );
	if( !gVideoWriter.isOpened(  ) )
	{
		cerr << "cannot open video" << endl;
		return false;
	}

	if( !(gCapFrame.data && gVideoFrame.data ))
	{
		cerr << "cannot create screen buffer" << endl;
		return false;
	}

	cerr << "capture start : " << fileName << endl;
	gIsVideoOpen = true;
*/
	return true;
}

bool captureWriteFrame()
{/*
	if( !gIsVideoOpen )
		return false;
	glReadBuffer(GL_FRONT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glReadPixels(0, 0, gCapSize[0], gCapSize[1], GL_BGR, GL_UNSIGNED_BYTE, gCapFrame.data );
	cv::flip(gCapFrame, gVideoFrame, 0);//垂直反転
	gVideoWriter.write( gVideoFrame );*/
	return true;
}
