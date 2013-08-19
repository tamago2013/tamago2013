/*
 * GLdraw.cpp
 *
 *  Created on: 2010/07/25
 *      Author: shikina
 */


#include <iostream>
#include <vector>
#include <cmath>

#include <ssm.hpp>
#include <GL/glut.h>
#include <ssmtype/spur-odometry.h>

#include "GLcommon.hpp"
#include "GLdraw.hpp"
#include "texture.hpp"


#define var2string( var ) # var
using namespace std;


SSMApi<Spur_Odometry, Spur_Odometry_Property> odmBS( SNAME_ODOMETRY, 0 );
SSMApi<Spur_Odometry, Spur_Odometry_Property> odmGL( SNAME_ADJUST, 0 );

SSMSOKUIKIData3D urgFS1( SSM_NAME_SOKUIKI_3D_FS, 0 );
SSMSOKUIKIData3D urgBS2( SSM_NAME_SOKUIKI_3D_BS, 2 );
SSMSOKUIKIData3D urgFS3( SSM_NAME_SOKUIKI_3D_FS, 3 );
SSMSOKUIKIData3D urgFS4( SSM_NAME_SOKUIKI_3D_FS, 4 );
//SSMSOKUIKIData3D urgObsBS2( SSM_NAME_OBSTACLE, 0 );

SSMParticles particlesGL(SNAME_PARTICLES ,0 );
//SSMFreeAreaMap freeMapGL(SNAME_FREE_AREA_MAP);
//SSMRoadEstimator roadMapGL( SSM_NAME_ROAD_ESTIMATOR, 0 );
SSMOPSMMap observMapGL( SNAME_OPSM_MAP, 0 );

//SSMWayPoint wayPointGL( SSM_NAME_WAYPOINT, 0 );
//SSMPlannedPath plannedPathGL( SSM_NAME_PLANNED_PATH, 0 );
//SSMConfiguration configurationFS( SSM_NAME_CONFIGURATION, 0 );
//SSMRunnableArea runnableAreaGL( SSM_NAME_RUNNABLE_AREA, 0 );

//SSMKinectRGB kinectCam( SSM_NAME_KINECT_RGB, 0 );



int gDrawDistMin = 20; // 最大描画距離
int gDrawDistMax = 30000; // 最大描画距離	
double gDrawHightMax = 30.0; // 最大描画高さ
double gDrawHightMin = -30.0; // 最小描画高さ
double gDrawReflectMax = INFINITY; // 最大反射強度
double gDrawReflectMin = -INFINITY; // 最小反射強度



#if 0
GLfloat UserColors[][3] =
{
	{1,1,1},
	{1,0,0},
	{1,1,0},
	{0,1,0},
	{0,1,1},
	{0,0,1},
	{1,0,1},
	{1,.5,0},
	{.5,1,0},
	{0,.5,1},
	{.5,0,1},
	{1,0,.5},
	{1,.5,.5},
	{.5,1,.5},
	{1,.5,.5},
	{.5,.5,1},
};
#endif

void myEventKey(unsigned char key, int x, int y)
{
	switch(key)
	{
	default : break;
	}
}


