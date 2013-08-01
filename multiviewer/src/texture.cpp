
#include <iostream>
#include <GL/glut.h>
//#include <opencv2/opencv.hpp>

#include "texture.hpp"
#include "GLdraw.hpp"

#include "gnd-bmp.hpp"

gnd::bmp8_t observmap;

GLuint textures[TEXTURE_NUM];
//cv::Mat gFreeMap, gObservMap, gRoadMap, gRunnableMap;
double gFreeMapZoom = 1.0, gObservMapZoom = 1.0, gRoadMapZoom = 1.0,gRunnableMapZoom = 1.0;



using namespace std;

bool textureLoadFreeMap( )
{
/*	glBindTexture( GL_TEXTURE_2D, textures[TEXTURE_FREEMAP] );
	cerr << "map file open '" << freeMapGL.property.fname << "'." << endl;
	cv::Mat raw = cv::imread( freeMapGL.property.fname, 1), small;
	if( !raw.data )
	{
		cerr << "Warning: cannot open map file '" << freeMapGL.property.fname << "'." << endl;
		return false;
	}
	else
	{
		int z = (raw.cols > raw.rows ? raw.cols : raw.rows );
		int offset;

		offset = freeMapGL.property.offset;

		while( z >= 4096 )
		{
			gFreeMapZoom *= 0.5;
			z /= 2;
		}
		cerr << "\tresize map file" << endl;
		cv::resize( raw, small, cv::Size(), gFreeMapZoom, gFreeMapZoom,cv::INTER_NEAREST );
		// アルファチャンネルの追加
		cerr << "\tconvert map file for GL" << endl;
		cv::Mat alpha( small.rows,small.cols,CV_8UC1 );
		gFreeMap.create(small.rows,small.cols,CV_8UC4 );
		int fromTo[] = {0,0, 1,1, 2,2, 3,3 };
		cv::Mat in[] = {small, alpha};
		float a;
		for( int i=0; i < small.size().area(); i++ )
		{
			if( small.data[i*3] != offset )
			{
				small.data[i*3 + 1] = 255;
				small.data[i*3 + 2] = 255;
				a = small.data[i*3] - offset;
				if( a >= 0 )
				{
//					a =  a / (255.0f - offset);
//					small.data[i*3    ] = small.data[i*3 + 1] = static_cast<unsigned char >( a * 128 ) + 127;
//					small.data[i*3 + 2] = 255;
//					alpha.data[i] = static_cast<unsigned char >( a * 255 );
					alpha.data[i] = 0;
				}
				else
				{
					a = ( offset - a ) / offset;
					//small.data[i*3] = small.data[i*3 + 2] = 255 - static_cast<unsigned char >( a * 255 );
					small.data[i*3] = 0x99;
					small.data[i*3 + 1] = 0xff;
					small.data[i*3 + 2] = 0x33;
					alpha.data[i] = static_cast<unsigned char >( a * 255 );
				}
			}
			else
				alpha.data[i] = 0;

		}
		cv::mixChannels(in, 2, &gFreeMap, 1, fromTo, 4);


		cerr << "\t" << gFreeMap.rows << " " << gFreeMap.cols << endl
				<< "\t" << gFreeMap.depth() << ", " << gFreeMap.channels() << ", " << gFreeMap.elemSize() << endl;

		cerr << "\tset texture." << endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, gFreeMap.cols, gFreeMap.rows, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, gFreeMap.data );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gFreeMap.cols, gFreeMap.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, gFreeMap.data );
	}*/
	return true;
}

bool textureLoadObservMap(  )
{
//	glBindTexture( GL_TEXTURE_2D, textures[TEXTURE_OBSERVMAP] );
//	cerr << "map file open '" << observMapGL.property.fname << "'." << endl;
//	cv::Mat raw = cv::imread( observMapGL.property.fname, 0), small;
	int ret;
	ret = gnd::bmp::read(observMapGL.property.fname, &observmap);
	
	observmap.pset_origin(observMapGL.property.point[1].x, observMapGL.property.point[1].y);
	/*
	if( !raw.data )
	{
		cerr << "Warning: cannot open map file '" << observMapGL.property.fname << "'." << endl;
		return false;
	}
	else
	{
		int z = (raw.cols > raw.rows ? raw.cols : raw.rows );
		while( z >= 4096 )
		{
			gObservMapZoom *= 0.5;
			z /= 2;
		}
		cerr << "\tresize map file" << endl;
		cv::resize( raw, small, cv::Size(), gObservMapZoom, gObservMapZoom,cv::INTER_NEAREST );
		// アルファチャンネルの追加
		cerr << "\tconvert map file for GL" << endl;
		gObservMap.create(small.rows,small.cols,CV_8UC1 );

		float a, offset = observMapGL.property.offset;
		for( size_t i=0; i < gObservMap.total(); i++ )
		{
			a = ( small.data[i] - offset );
			a =  a / (255.0f - offset) * 255;
			gObservMap.data[i] = static_cast<unsigned char>( a );
		}


		cerr << "\t" << gObservMap.rows << " " << gObservMap.cols << endl
				<< "\t" << gObservMap.depth() << ", " << gObservMap.channels() << ", " << gObservMap.elemSize() << endl;

		cerr << "\tset texture." << endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, gObservMap.cols, gObservMap.rows, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, gObservMap.data );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, gObservMap.cols, gObservMap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, gObservMap.data );
	}*/
	return true;

}

