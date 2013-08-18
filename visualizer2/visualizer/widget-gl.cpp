#include <GL/glu.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include "window.hpp"
#include "widget-gl.hpp"
#include "widget-msg.hpp"
#include "viewer-map.hpp"
#include "fps-timer.hpp"
#include "ssm-message.hpp"
#include "ssm-laser.hpp"
#include "ssmtype/spur-odometry.h"
#include "tkg-utility.hpp"
#include "tkg-geometry-gl.hpp"
using namespace tkg;
using namespace ssm;

#include <fstream>
using namespace std;


enum ssm_string { robot, laser1, laser2, ssm_count };

WidgetGL::WidgetGL(Window *parent, tkg::ConfigFile &conf) : QGLWidget()
{
    printf("GL constructor\n");
    setFocusPolicy(Qt::StrongFocus);

    window = parent;

    robot_x = 0;
    robot_y = 0;
    robot_t = 0;

    ssm[robot ] = new SSMApi<Spur_Odometry> (tkg::parseStr(conf["Odom"]["ssm-name"]), tkg::parseInt(conf["Odom"]["ssm-id"]));
    ssm[laser1] = new SSMSOKUIKIData3D      (tkg::parseStr(conf["Urg1"]["ssm-name"]), tkg::parseInt(conf["Urg1"]["ssm-id"]));
    ssm[laser2] = new SSMSOKUIKIData3D      (tkg::parseStr(conf["Urg2"]["ssm-name"]), tkg::parseInt(conf["Urg2"]["ssm-id"]));

    camera = new Camera;

    vmap = new ViewerMap;
    //mapviewer.read_cmap("/home/ena8781/roboken/map/tc2013");

    timer = new FPSTimer(conf["Viewer"]["fps"]);
    window->addMenuFPS(timer, tkg::parseStr(conf["Viewer"]["title"]));
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

}

WidgetGL::~WidgetGL()
{

    delete camera;

    delete vmap;
    printf("GL destructor\n");
}

bool WidgetGL::init()
{
    for(int i=0; i<SSM_COUNT; i++)
    {
        smConnect(ssm[i]);
    }

    char path[256];
    ifstream fin("visualizer.conf");
    if(fin) fin >> path;
    //vmap->read_cmap(path);
}

void WidgetGL::initializeGL()
{
    // 背景色の指定
    glClearColor(0, 0, 0, 0);
}

void WidgetGL::resizeGL(int w, int h)
{
    width  = w;
    height = h;
    aspect = (double)w/h;
}

void WidgetGL::paintGL()
{

    /*
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    */
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, width, height);

/*
    double cx,cy,th;
    if(vssm->test_get_pos(&cx, &cy, &th))
    {
        camera->setpos(cx,cy,th);
    }
*/

    // カメラの設定
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 500);
    camera->update();

    // モデルの描画
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 各種データの描画
    vmap->draw();

    for(int i=0; i<SSM_COUNT; i++)
    {
        smReadNew(ssm[i]);
    }
    drawGround();
    drawRobot(robot);
    drawLaser(laser1);
    drawLaser(laser2);

    glFlush();
}

void WidgetGL::drawGround()
{
    const int gsize = 30;
    double gr_x = 0; //glpos.data.x;
    double gr_y = 0; //glpos.data.y;
    /*
    glVertex3d(gr_x+gsize, gr_y+gsize, 0.0);
    glVertex3d(gr_x-gsize, gr_y+gsize, 0.0);
    glVertex3d(gr_x-gsize, gr_y-gsize, 0.0);
    glVertex3d(gr_x+gsize, gr_y-gsize, 0.0);
    */
    glColor3d(0.2,0.2,0.2);
    glBegin(GL_LINES);
    for(int i=-gsize; i<=gsize; i++)
    {
        glVertex3d(gr_x+i*1.0, gr_y-gsize, 0.0);
        glVertex3d(gr_x+i*1.0, gr_y+gsize, 0.0);
        glVertex3d(gr_x-gsize, gr_y+i*1.0, 0.0);
        glVertex3d(gr_x+gsize, gr_y+i*1.0, 0.0);
    }
    glEnd();
}