bool myInit()
{
	bool ret = true;

	glGenTextures( TEXTURE_NUM, textures );
	gDrawMode = DRAW_MODE_REFLECTION;
	cerr << "stream open" << endl;
	// Spur
	odmBS.open();
	odmGL.open();

	// URG
	if( urgFS1.open( SSM_READ ) ){
		urgFS1.getProperty();
		urgFS1.data.alloc(urgFS1.property.numPoints);
		gIsDraw[BUTTON_URG_FRONT] = 2;
	}
	if( urgBS2.open( SSM_READ ) ) gIsDraw[BUTTON_URG_BOTTOM] = 1;
//	if( urgObsBS2.open( SSM_READ ) ) gIsDraw[BUTTON_URG_BOTTOM_OBS] = 1;
	if( urgFS3.open( SSM_READ ) ) gIsDraw[BUTTON_URG_BACK] = 1;
	if( urgFS4.open( SSM_READ ) ) gIsDraw[BUTTON_URG_BACK] = 1;

	// localizer
	if( particlesGL.open( SSM_READ ) ) gIsDraw[BUTTON_PARTICLE] = 1;
/*	if( freeMapGL.open( SSM_READ ) && freeMapGL.getProperty(  ) )
	{
		if( textureLoadFreeMap( ) )
			gIsDraw[BUTTON_FREEMAP] = 1;
	}*/
	if( observMapGL.open( SSM_READ ) && observMapGL.getProperty(  ) )
	{
		if( textureLoadObservMap() )
			gIsDraw[BUTTON_FREEMAP] = 1;
	}
/*
	if( roadMapGL.open( SSM_READ ) && roadMapGL.getProperty(  ) )
	{
		if( textureLoadRoadMap( ) )
			gIsDraw[BUTTON_ROADMAP] = 1;
	}*/
	// path planner
//	if( wayPointGL.open( SSM_READ ) && wayPointGL.getProperty(  ) ) gIsDraw[BUTTON_PATH_PLAN] = 1;
//	if( plannedPathGL.open( SSM_READ ) ) gIsDraw[BUTTON_PATH_PLAN] = 1;
//	wayPointGL.open( SSM_READ ) && wayPointGL.getProperty(  );
//	plannedPathGL.open( SSM_READ );
//	if( configurationFS.open( SSM_READ )/* && configurationFS.getProperty(  ) */ )
//	{
//		gIsDraw[BUTTON_CONFIGURATION] = 1;
//	}
//	if( runnableAreaGL.open( SSM_READ ) && runnableAreaGL.getProperty(  ) )
//	{
//		// runnalbe mapはデフォルトでは表示しない
//		if( textureLoadRunnableMap(  ) )
//			gIsDraw[BUTTON_RUNNABLE_MAP] = 0;
//	}

	// kinect
//	if( kinectCam.open( SSM_READ ) ) gIsDraw[BUTTON_KINECT] = 1;

	usrEventKey = myEventKey;
	return 	ret;
}

int drawSOKUIKIData3D( ssm::SOKUIKIData3D &data, int group, int drawType, int drawMode )
{
	if( group < 1 ){ group = 1; }
	int cnt = 0;
	GLfloat pc;
//	GLfloat color[4];
	int mode = drawMode % DRAW_MODE_NUM;
//	color[3] = 1.0;

	//cerr << data << endl;

	switch( drawType )
	{
	case DRAW_TYPE_URG_BEAM:
		glLineWidth( 1.0 );
		glBegin( GL_LINES );
		break;
	case DRAW_TYPE_URG_POINT:
		glBegin( GL_POINTS );
		break;
	default:
		cerr << "ERROR : drawSOKUIKIData3D() :" << "drawType : invalid arg." << endl;
		return 0;
		break;
	}
	for( size_t i=0; i < data.numPoints(); i += group )
	{
		// エラー処理
		if( data[i].isWarning(  ) ){ continue; }
		//if( data[i].reflect.z < gDrawHightMin || data[i].reflect.z > gDrawHightMax){continue;}
		//if( data[i].intensity < gDrawReflectMin || data[i].intensity > gDrawReflectMax){continue;}
		// 色の決定
		switch(mode)
		{
		case DRAW_MODE_REFLECTION :// 反射強度モードのとき
		{
			pc = data[i].intensity;
		}break;
		case DRAW_MODE_HIGHT : // 高さモードのとき
		{
			pc = (data[i].reflect.z )/ 0.5;
		}break;
		default : {pc = 0;} break;
		}
		if(mode != DRAW_MODE_URG_COLOR)
		{
			//glColor3fv( getGLColor( pc, color ) );
			glColor3ubv( gColorIndex.get(pc) );
		}
		/*描画*/
		glVertex3dv( data[i].reflect.vec );
		switch( drawType )
		{
//		case DRAW_TYPE_URG_:
//			glVertex3f(data[i].reflect.x, data[i].reflect.y, 0.0);
			break;
		case DRAW_TYPE_URG_BEAM:
			glVertex3dv( data[i].origin.vec );
			break;
		default:
			break;
		}

		cnt++;
	}
	glEnd();

	return cnt;
}



