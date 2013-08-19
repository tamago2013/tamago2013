/*
 * GLdraw.hpp
 *
 *  Created on: 2010/07/25
 *      Author: shikina
 */

#ifndef GLDRAW_HPP_
#define GLDRAW_HPP_

#include <GL/glut.h>
#include "GLcommon.hpp"
#include "stktable.hpp"


#include "ssm-laser.hpp"
//#include <ssm-obstacles.hpp>
#include "ssm-particles.hpp"
//#include <ssm-free-area-map.hpp>
#include <ssm-opsm-map.hpp>
//#include <ssm-waypoint.hpp>
//#include <ssm-road-estimator.hpp>
//#include <ssm-pathplan.hpp>
//#include <ssm-kinect.hpp>


static const float POS_PARTICLE_Z = 0.5;
static const float POS_WAYPOINT_Z = 0.4;
static const float POS_CONFIGURATION_Z = 0.1;
static const float POS_RUNNABLE_Z = 0.05;
static const float POS_OBSERVMAP_Z = -0.07;
static const float POS_FREEMAP_Z = -0.1;
static const float POS_ROADMAP_Z = -0.2;
static const float POS_PLANNDEPATH_Z = 0.4;

static indexcolor::PseudoColorKBGW<256> gColorIndex;

enum
{
	DRAW_MODE_URG_COLOR = 0,	// 色を指定したカラーで表示するモード
	DRAW_MODE_HIGHT,			// 色を高さに対応させて表示するモード
	DRAW_MODE_REFLECTION,		// 色を反射強度に対応させて表示するモード
	DRAW_MODE_NUM				// 描画モードの数
};

enum
{
	DRAW_TYPE_URG_NODRAW = 0,	// 描画無し
	DRAW_TYPE_URG_POINT,		// 点で描画
	DRAW_TYPE_URG_BEAM,			// ビームモード
	DRAW_TYPE_URG_NUM			// 描画タイプの数
};



// ボタンとの対応付け
enum
{
	BUTTON_PARTICLE = 0,		///< パーティクル
	BUTTON_URG_FRONT,			///< URG1,3,4
	BUTTON_URG_BOTTOM,			///< URG2
	BUTTON_URG_BACK,			///< URG3,4
	BUTTON_URG_BOTTOM_OBS,		///< URG2 障害物
	BUTTON_KINECT,				///< kinect
	BUTTON_FREEMAP,				///< フリー地図
	BUTTON_ROADMAP,				///< 路面地図
	BUTTON_RUNNABLE_MAP,		///< 走行可能領域
//	BUTTON_PATH_PLAN,			///< 計画された経路
	BUTTON_CONFIGURATION,		///< コンフィグレーション空間
};


bool myInit();

void drawGrid(double x, double y, double th);
int drawURGFS(int group);
int drawURGBS(int group, int cull);
void drawRobotFS(double cx, double cy, double cq);
void drawWayPointGL( int skip );
void drawAll(int mode);

inline double deg2rad(double th){return th * M_PI / 180.0;}
inline double rad2deg(double th){return th / M_PI * 180.0;}


extern int gDrawDistMin; // [mm] 最小描画距離
extern int gDrawDistMax; // [mm] 最大描画距離
extern double gDrawHightMax; // [m] 最大描画高さ
extern double gDrawHightMin; // [m] 最小描画高さ
extern double gDrawReflectMax; // 最大反射強度
extern double gDrawReflectMin; // 最小反射強度

extern SSMApi<Spur_Odometry, Spur_Odometry_Property> odmBS;
extern SSMApi<Spur_Odometry, Spur_Odometry_Property> odmGL;

extern SSMSOKUIKIData3D urgFS1;
extern SSMSOKUIKIData3D urgBS2;
extern SSMSOKUIKIData3D urgFS3;
extern SSMSOKUIKIData3D urgFS4;
extern SSMSOKUIKIData3D urgObsBS2;

extern SSMParticles particlesGL;
//extern SSMFreeAreaMap freeMapGL;
//extern SSMRoadEstimator roadMapGL;
extern SSMOPSMMap observMapGL;

//extern SSMWayPoint wayPointGL;
//extern SSMPlannedPath plannedPathGL;
//extern SSMConfiguration configurationFS;
//extern SSMRunnableArea runnableAreaGL;

//extern SSMKinectRGB kinectCam;


#endif /* GLDRAW_HPP_ */