void WidgetGL::drawRobot(int id)
{
    if(!ssm[id]->isOpen()) return;
    Spur_Odometry &data = (*(Spur_Odometry*)(ssm[id]->data()));

    robot_x = data.x;
    robot_y = data.y;
    robot_t = data.theta;
    point3 robot_p = point3(robot_x, robot_y, 0);

    glColor3d(1,0,0);
    glLineWidth(3);
    ggDrawCross(robot_p, robot_t, 1);
    glLineWidth(1);
}

void WidgetGL::drawLaser(int id)
{
    if(!ssm[id]->isOpen()) return;
    SOKUIKIData3D &data = (*(SOKUIKIData3D*)(ssm[id]->data()));

    point3 robot_p = point3(robot_x, robot_y, 0);

    //if(laser[s]->view_state & 1)
    {
        glColor3d(0,1,0);
        //if(s) glColor3d(0,1,0); else glColor3d(0,1,1);
        glPointSize(3);
        glBegin(GL_POINTS);

        for(int i=0; i<data.numPoints(); i++)
        {
            if(data[i].isWarning()) continue;

            point3 ref(data[i].reflect.vec);
            ref.rotZ(robot_t);
            ggVertex(robot_p + ref);
        }
        glEnd();
        glPointSize(1);
    }

    //if(laser[s]->view_state & 2)
    {
        //if(s) glColor3d(0,0.5,0); else glColor3d(0,0.5,0.5);
        glLineWidth(1);
        glBegin(GL_LINES);
        for(int i=0; i<data.numPoints(); i++)
        {
            if(data[i].isWarning()) continue;

            point3 ori(data[i].origin.vec);
            point3 ref(data[i].reflect.vec);
            ori.rotZ(robot_t);
            ref.rotZ(robot_t);
            ggVertex(robot_p + ori);
            ggVertex(robot_p + ref);
        }
        glEnd();
        glPointSize(1);
    }
}

void WidgetGL::keyPressEvent(QKeyEvent *event)
{
    double dx = 0;
    double dy = 0;
    printf("%d\n", event->key());
    if(event->key() == Qt::Key_Up   ) dy -= 0.1;
    if(event->key() == Qt::Key_Down ) dy += 0.1;
    if(event->key() == Qt::Key_Left ) dx -= 0.1;
    if(event->key() == Qt::Key_Right) dx += 0.1;



    if(event->modifiers() & Qt::ControlModifier)
    {
        camera->scale(dy*10);
    }
    else if(event->modifiers() & Qt::ShiftModifier)
    {
        camera->rotate(dx,dy);
    }
    else // (NoModifier)
    {
        camera->translate(dx,dy);
    }
}

void WidgetGL::keyReleaseEvent(QKeyEvent *event)
{

}

void WidgetGL::wheelEvent(QWheelEvent *event)
{
    double dd = (double) event->delta() / 120;
    camera->scale(-dd);
}

void WidgetGL::mouseMoveEvent(QMouseEvent *event)
{
    double dx = (double) (mouse_prev_x - event->x()) / width;
    double dy = (double) (mouse_prev_y - event->y()) / height;

    if(mouse_prev_b == Qt::LeftButton)
    {
        camera->translate(dx,dy);
    }

    if(mouse_prev_b == Qt::RightButton)
    {
        camera->rotate(dx,dy);
    }


    mouse_prev_x = event->x();
    mouse_prev_y = event->y();
}

void WidgetGL::mousePressEvent(QMouseEvent *event)
{
    mouse_prev_x = event->x();
    mouse_prev_y = event->y();
    mouse_prev_b = event->button();
}

void WidgetGL::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_prev_x = event->x();
    mouse_prev_y = event->y();
    mouse_prev_b = event->button();
}