int drawURGFS(int group)
{
	int cnt = 0;
	int drawMode;

	drawMode = gDrawMode % DRAW_MODE_NUM;
	if( drawMode == DRAW_MODE_REFLECTION )
		drawMode = DRAW_MODE_URG_COLOR;

	glPointSize( 2.0 );
	if( gIsDraw[BUTTON_URG_FRONT] % DRAW_TYPE_URG_NUM != 0 )
	{
		glColor3f( 1.0, 0.3, 0.7);
		urgFS1.readLast(  );
		cnt += drawSOKUIKIData3D( urgFS1.data, group, gIsDraw[BUTTON_URG_FRONT] % DRAW_TYPE_URG_NUM, drawMode );
	}

	if( gIsDraw[BUTTON_URG_BACK] % DRAW_TYPE_URG_NUM != 0 )
	{
		// 左側URG
		glColor3f( 1.0, 0.8, 0.5);
		urgFS3.readLast(  );
		cnt += drawSOKUIKIData3D( urgFS3.data, group, gIsDraw[BUTTON_URG_BACK] % DRAW_TYPE_URG_NUM,drawMode );

		// 右側URG
		glColor3f( 0.8, 1.0, 0.5);
		urgFS4.readLast(  );
		cnt += drawSOKUIKIData3D( urgFS4.data, group, gIsDraw[BUTTON_URG_BACK] % DRAW_TYPE_URG_NUM,drawMode );
	}

	glPointSize(1.0);

	return cnt;

}

int drawURGBS( int group, int cull )
{
	static const size_t SAVE_NUM = 40 * 5;
	static ssm::SOKUIKIData3D data[2][ SAVE_NUM ];
	static size_t dataNum[2] = {0};
	int cnt = 0;


	glPointSize( 2.0 );

	if( gIsDraw[BUTTON_URG_BOTTOM] % 2 )
	{
		while( urgBS2.readNext(  ) )
		{
			data[0][ dataNum[0] % SAVE_NUM ] = urgBS2.data;
			dataNum[0]++;
		}
		glColor3f( 0.3, 0.5, 1.0);
		for( size_t i = 0; i < SAVE_NUM && i < dataNum[0]; i+= cull )
		{
			cnt += drawSOKUIKIData3D( data[0][i], group * 2, DRAW_TYPE_URG_POINT, gDrawMode );
		}
	}

	glPushMatrix(  );
	glTranslatef( 0.0, 0.0, 0.1 );
	glPointSize( 3.0 );
	if( gIsDraw[BUTTON_URG_BOTTOM_OBS] % 2 )
	{
/*		while( urgObsBS2.readNext(  ) )
		{
			data[1][ dataNum[1] % SAVE_NUM ] = urgObsBS2.data;
			dataNum[1]++;
		}*/
		glColor3f( 1.0, 0.0, 0.0 );
		for( size_t i = 0; i < SAVE_NUM && i < dataNum[1]; i+= cull )
		{
			cnt += drawSOKUIKIData3D( data[1][i], group * 2, DRAW_TYPE_URG_POINT, DRAW_MODE_URG_COLOR );
		}
	}
	glPopMatrix(  );

	return cnt;
}

