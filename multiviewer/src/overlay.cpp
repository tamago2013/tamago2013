/*
 * overlay.cpp
 *
 *  Created on: 2011/10/27
 *      Author: shikina
 */

#include <iostream>
#include "GLdraw.hpp"
#include "texture.hpp"
using namespace std;

static const double OVERLAY_RATE = 0.35;

void drawConfigurationOL(  )
{
//	double xMin = INFINITY, xMax = -INFINITY, yMin = INFINITY, yMax = -INFINITY;
/*	int width, height;
	if( !( gIsDraw[BUTTON_CONFIGURATION] %2) )
		return;

	if( !configurationFS.isOpen(  ) )
		return;

//	configurationFS.readNew(  );
//	plannedPathGL.readNew(  );

	width = height = gWindowSize[0] * OVERLAY_RATE;

	// 座標の設定
	for( int i = 0; i < 4; i++ )
	{
		xMin = ( configurationFS.data.vertex[i].x < xMin ? configurationFS.data.vertex[i].x : xMin );
		yMin = ( configurationFS.data.vertex[i].y < yMin ? configurationFS.data.vertex[i].y : yMin );
		xMax = ( configurationFS.data.vertex[i].x > xMax ? configurationFS.data.vertex[i].x : xMax );
		yMax = ( configurationFS.data.vertex[i].y > yMax ? configurationFS.data.vertex[i].y : yMax );
	}


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(xMin, xMax, yMin, yMax, 0, 1);
//		glOrtho(0, 1, 1, 0, 0.0, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glViewport( gWindowSize[0] - width, gWindowSize[1] - height, width, height );



	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture( GL_TEXTURE_2D, textures[TEXTURE_CONFIGURATION] );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, gRoadMap.cols, gRoadMap.rows, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, gRoadMap.data );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, configurationFS.data.width, configurationFS.data.height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, configurationFS.data.data );

	{
		glPushMatrix();
		glRotatef(90,0,0,1);
//		glTranslatef( 0,0,POS_CONFIGURATION_Z );
//		glTranslatef( gWindowSize[0] - width,0, -0.1 );
		glTranslatef( 0,0, -0.1 );

#if 1

//		glColor4f( 0.0,0.0,0.0, 0.8 );
		glColor4f( 1.0,1.0,1.0,1.0 );
		glBegin(GL_QUADS);
		{
#if 1
			glVertex2dv( (double *)&configurationFS.data.vertex[0] );
			glVertex2dv( (double *)&configurationFS.data.vertex[1] );
			glVertex2dv( (double *)&configurationFS.data.vertex[2] );
			glVertex2dv( (double *)&configurationFS.data.vertex[3] );
#else
			glVertex2i( 0,0 );
			glVertex2i( width, 0 );
			glVertex2i( width, height );
			glVertex2i( 0, height );
#endif
		}
		glEnd();

		glTranslatef( 0,0, 0.05 );

		// 経路
		glPointSize(5.0 );
		glColor3f(1.0,0.0,0.0);
		glBegin( GL_POINTS );
		for( int i = 0; i < plannedPathGL.data.count; i++ )
		{
			//cout << plannedPathGL.data.data[i].x << ' ' << plannedPathGL.data.data[i].y << ' '  << plannedPathGL.data.data[i].theta << endl;
			//drawPosition( plannedPathGL.data.data[i].x,plannedPathGL.data.data[i].y,plannedPathGL.data.data[i].theta);
			glVertex2d( plannedPathGL.data.data[i].x, plannedPathGL.data.data[i].y );
		}
		glEnd();

		// robot
		static float rpos[7][2] = {{0.190 ,0.198},{ 0.110 ,0.258 }, {-0.445, 0.258 }, {-0.565, 0.0}, {-0.445, -0.258 }, {0.110, -0.258}, {0.190 ,-0.198}};
		static float cpos[4][2] = {{0.190,0.0},{-0.565,0.0}, {0.0,0.258},{0.0, -0.258}};
		glColor3f(0.5,0.5,0.5);
		glLineWidth( 2.0 );
		glBegin( GL_LINE_LOOP );
		for( int i= 0; i< 7; i++ )
		{
			glVertex2fv( rpos[i] );
		}
		glEnd();
		glLineWidth( 1.0 );
		glBegin( GL_LINES );
		for( int i= 0; i< 4; i++ )
		{
			glVertex2fv( cpos[i] );
		}
		glEnd();

		// waypoint
		glPushMatrix(  );
		glRotatef( -rad2deg( odmGL.data.theta), 0,0,1 );
		glTranslatef( -odmGL.data.x,-odmGL.data.y,-0.4 );
		drawWayPointGL( 0 );
		glPopMatrix(  );
		// コンフィグレーション空間
		glColor4f(0.0,1.0,0.0,1.0);
		glEnable(GL_TEXTURE_2D);

		glBegin(GL_QUADS);
		{
			// upper left
			glTexCoord2f(0, 0);
			glVertex2dv( (double *)&configurationFS.data.vertex[2] );
			// upper right
			glTexCoord2f(1, 0);
			glVertex2dv( (double *)&configurationFS.data.vertex[3] );
			// bottom right
			glTexCoord2f(1.0, 1.0);
			glVertex2dv( (double *)&configurationFS.data.vertex[0] );
			// bottom left
			glTexCoord2f(0.0, 1.0 );
			glVertex2dv( (double *)&configurationFS.data.vertex[1] );

#if 0
			// upper left
			glTexCoord2f(1, 1);
			glVertex2i( 0,0 );
			// upper right
			glTexCoord2f(1, 0);
			glVertex2i( width, 0 );
			// bottom right
			glTexCoord2f(0.0, 0.0);
			glVertex2i( width, height );
			// bottom left
			glTexCoord2f(0.0, 1.0 );
			glVertex2i( 0, height );
#endif
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
#endif

		glPopMatrix(  );
	}

	glViewport( 0,0,gWindowSize[0],gWindowSize[1] );
	glPopMatrix(  );
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(  );
	glMatrixMode(GL_MODELVIEW);
*/
}