bool textureLoadRoadMap( )
{/*
	glBindTexture( GL_TEXTURE_2D, textures[TEXTURE_ROADMAP] );
	cerr << "map file open '" << roadMapGL.property.mapName << "'." << endl;
	cv::Mat raw = cv::imread( roadMapGL.property.mapName, 0), small;
	if( !raw.data )
	{
		cerr << "Warning: cannot open map file '" << roadMapGL.property.mapName << "'." << endl;
		return false;
	}
	else
	{
		int z = (raw.cols > raw.rows ? raw.cols : raw.rows );
		while( z >= 4096 )
		{
			gRoadMapZoom *= 0.5;
			z /= 2;
		}
		cerr << "\tresize map file" << endl;
		cv::resize( raw, small, cv::Size(), gRoadMapZoom, gRoadMapZoom,cv::INTER_NEAREST );
		// アルファチャンネルの追加
		cerr << "\tconvert map file for GL" << endl;
		gRoadMap.create(small.rows,small.cols,CV_8UC4 );
		unsigned char *c;

		for( size_t i=0; i < gRoadMap.total(); i++ )
		{
			//cout <<i<<  "," << (int)small.data[i]  << endl;
			c = gColorIndex.get( (int)small.data[i] );
			gRoadMap.data[i*4    ] = c[0];
			gRoadMap.data[i*4 + 1] = c[1];
			gRoadMap.data[i*4 + 2] = c[2];//255;
			if( small.data[i] == ROAD_UNKNOWN )
			{
				gRoadMap.data[i*4    ] = gRoadMap.data[i*4 + 1] = 200;
				gRoadMap.data[i*4 + 2] = 255;
				gRoadMap.data[i*4 + 3] = 155;
			}
			else if( small.data[i] == ROAD_NO_EXIST )
				gRoadMap.data[i*4 + 3] = 0;
			else
				gRoadMap.data[i*4 + 3] = 255;//c[3];
		}


		cerr << "\t" << gRoadMap.rows << " " << gRoadMap.cols << endl
				<< "\t" << gRoadMap.depth() << ", " << gRoadMap.channels() << ", " << gRoadMap.elemSize() << endl;

		cerr << "\tset texture." << endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, gRoadMap.cols, gRoadMap.rows, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, gRoadMap.data );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gRoadMap.cols, gRoadMap.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, gRoadMap.data );
	}*/
	return true;
}