int drawParticlesGL( int group )
{
	if( ++group < 1 ){ group = 1; }

	position_particle_t aorg;
//	position_particle_t *pos;
	gnd::matrix::fixed<1, PARTICLE_DIM> *pos;
	position_particle_t *org;//原点

	aorg.odo.x = odmGL.data.x;
	aorg.odo.y = odmGL.data.y;
	aorg.odo.theta = odmGL.data.theta;
	//org = &(position_particle_t *)particlesGL.data[0];
	org = &aorg;
	glLineWidth( 0.1 );
	glPushMatrix();//TODO:消す
	glTranslatef( 0,0,POS_PARTICLE_Z );
	glRotated(rad2deg( - org->odo.theta ), 0.0, 0.0, 1.0 );
	//glRotatef(rad2deg( -org->odo.theta ), 0.0, 0.0, 1.0 );
	//glTranslated( -org->odo.x, -org->odo.y, 0.0 );
	//glRotatef(rad2deg( -org->odo.theta ), 0.0, 0.0, 1.0 );

	//TODO:spur_odometryから描画するように変える

	//cout << "------" << endl;
	int cnt = 0;
	for( int i=0; i< particlesGL.data.size(); i+= group )
	{
		pos = particlesGL.data + i;
		glPushMatrix(  );
		//glTranslated( -( pos->odo.x - org->odo.x) , -( pos->odo.y - org->odo.y ), 0.0 );
		glTranslated( ( (*pos)[0][PARTICLE_X] - org->odo.x) , ( (*pos)[0][PARTICLE_Y] - org->odo.y ), 0.0 );
		glRotated(rad2deg( (*pos)[0][PARTICLE_THETA] ), 0.0, 0.0, 1.0 );

		//cout << rad2deg( pos->odo.theta ) << endl;

		glColor4f(1.0 * (*pos)[0][PARTICLE_EVAL] + 0.5,0.5*(*pos)[0][PARTICLE_EVAL] + 0.5,0.0, 0.5);
		glRotated(90.0, 0.0, 1.0, 0.0 ); // x軸方向に傾ける
		glScalef(5,5,5);
//		glScalef(pos->eval,pos->eval,pos->eval);
		glutSolidCone( 0.02, 0.1, 4, 0);// z軸方向に円錐を描画する関数
//		glColor3f(0.0,0.0,0.0);
//		glutWireCone( 0.02, 0.1, 4, 0);
		glPopMatrix(  );
		cnt++;
	}

	glPopMatrix();//TODO:消す

	return cnt;
}

double getGridWidth( double width )
{
	double grid;
	double scale;
	scale = log10( width );

	grid = pow(10.0, (int)scale);
	if( scale >= 0 )
	{
		if( scale - (int) scale < 0.5 )
			grid /= 2.0;
	}
	else
	{
		grid /= 10.0;
		if( scale - (int) scale < -0.5 )
			grid /= 2.0;
	}

	return grid;
}

void drawGrid(double x, double y, double th)
{
#if 0
	double width = getViewSize() * 2;
	double space = getGridWidth( width ); // 線の間隔
	int ix = x, iy = y;
	glColor3d(0.8, 0.8, 0.8);
	glLineWidth(1.0);
	glBegin(GL_LINES);
	{
		for(double i = -width; i <= width; i += space)
		{
			glVertex3f( ix + i, iy + width, 0.0 ); glVertex3f( ix + i, iy - width, 0.0 );

			glVertex3f( ix + width, iy + i, 0.0 ); glVertex3f( ix - width, iy + i, 0.0 );
		}
	}
	glEnd();
#else
/*
	double width = 30;
	int space = 1; // 線の間隔
	int ix = x, iy = y;
	glColor3d(0.8, 0.8, 0.8);
	glLineWidth(1.0);
	glBegin(GL_LINES);
	{
		for(int i = -width; i <= width; i += space)
		{
			glVertex3f( ix + i, iy + width, 0.0 ); glVertex3f( ix + i, iy - width, 0.0 );

			glVertex3f( ix + width, iy + i, 0.0 ); glVertex3f( ix - width, iy + i, 0.0 );
		}
	}
	glEnd();*/
#endif

}


void drawRobotFS(/*double cx, double cy, double cq*/)
{
	//  double p[4][3]={{0.1,0.25,0},{0.1,-0.25,0},{-0.5,-0.25,0},{-0.5,0.25,0}};
	double p[5][3] = { { 0.1, 0.25, 0 }, { 0.2, 0.0, 0.0 }, { 0.1, -0.25, 0 },
			{ -0.5, -0.25, 0 }, { -0.5, 0.25, 0 } };
	int i;

	glPushMatrix(  );
	glTranslatef( 0,0, 0.5 );
	glColor3d(0.4, 0.4, 0.4);
	glLineWidth(10.0);

	glBegin(GL_LINE_LOOP);
	for (i = 0; i < 5; i++) {
		glVertex3dv(p[i]);
	}
	glEnd();

	glPopMatrix(  );

}



