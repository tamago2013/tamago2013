/*
 * GLcommon.hpp
 *
 *  Created on: 2010/07/25
 *      Author: shikina
 */

#ifndef GLCOMMON_HPP_
#define GLCOMMON_HPP_
#include <GL/glut.h>

void drawBegin();
void drawEnd();
int drawString(const char *str, double posx, double posy, double posz);
int drawStringDisplay(const char *str, double  posx, double posy);
void drawHelp();
void eventDisplay();
void eventTime(int value);
void eventReshape(int w, int h);
void eventKey(unsigned char key, int x, int y);
void eventMouse(int button, int status, int x, int y);
void eventMotion(int x, int y);
void eventSpecialKey(int key, int x, int y);
void setEventCallback();
void unsetEventCallback();
void initGL(int *aArgc, char **aArgv);
void drawWindow();
void myExit();

double getViewSize();

extern void drawAll(int mode); // GLcommonはこれを通して画面を出力する
extern bool gIsFPView; // 主観視点(First-Person point of View)モード
extern int gIsDraw[10];// 表示するかどうかのスイッチ
extern int gDrawMode; // 描画モード key m を押すと１つずつ増加する
extern bool gIsDrawHelp;
extern bool gIsDrawWorldMap;
extern void ( *usrEventKey)(unsigned char key, int x, int y);

extern GLsizei gWindowSize[2]; // window size
#endif /* GLCOMMON_HPP_ */