void drawWorldMap()
{
//	int width, height;
/*
//	const static int WORLD_MAP_WIDTH = 400;
//	const static int WORLD_MAP_HEIGHT = 300;
	double xMin = INFINITY, xMax = -INFINITY, yMin = INFINITY, yMax = -INFINITY;
	double xSize, ySize;

	width = gWindowSize[0] * OVERLAY_RATE;
	height = gWindowSize[1] - width;
	if( height < 0 )
		return ;

	if( !gIsDrawWorldMap )
		return;

	if( !( freeMapGL.isOpen(  ) || roadMapGL.isOpen(  ) || runnableAreaGL.isOpen(  ) ) )
		return;

	// サイズの計算
	for( int i = 0; i < 4; i++ )
	{
		xMin = ( freeMapGL.property.point[i].x < xMin ? freeMapGL.property.point[i].x : xMin );
		yMin = ( freeMapGL.property.point[i].y < yMin ? freeMapGL.property.point[i].y : yMin );
		xMax = ( freeMapGL.property.point[i].x > xMax ? freeMapGL.property.point[i].x : xMax );
		yMax = ( freeMapGL.property.point[i].y > yMax ? freeMapGL.property.point[i].y : yMax );

		xMin = ( roadMapGL.property.point[i].x < xMin ? roadMapGL.property.point[i].x : xMin );
		yMin = ( roadMapGL.property.point[i].y < yMin ? roadMapGL.property.point[i].y : yMin );
		xMax = ( roadMapGL.property.point[i].x > xMax ? roadMapGL.property.point[i].x : xMax );
		yMax = ( roadMapGL.property.point[i].y > yMax ? roadMapGL.property.point[i].y : yMax );

		xMin = ( runnableAreaGL.property.vertex[i].x < xMin ? runnableAreaGL.property.vertex[i].x : xMin );
		yMin = ( runnableAreaGL.property.vertex[i].y < yMin ? runnableAreaGL.property.vertex[i].y : yMin );
		xMax = ( runnableAreaGL.property.vertex[i].x > xMax ? runnableAreaGL.property.vertex[i].x : xMax );
		yMax = ( runnableAreaGL.property.vertex[i].y > yMax ? runnableAreaGL.property.vertex[i].y : yMax );

	}

	xSize = xMax - xMin;
	ySize = yMax - yMin;

	if( xSize / ySize > (double)width / height )
	{
		ySize = xSize / ((double)width / height );
	}
	else
		xSize = ySize * ((double)width / height );


	// 描画
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(xMin, xMin + xSize, yMin, yMin + ySize, 0, 1);
//		glOrtho(0, 1, 1, 0, 0.0, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glViewport( gWindowSize[0] - width, 0, width, height );

	{
		glColor3f(1.0,1.0,1.0 );
		glLineWidth( 5.0 );
		// 枠
		glPushMatrix();
		glTranslatef(0.0,0.0,-0.15 );
		glBegin( GL_LINE_LOOP );
		glVertex2f( xMin, yMin );
		glVertex2f( xMin, yMin + ySize -3);
		glVertex2f( xMin + xSize - 3 , yMin + ySize -3);
		glVertex2f( xMin + xSize - 3, yMin );
		glEnd(  );
		// 背景
		glColor4f(0.0,0.0,0.0, 0.5 );
		glBegin( GL_QUADS );
		{
			glVertex2f( xMin, yMin );
			glVertex2f( xMin + xSize, yMin );
			glVertex2f( xMin + xSize, yMin + ySize );
			glVertex2f( xMin, yMin + ySize );
		}
		glEnd();
		glPopMatrix();

		glColor4f(1.0,1.0,1.0, 1.0 );
		glPushMatrix();
		glTranslatef(( xSize - xMax + xMin )/ 2.0,( ySize - yMax + yMin )/ 2.0,0.0 );
		// 地図の描画
		// 自己位置の描画
		{
			GLfloat TRIANGLE[][2] = {{xSize / 30.0, 0.0}, {-xSize / 60.0,-xSize/50},{-xSize / 60.0,xSize/50}};
			glColor4f(1.0,0.0,0.0, 1.0 );
			glPushMatrix(  );
			// 自己位置
			glTranslated( odmGL.data.x, odmGL.data.y, -0.01 );
			glRotated(rad2deg( odmGL.data.theta ), 0.0, 0.0, 1.0 );
			glBegin( GL_TRIANGLES );
			glVertex2fv( TRIANGLE[0] );
			glVertex2fv( TRIANGLE[1] );
			glVertex2fv( TRIANGLE[2] );
			glEnd();
			glColor4f(1.0,1.0,1.0, 1.0 );
			glLineWidth( 2.0 );
			glBegin( GL_LINE_LOOP );
			glVertex2fv( TRIANGLE[0] );
			glVertex2fv( TRIANGLE[1] );
			glVertex2fv( TRIANGLE[2] );
			glEnd();
			glPopMatrix(  );

			// 線
			glPushMatrix(  );
			glTranslated( 0, 0, -0.005 );
			glColor4f(1.0,1.0,1.0, 1.0 );
			glLineWidth( 1.0 );
			glBegin( GL_LINES );
			glVertex2d( xMin- xSize, odmGL.data.y ); glVertex2d( xMax + xSize, odmGL.data.y );
			glVertex2d( odmGL.data.x, yMin- ySize); glVertex2d( odmGL.data.x, yMax + ySize );
			glEnd(  );
			glPopMatrix(  );

		}
		glPushMatrix(  );
		glTranslatef( 0,0, 0.06 );
		drawRoadMapGL( 0 );
		drawFreeMapGL( 0 );
		drawRunnableMapGL( 0 );
		glPopMatrix(  );

		glPopMatrix(  );

	}

	glViewport( 0,0,gWindowSize[0],gWindowSize[1] );
	glPopMatrix(  );
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(  );
	glMatrixMode(GL_MODELVIEW);
*/
//	glColor4f(0.0,0.0,0.0, 0.5 );
//	glBegin( GL_QUADS );
//	{
//		glVertex2i( gWindowSize[0] - WORLD_MAP_WIDTH, gWindowSize[1] - WORLD_MAP_HEIGHT );
//		glVertex2i( gWindowSize[0], gWindowSize[1] - WORLD_MAP_HEIGHT );
//		glVertex2i( gWindowSize[0], gWindowSize[1] );
//		glVertex2i( gWindowSize[0] - WORLD_MAP_WIDTH, gWindowSize[1] );
//	}
//	glEnd();
}