void drawPosition( const double &x, const double &y, const double &theta )
{
	glPushMatrix(  );
	//glTranslated( -( pos->odo.x - org->odo.x) , -( pos->odo.y - org->odo.y ), 0.0 );
	glTranslated( x, y, 0.0 );
	glRotated(rad2deg( theta ), 0.0, 0.0, 1.0 );

	//cout << rad2deg( pos->odo.theta ) << endl;

	glRotated(90.0, 0.0, 1.0, 0.0 ); // x軸方向に傾ける
	glutSolidCone( 0.1, 0.5, 4, 0);// z軸方向に円錐を描画する関数
	glPopMatrix(  );

}

inline void drawSpurOdometry( Spur_Odometry &odm )
{
	drawPosition( odm.x, odm.y, odm.theta );
}

int drawOdometryGL(  )
{
	static const size_t SAVE_NUM = 1 * 60 * 40; // 1Hz 40分
	static Spur_Odometry data[ SAVE_NUM ];
	static size_t dataNum = 0;
	static ssmTimeT saveTime = 0.0;
	int cnt = 0;

	glColor3d(1.0, 1.0, 0.0);
	if( odmGL.time >= saveTime )
	{
		saveTime = odmGL.time + 1.0;
		data[dataNum++ % SAVE_NUM ] = odmGL.data;
	}
	for( size_t i= 0; i< SAVE_NUM && i < dataNum; i++ )
	{
		drawSpurOdometry( data[i] );
		cnt++;
	}
	return cnt;
}

int drawPlannedPathGL(  )
{
	int cnt = 0;
/*	glColor3f( 1.0, 0.5, 1.0 );
	glPushMatrix(  );
	glTranslatef( 0.0, 0.0, POS_PLANNDEPATH_Z );
	plannedPathGL.readNew(  );

	for( int i = 0; i < plannedPathGL.data.count; i++ )
	{
		//cout << plannedPathGL.data.data[i].x << plannedPathGL.data.data[i].y << plannedPathGL.data.data[i].theta << endl;
		drawPosition( plannedPathGL.data.data[i].x,plannedPathGL.data.data[i].y,plannedPathGL.data.data[i].theta);
		cnt++;
	}

	glPopMatrix(  );
*/
	return cnt;
}

void drawCircle( double r, int div )
{
	double dTh = 2.0 * M_PI / div;
	glBegin( GL_LINE_LOOP );
	for( double th = 0; th < 2 * M_PI; th += dTh )
	{
		glVertex2d( r * cos( th ), r * sin(th) );
	}
	glEnd();
}
/*
void drawWayPoint( const WayPoint &way )
{
	char buf[10];
	glPushMatrix(  );
	glTranslated( way.x, way.y, 0.0 );

	if( way.isPause )
		drawCircle( way.circle, 4 );
	else if( way.isBack )
		drawCircle( way.circle, 3 );
	else
		drawCircle( way.circle, 36 );
	sprintf(buf, "No.%d", way.id );
	//重たいので消す
	drawString( buf, 0,0,0 );

	glPopMatrix(  );
}*/