bool textureLoadRunnableMap( )
{/*
	glBindTexture( GL_TEXTURE_2D, textures[TEXTURE_RUNNABLE_MAP] );
	cerr << "map file open '" << runnableAreaGL.property.fname << "'." << endl;
	cv::Mat raw = cv::imread( runnableAreaGL.property.fname, 0), small, edge, edgeDilate, mask;
	if( !raw.data )
	{
		cerr << "Warning: cannot open map file '" << runnableAreaGL.property.fname << "'." << endl;
		return false;
	}
	else
	{
		int z = (raw.cols > raw.rows ? raw.cols : raw.rows );
		while( z >= 4096 )
		{
			gRunnableMapZoom *= 0.5;
			z /= 2;
		}
		cerr << "\tresize map file" << endl;
		cv::resize( raw, small, cv::Size(), gRunnableMapZoom, gRunnableMapZoom,cv::INTER_NEAREST );
#if 0
		// そのままでは見づらいので、エッジのみ表示
		cerr << "\tedge map file" << endl;
		cv::Laplacian( small, edge, CV_8U );
		// 見づらいので膨張
		cv::dilate( edge, edgeDilate, cv::Mat(), cv::Point(-1,-1), 3 );
		cv::bitwise_not( small, mask ); // マスク画像の作成
		edgeDilate.copyTo( edge, mask ); // マスキング
#endif
		// アルファチャンネルの追加
		cerr << "\tconvert map file for GL" << endl;
		gRunnableMap.create(small.rows,small.cols,CV_8UC4 );
		for( size_t i=0; i < gRunnableMap.total(); i++ )
		{
			//cout <<i<<  "," << (int)small.data[i]  << endl;
			if( small.data[i] )
			{
				gRunnableMap.data[i*4    ] = 255;
				gRunnableMap.data[i*4 + 1] = 128;
				gRunnableMap.data[i*4 + 2] = 0;//255;
				gRunnableMap.data[i*4 + 3] = 128;//c[3];
			}
			else
				gRunnableMap.data[i*4 + 3] = 0;//c[3];
		}


		cerr << "\t" << gRunnableMap.rows << " " << gRunnableMap.cols << endl
				<< "\t" << gRunnableMap.depth() << ", " << gRunnableMap.channels() << ", " << gRunnableMap.elemSize() << endl;

		cerr << "\tset texture." << endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, gRoadMap.cols, gRoadMap.rows, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, gRoadMap.data );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gRunnableMap.cols, gRunnableMap.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, gRunnableMap.data );
	}*/
	return true;
}

void drawFreeMapGL( int skip )
{/*
	if( !gFreeMap.data )
		return;
	glColor4f(1.0,1.0,1.0,1.0);
	glBindTexture( GL_TEXTURE_2D, textures[TEXTURE_FREEMAP] );
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glTranslatef(0,0,POS_FREEMAP_Z);
	glBegin(GL_QUADS);
	{
		// upper left
		glTexCoord2f(0, 0);
		glVertex2f( freeMapGL.property.point[2].x, freeMapGL.property.point[2].y );
		// upper right
		glTexCoord2f(1, 0);
		glVertex2f( freeMapGL.property.point[3].x, freeMapGL.property.point[3].y );
		// bottom right
		glTexCoord2f(1.0, 1.0);
		glVertex2f( freeMapGL.property.point[0].x, freeMapGL.property.point[0].y );
		// bottom left
		glTexCoord2f(0.0, 1.0 );
		glVertex2f( freeMapGL.property.point[1].x, freeMapGL.property.point[1].y );
	}
	glEnd();
	glPopMatrix(  );
	glDisable(GL_TEXTURE_2D);
*/
}

void drawObservMapGL( int skip )
{
	glBegin(GL_POINTS);
	for(int r = 0; r < (signed) observmap.row(); r++) {
		for(int c = 0; c < (signed) observmap.column(); c++) {
			double v;
			unsigned char _v;
			double x, y;
			
			observmap.pget_pos_core(r, c, &x, &y);
			observmap.get(r, c, &_v);
			v = (double)_v / 255;
			if(v == 0) continue;
			glColor3d(v, v, v);
			glVertex2d(x, y);
		}
	}
	glEnd();

/*
	if( !gObservMap.data )
		return;
	glColor3f(1.0,1.0,1.0);
	glBindTexture( GL_TEXTURE_2D, textures[TEXTURE_OBSERVMAP] );
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glTranslatef(0,0,POS_OBSERVMAP_Z);
	glBegin(GL_QUADS);
	{
		// upper left
		glTexCoord2f(0, 0);
		glVertex2f( observMapGL.property.point[2].x, observMapGL.property.point[2].y );
		// upper right
		glTexCoord2f(1, 0);
		glVertex2f( observMapGL.property.point[3].x, observMapGL.property.point[3].y );
		// bottom right
		glTexCoord2f(1.0, 1.0);
		glVertex2f( observMapGL.property.point[0].x, observMapGL.property.point[0].y );
		// bottom left
		glTexCoord2f(0.0, 1.0 );
		glVertex2f( observMapGL.property.point[1].x, observMapGL.property.point[1].y );
	}
	glEnd();
	glPopMatrix(  );
	glDisable(GL_TEXTURE_2D);
*/

}