void drawVelMeter( int v, int x, int y )
{
	static const int TRIANGLES_POS[][2] = {{9, -12}, {18, 6},{0, 6}};
	static const int DIFF = 20;
	int i,j, yy;
	glLineWidth( 1.0 );
	glBegin( GL_TRIANGLES );
	for( i = 1; i <= v; i++ )
	{
		yy = y - i * DIFF;

		glVertex2i( TRIANGLES_POS[0][0] + x, TRIANGLES_POS[0][1] + yy );
		glVertex2i( TRIANGLES_POS[1][0] + x, TRIANGLES_POS[1][1] + yy);
		glVertex2i( TRIANGLES_POS[2][0] + x, TRIANGLES_POS[2][1] + yy);
	}
	for( j = -1; j >= v; j-- )
	{
		yy = y - j * DIFF;

		glVertex2i( TRIANGLES_POS[0][0] + x, -TRIANGLES_POS[0][1] + yy);
		glVertex2i( TRIANGLES_POS[1][0] + x, -TRIANGLES_POS[1][1] + yy);
		glVertex2i( TRIANGLES_POS[2][0] + x, -TRIANGLES_POS[2][1] + yy);
	}
	glEnd();

	for( ;i <= 10; i++ )
	{
		yy = y - i * DIFF;
		glBegin( GL_LINE_LOOP );

		glVertex2i( TRIANGLES_POS[0][0] + x, TRIANGLES_POS[0][1] + yy );
		glVertex2i( TRIANGLES_POS[1][0] + x, TRIANGLES_POS[1][1] + yy );
		glVertex2i( TRIANGLES_POS[2][0] + x, TRIANGLES_POS[2][1] + yy );

		glEnd();
	}
	for( ; j >= -10; j-- )
	{
		yy = y - j * DIFF;

		glBegin( GL_LINE_LOOP );
		glVertex2i( TRIANGLES_POS[0][0] + x, -TRIANGLES_POS[0][1] + yy);
		glVertex2i( TRIANGLES_POS[1][0] + x, -TRIANGLES_POS[1][1] + yy);
		glVertex2i( TRIANGLES_POS[2][0] + x, -TRIANGLES_POS[2][1] + yy);

		glEnd();
	}

}

