/*
 * GLcommon.cpp
 *
 *  Created on: 2010/07/25
 *      Author: shikina
 */
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <GL/glut.h>
#include <ssm.hpp>

#include "GLcommon.hpp"
#include "GLdraw.hpp"
#include "capture.hpp"
#include <ssmtype/joystick.hpp>
#include <rppjs-ps3.h>


using namespace std;

int gSkip = 0, gSkipBase = 0;
bool gIsFPView; // 表示が主観視点(First-Person point of View)モード
bool gIsFPCam; // カメラが主観視点(First-Person point of View)モード
int gIsDraw[10]; // 表示するかどうかのスイッチ key 0 - 9 に対応
int gDrawMode; // 描画モード key m を押すと１つずつ増加する

GLdouble gCamPos[3], gViewPos[3];
GLdouble gModelTrans[3], gModelRotate[3];
int gMousePos[2];
GLsizei gWindowSize[2] = {640,480};
double gAspect = 1;
double gViewLength; // 見る範囲
bool gMouseButton[3];
bool gIsDrawWorldMap = true;


SSMJoystick gJs( SSM_NAME_JOYSTICK , 0 );



void ( *usrEventKey)(unsigned char key, int x, int y) = NULL;

double getViewSize()
{
	return gViewLength;
}

/**
 * @brief 描画の開始
 */
void drawBegin()
{
	// 画面の消去
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	// カメラの設定
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glFrustum(-1 * .08, 1 * .08, -.08, .08, 0.1, 150.0); // 透視投影
	glOrtho(-gViewLength * gAspect, gViewLength * gAspect, -gViewLength, gViewLength, 0, 150); // 正射影

	//見る場所の設定(カメラ位置、見る位置、カメラ上方向)
	gluLookAt( gCamPos[0], gCamPos[1], gCamPos[2],   gViewPos[0], gViewPos[1], gViewPos[2],   1.0, 0.0, 0.0);

	// 物体の描画
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(gModelTrans[0], gModelTrans[1], gModelTrans[2]);
	glRotated(gModelRotate[1], 0, 1, 0);
	glRotated(gModelRotate[2], 0, 0, 1);
}

/**
 * @brief 描画の終了及び画面への出力
 */
void drawEnd()
{
	glutSwapBuffers();
	captureWriteFrame(  );
}

/**
 * @brief 任意の位置に文字列表示
 */