void drawRoadMapGL( int skip )
{	/*
	if( !gRoadMap.data )
		return;
	glColor4f(1.0,1.0,1.0,1.0);
	glBindTexture( GL_TEXTURE_2D, textures[TEXTURE_ROADMAP] );
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glTranslatef(0,0,POS_ROADMAP_Z );
	glBegin(GL_QUADS);
	{
		// upper left
		glTexCoord2f(0, 0);
		glVertex2f( roadMapGL.property.point[0].x, roadMapGL.property.point[0].y );
		// upper right
		glTexCoord2f(1, 0);
		glVertex2f( roadMapGL.property.point[1].x, roadMapGL.property.point[1].y );
		// bottom right
		glTexCoord2f(1.0, 1.0);
		glVertex2f( roadMapGL.property.point[2].x, roadMapGL.property.point[2].y );
		// bottom left
		glTexCoord2f(0.0, 1.0 );
		glVertex2f( roadMapGL.property.point[3].x, roadMapGL.property.point[3].y );
	}
	glEnd();
	glPopMatrix(  );
	glDisable(GL_TEXTURE_2D);*/

}

void drawRunnableMapGL( int skip )
{/*
	if( !gRunnableMap.data )
		return;
	glColor4f(1.0,1.0,1.0,1.0);
	glBindTexture( GL_TEXTURE_2D, textures[TEXTURE_RUNNABLE_MAP] );
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glTranslatef(0,0,POS_RUNNABLE_Z);
	glBegin(GL_QUADS);
	{
		// upper left
		glTexCoord2f(0, 0);
		glVertex2dv( (double *)&runnableAreaGL.property.vertex[0] );
		// upper right
		glTexCoord2f(1, 0);
		glVertex2dv( (double *)&runnableAreaGL.property.vertex[3] );
		// bottom right
		glTexCoord2f(1.0, 1.0);
		glVertex2dv( (double *)&runnableAreaGL.property.vertex[2] );
		// bottom left
		glTexCoord2f(0.0, 1.0 );
		glVertex2dv( (double *)&runnableAreaGL.property.vertex[1] );
	}
	glEnd();
	glPopMatrix(  );
	glDisable(GL_TEXTURE_2D);
*/
}


#if 0
void drawConfigurationFS(  )
{
	if( !configurationFS.isOpen(  ) )
		return;
	configurationFS.readNew(  );

	glColor4f(1.0,0.0,0.0,1.0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture( GL_TEXTURE_2D, textures[TEXTURE_CONFIGURATION] );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, gRoadMap.cols, gRoadMap.rows, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, gRoadMap.data );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, configurationFS.data.width, configurationFS.data.height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, configurationFS.data.data );

	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glTranslatef( 0,0,POS_CONFIGURATION_Z );
	glBegin(GL_QUADS);
	{
		// upper left
		glTexCoord2f(0, 0);
		glVertex2dv( (double *)&configurationFS.property.vertex[2] );
		// upper right
		glTexCoord2f(1, 0);
		glVertex2dv( (double *)&configurationFS.property.vertex[3] );
		// bottom right
		glTexCoord2f(1.0, 1.0);
		glVertex2dv( (double *)&configurationFS.property.vertex[0] );
		// bottom left
		glTexCoord2f(0.0, 1.0 );
		glVertex2dv( (double *)&configurationFS.property.vertex[1] );
	}
	glEnd();
	glPopMatrix(  );
	glDisable(GL_TEXTURE_2D);

}
#endif
/*
void drawKinectRGB( float width )
{
	float height = width * 3.0 /4.0;
	if( gIsDraw[BUTTON_KINECT] % 2 && kinectCam.isOpen() )
	{
		kinectCam.readNew(  );
#if 1
		glBindTexture( GL_TEXTURE_2D, textures[TEXTURE_KINECT] );

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, gFreeMap.cols, gFreeMap.rows, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, gFreeMap.data );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ssm::KINECT_RGB_PIX_X, ssm::KINECT_RGB_PIX_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, kinectCam.data.image );
		glColor4f(1,1,1,1.0);
		glEnable(GL_TEXTURE_2D);

		glBegin(GL_QUADS);
		{
			// upper left
			glTexCoord2f(0, 0);
			glVertex2f( 0, 0 );
			// upper right
			glTexCoord2f(1, 0);
			glVertex2f( width, 0 );
			// bottom right
			glTexCoord2f(1.0, 1.0);
			glVertex2f( width, height );
			// bottom left
			glTexCoord2f(0.0, 1.0 );
			glVertex2f( 0.0, height );
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);


#else
		glPixelZoom( 1, -1 );
		glRasterPos2f(gWindowSize[0] - ssm::KINECT_RGB_PIX_X , 0);
//		glRasterPos2f(0 , 0);
		glDrawPixels( ssm::KINECT_RGB_PIX_X, ssm::KINECT_RGB_PIX_Y, GL_RGB, GL_UNSIGNED_BYTE, kinectCam.data.image ); //画像を貼る
#endif

	}
}*/