void drawStatusMeter()
{
	const static int STATUS_METER_WIDTH = 120;
	int v, w;


	glColor3f( 1,1,1);
	int x, y;
	x = 1;
	y= 24;
//	drawString( "Tsukuba", x,y,0 ) ;
	y += 24;
//	drawString( "Challenge", x,y,0 ) ;
//	y += 24;
//	drawString( "2011", x,y,0 ) ;

	// 速度メータ
	v = odmBS.data.v * 10;
	w = sqrt( fabs( odmBS.data.w) ) * 10;
	if(odmBS.data.w < 0)
		w = -w;
	y = 300;
	x = 20;
	drawVelMeter( v, x, y );
	drawString( "v", x + 2, y + 6,0) ;
	x = 65;
	drawVelMeter( w, x, y );
	drawString( "w", x + 2, y + 6,0) ;

	// 現在時刻

	char day[64];
	time_t t = gettimeSSM(  );
	x = 1;
	y = 24;//gWindowSize[1] - 30;
	strftime( day, sizeof ( day ), "%m/%d(%a)", localtime( &t ) );
	drawString( day, x,y,0 ) ;
	y += 24;
	strftime( day, sizeof ( day ), " %H:%M:%S", localtime( &t ) );
	drawString( day, x,y,0 ) ;

	// 背景の描画
	glColor4f(0.0,0.0,0.0, 0.5 );
	glBegin( GL_QUADS );
	{
		glVertex2i( 0, 0 );
		glVertex2i( STATUS_METER_WIDTH, 0 );
		glVertex2i( STATUS_METER_WIDTH, gWindowSize[1] );
		glVertex2i( 0, gWindowSize[1] );
	}
	glEnd();



}


void drawWindow()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, gWindowSize[0], gWindowSize[1], 0, 0.0, 1);
//		glOrtho(0, 1, 1, 0, 0.0, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	{
		glPushMatrix(  );
		glTranslatef( 0, gWindowSize[1]- ( 200 / 4.0 * 3.0), 0 );
//		drawKinectRGB( 200 );
		glPopMatrix(  );
		drawWorldMap();
		if( gWindowSize[0] > 900 )
			drawStatusMeter();
		drawConfigurationOL(  );
		drawHelp();
	}

	glPopMatrix(  );
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(  );
	glMatrixMode(GL_MODELVIEW);


}