int drawString(const char *str, double posx, double posy, double posz)
{
	int i;
	glRasterPos3d(posx, posy, posz);
	for( i = 0; str[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
		//glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
	}
	return i;
}

/**
 * @brief ディスプレイへの文字列表示
 */
int drawStringDisplay(const char *str, double  posx, double posy)
{
	int cnt;
	glPushMatrix();
	glLoadIdentity();
	cnt = drawString(str, posx, posy, 0.0);
	glPopMatrix();
	return cnt;
}

/**
 * @brief ターミナルへのヘルプ表示
 */
void printHelp()
{
	cerr
		<< "--------------------------" << endl
		<< "Help" << endl
		<< "\th : print this help" << endl
		<< "\tq : exit"
		<< endl;
}

/**
 * @brief openGLに描画要求を行う
 */
void setPostRedisplay(int skip)
{
	gSkip = skip;
	glutPostRedisplay();
}
void movePos( int dx_pic, int dy_pic );

/**
 * カメラ・オブジェクトの位置を初期化する
 */
void initPos()
{
	gSkip = 0;
	gIsFPView = true; // 表示が主観視点(First-Person point of View)モード
	gIsFPCam = false; // カメラが主観視点(First-Person point of View)モード

	gCamPos[0] = 0;
	gCamPos[1] = 0;
	gCamPos[2] = 30;

	gViewPos[0] = 0;
	gViewPos[1] = 0;
	gViewPos[2] = 0;

	gModelTrans[0] = 0;
	gModelTrans[1] = 0;
	gModelTrans[2] = 0;
	gModelRotate[0] = 0;
	gModelRotate[1] = 0;
	gModelRotate[2] = 0;
	gViewLength = 20; // 見る範囲

	movePos( -( gWindowSize[0] - gWindowSize[0] * 0.4 ) / 4, gWindowSize[1] / 4 );
}

void movePos( int dx_pic, int dy_pic )
{
	gModelTrans[1] -= dx_pic * gViewLength / gWindowSize[0] * gAspect * 2;
	gModelTrans[0] -= dy_pic * gViewLength / gWindowSize[1] * 2;
}


void eventDisplay()
{
	drawBegin();
	drawAll(gSkip + gSkipBase);
	drawEnd();
}

void pollingJoystcik(  )
{
	double rot = 10, move = gViewLength / 10.0;
	gJs.readNew(  );
	if( gJs.data.getButton( JS_BUTTON_R1 ) && gJs.data.getButton( JS_BUTTON_R2 ) )
	{
		gModelRotate[2] += rot * gJs.data.getAxis( JS_AXIS_STICK_RIGHT_X );

	}
	else
	{
		if( gJs.data.getButton( JS_BUTTON_R1 ) )
			gViewLength *= 1.1;
		if( gJs.data.getButton( JS_BUTTON_R2 ) )
			gViewLength /= 1.1;
		if( gJs.data.getButton( JS_BUTTON_R3 ) )
			initPos();
		gModelTrans[0] += -move * gJs.data.getAxis( JS_AXIS_STICK_RIGHT_Y );
		gModelTrans[1] += -move * gJs.data.getAxis( JS_AXIS_STICK_RIGHT_X );

	}
}

void eventTime(int value)
{
	// joystick処理
	pollingJoystcik(  );
	//描画
	setPostRedisplay(0);
	glutTimerFunc(300, eventTime, 0);
}

void eventReshape(int w, int h)
{
	gWindowSize[0] = w;
	gWindowSize[1] = h;
	gAspect = static_cast<double>( w ) / h;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	setPostRedisplay(5);
}

void eventKey(unsigned char key, int x, int y)
{
	switch(key)
	{
	case '0' :
	case '1' :
	case '2' :
	case '3' :
	case '4' :
	case '5' :
	case '6' :
	case '7' :
	case '8' :
	case '9' : {gIsDraw[key - '0']++;}break;
	case '+' : {gSkipBase--; if(gSkipBase < 0)gSkipBase = 0;}break;
	case '-' : {gSkipBase++;}break;
	case 'u' : {initPos();} break;
	case 'f' : { gIsFPView = !gIsFPView;} break;
	case 'c' : { gIsFPCam = !gIsFPCam;} break;
	case 'w' : { gIsDrawWorldMap = !gIsDrawWorldMap; } break;
	case 'm' : { gDrawMode++;}break;
	case 'b' : { glClearColor(0.0, 0.0, 0.0, 0.0); }break;
	case 'B' : { glClearColor(1.0, 1.0, 1.0, 1.0); }break;
	case 'V' : { captureInit( "out.avi" ,10 );}break;
	case 'h' :
	case 'H' :
		{
			gIsDrawHelp = !gIsDrawHelp;
//			movePos( gIsDrawHelp ? -200 : 200, 0 );
		}break;
//	case 'q' : // 安全のため、qでは終わらないようにした
	case 'Q' : { myExit(); }break;
	default :
	{
		if( usrEventKey )
			usrEventKey(key, x, y);
	}break;
	}
}

void eventSpecialKey(int key, int x, int y)
{
	int active;
	double rot = 10, move = gViewLength / 10.0;
	active = glutGetModifiers();

	if(!gIsFPCam)
	{// 俯瞰視点
		if(active & GLUT_ACTIVE_CTRL)
		{
			switch(key)
			{
			case GLUT_KEY_DOWN		: { gModelTrans[0] -= move;} break;
			case GLUT_KEY_UP		: { gModelTrans[0] += move;} break;
			case GLUT_KEY_LEFT		: { gModelTrans[1] += move;} break;
			case GLUT_KEY_RIGHT		: { gModelTrans[1] -= move;} break;
			case GLUT_KEY_PAGE_UP	: { gModelTrans[2] += move;} break;
			case GLUT_KEY_PAGE_DOWN	: { gModelTrans[2] -= move;} break;
			default : break;
			}
		}
		else
		{
			switch(key)
			{
			case GLUT_KEY_DOWN		: { gModelRotate[1] -= rot;} break;
			case GLUT_KEY_UP		: { gModelRotate[1] += rot;} break;
			case GLUT_KEY_LEFT		: { gModelRotate[2] -= rot;} break;
			case GLUT_KEY_RIGHT		: { gModelRotate[2] += rot;} break;
			case GLUT_KEY_PAGE_UP	: { gViewLength /= 1.1;} break;
			case GLUT_KEY_PAGE_DOWN	: { gViewLength *= 1.1;} break;
			default : break;
			}
		}
		setPostRedisplay(5);
	}
	else
	{// 主観視点
		//double d = sqrt(pow(camPos[0] - viewPos[0], 2) + pow(camPos[1] - viewPos[1], 2) + pow(camPos[2] - viewPos[2], 2));
		//double d = 0;
	}
}


void eventMouse(int button, int status, int x, int y)
{
	gMousePos[0] = x;
	gMousePos[1] = y;

	switch (button) {
	case GLUT_LEFT_BUTTON :
		gMouseButton[0] = (status == GLUT_DOWN);
		break;
	case GLUT_RIGHT_BUTTON :
		gMouseButton[1] = (status == GLUT_DOWN);
		break;
	case 3:
		gViewLength /= 1.1;
		setPostRedisplay(5);
		break;
	case 4:
		gViewLength *= 1.1;
		setPostRedisplay(5);
		break;
	}
}


void eventMotion(int x, int y)
{
	double dx = x - gMousePos[0];
	double dy = y - gMousePos[1];
	gMousePos[0] = x;
	gMousePos[1] = y;

	if(gMouseButton[0] && gMouseButton[1])
	{

	}
	else if(gMouseButton[0])
	{
		movePos( dx, dy );
//		gModelTrans[1] -= dx * gViewLength / gWindowSize[0] * gAspect * 2;
//		gModelTrans[0] -= dy * gViewLength / gWindowSize[1] * 2;
	}
	else if(gMouseButton[1])
	{
		gModelRotate[2] += dx;
		//gModelRotate[1] -= dy;
	}
	setPostRedisplay(10);
}


/**
 * @brief GLイベントのコールバック関数を設定
 */
void setEventCallback()
{
	glutDisplayFunc( eventDisplay );
	glutReshapeFunc( eventReshape );
	//glutIdleFunc( eventIdle );
	glutTimerFunc(100, eventTime, 0);
	glutKeyboardFunc( eventKey );
	glutSpecialFunc( eventSpecialKey );
	glutMouseFunc( eventMouse );
	glutMotionFunc( eventMotion );
	gJs.open(  );
}

/**
 * @brief GLイベントコールバック関数を取り消し
 */
void unsetEventCallback()
{
	//glutDisplayFunc( NULL );
	glutReshapeFunc( NULL );
	//glutIdleFunc( NULL );
	glutKeyboardFunc( NULL );
	glutSpecialFunc( NULL );
	glutMouseFunc( NULL );
	glutMotionFunc( NULL );
}

/**
 * @brief GL描画周りの初期化
 */
void initGL(int *aArgc, char **aArgv)
{
	/*OpenGL初期化*/
	glutInitWindowSize(gWindowSize[0], gWindowSize[1]);
	glutInitWindowPosition(0, 0);

	glutInit(aArgc, aArgv);
	glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA );
	glutCreateWindow(aArgv[0]);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glLineWidth(4.0);
	glPointSize(1.0);
	//glEnable(GL_LINE_SMOOTH); // アンチエイリアシング
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	initPos();
}


/**
 * @brief exit関数
 */
void myExit()
{
	unsetEventCallback();
	endSSM();
	cerr << "exit." << endl;
	exit(0);
}