void drawWayPointGL( int skip )
{
	int i;
/*	wayPointGL.readNew(  );
	glPushMatrix(  );
	glTranslatef( 0.0,0.0, POS_WAYPOINT_Z );
	glLineWidth( 10.0 );
	glColor3f(1.0,0.5,0.0);
	// 次のウェイポイントの描画
	drawWayPoint( wayPointGL.data.nextPoint );
	glColor3f(0.0,0.7,1.0);
	drawWayPoint( wayPointGL.data.nowPoint );
	// その他ウェイポイント
	glLineWidth( 2.0 );
	for( i = 0; i < wayPointGL.property.size; i++ )
	{
		if( i ==  wayPointGL.data.nowPointNum || i == wayPointGL.data.nowPointNum + 1 )
			continue;

		if( i < wayPointGL.data.nowPointNum )
			glColor3f(0.7,0.7,0.7);
		else
			glColor3f(1,1,0);

		drawWayPoint( wayPointGL.property.wayPoints[i] );
	}

	// ウェイポイントを結ぶ線
	double speed;
	for( i = 0; i < wayPointGL.property.size - 1; i++ )
	{
		speed = wayPointGL.property.wayPoints[i].maxSpeed;
		glLineWidth( 5.0 * speed );
		if( i < wayPointGL.data.nowPointNum )
			glColor3f(0.7,0.7,0.7);
		else
			glColor3f(1,0.8,0.5);
		glBegin( GL_LINE );
		glVertex2d( wayPointGL.property.wayPoints[i].x, wayPointGL.property.wayPoints[i].y );
		glVertex2d( wayPointGL.property.wayPoints[i+1].x, wayPointGL.property.wayPoints[i+1].y );
		glEnd();
	}
	glPopMatrix(  );*/	
}



/**
 * @brief 全ての描画を行う
 * @param skip[in] 読み飛ばす点数
 */
void drawAll(int skip)
{
	// 引数の処理
	if(skip < 0){skip = 0;}

	particlesGL.readLast(  );
	odmBS.readLast(  );
	odmGL.readLast(  );

	//cerr << "drawAll : FS" << endl;
	if( !gIsFPView ){ glRotatef(rad2deg(odmGL.data.theta), 0.0, 0.0, 1.0); } // 俯瞰視点モードのとき

	//--------------------------------
	//FS座標系での描画
	drawURGFS( skip + 1 );
	drawRobotFS();
//	if( gIsDraw[BUTTON_PATH_PLAN] % 2 )
//	drawPlannedPathGL(  );


	//--------------------------------
	// BS座標系をFS座標系に変換しての描画
	// cerr << "drawAll : BS" << endl;
	glPushMatrix(); //変換行列の保存
	{
		glRotated(rad2deg(-odmBS.data.theta), 0.0, 0.0, 1.0);
		glTranslated(-odmBS.data.x, -odmBS.data.y, 0.0);

		drawURGBS(skip + 1, skip + 1);

	}
	glPopMatrix();

	if( gIsDraw[BUTTON_PARTICLE] % 2 && particlesGL.isOpen(  ) )
		drawParticlesGL( skip );//GL系に入れる
	//--------------------------------
	// GL座標系をFS座標系に変換しての描画
	glPushMatrix();
	{
		glRotated(rad2deg(-odmGL.data.theta), 0.0, 0.0, 1.0);
		glTranslated(-odmGL.data.x, -odmGL.data.y, 0.0);
		drawOdometryGL();

//		if( gIsDraw[BUTTON_PATH_PLAN] % 2 )
		drawWayPointGL( skip );
		drawGrid(odmGL.data.x, odmGL.data.y, odmGL.data.theta);

		drawObservMapGL( skip );
		// テクスチャ系は最後に描画しないと後ろが写らないので、下から描画する
		if( gIsDraw[BUTTON_ROADMAP] % 2 )
			drawRoadMapGL( skip );
		if( gIsDraw[BUTTON_FREEMAP] % 2 )
		{
			drawFreeMapGL( skip );
		}
		if( gIsDraw[BUTTON_RUNNABLE_MAP] % 2 )
			drawRunnableMapGL( skip );

	}
	glPopMatrix();
#if 0
	if( gIsDraw[BUTTON_CONFIGURATION] %2 )
		drawConfigurationFS();
#endif
/*
	glPushMatrix();
	glLoadIdentity();
	glPixelZoom( 0.1f, -0.1f );
	glRasterPos2f(20 , 10);
	glDrawPixels( gFreeMap.cols, gFreeMap.rows, GL_LUMINANCE, GL_UNSIGNED_BYTE, gFreeMap.data); //画像を貼る
	glPopMatrix();
*/
	// その他描画
	// cerr << "drawAll : etc." << endl;
	drawWindow();

}

