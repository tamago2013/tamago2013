/*
 * texture.hpp
 *
 *  Created on: 2011/10/26
 *      Author: shikina
 */

#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_


// テクスチャ配列の対応付け
enum
{
	TEXTURE_FREEMAP = 0,
	TEXTURE_OBSERVMAP,
	TEXTURE_ROADMAP,
	TEXTURE_CONFIGURATION,
	TEXTURE_RUNNABLE_MAP,
	TEXTURE_KINECT,
	TEXTURE_NUM,
};

extern GLuint textures[TEXTURE_NUM];

bool textureLoadFreeMap( );
bool textureLoadObservMap(  );
bool textureLoadRoadMap( );
bool textureLoadRunnableMap( );

void drawFreeMapGL( int skip );
void drawObservMapGL( int skip );
void drawRoadMapGL( int skip );
void drawRunnableMapGL( int skip );
void drawConfigurationFS(  );

void drawKinectRGB( float width );
void drawWorldMap(  );
void drawVelMeter( int v, int x, int y );
void drawStatusMeter(  );
void drawWindow(  );

#endif /* TEXTURE_HPP_ */
