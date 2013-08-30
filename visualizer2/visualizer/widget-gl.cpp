#include <GL/glu.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include "window.hpp"
#include "widget-gl.hpp"
#include "widget-msg.hpp"
#include "fps-timer.hpp"
#include "ssm-message.hpp"
#include "tkg-opengl.hpp"
#include "tkg-geometry-gl.hpp"
#include "gnd-bmp.hpp"
#include "gnd-opsm.hpp"
using namespace ssm;

WidgetGL::WidgetGL(Window *parent, tkg::ConfigFile &conf) : QGLWidget()
{
    printf("GL constructor\n");
    setFocusPolicy(Qt::StrongFocus);

    window = parent;

    robot_x = 0;
    robot_y = 0;
    robot_t = 0;

    ssm_robot    = new SSMApi<Spur_Odometry> (tkg::parseStr(conf["Odom"]["ssm-name"]), tkg::parseInt(conf["Odom"]["ssm-id"]));
    ssm_laser[0] = new SSMSOKUIKIData3D      (tkg::parseStr(conf["Urg1"]["ssm-name"]), tkg::parseInt(conf["Urg1"]["ssm-id"]));
    ssm_laser[1] = new SSMSOKUIKIData3D      (tkg::parseStr(conf["Urg2"]["ssm-name"]), tkg::parseInt(conf["Urg2"]["ssm-id"]));

    color_point[0] = tkg::parseColor(conf["Urg1"]["point-color"]);
    color_laser[0] = tkg::parseColor(conf["Urg1"]["laser-color"]);
    color_point[1] = tkg::parseColor(conf["Urg2"]["point-color"]);
    color_laser[1] = tkg::parseColor(conf["Urg2"]["laser-color"]);

    camera = new Camera;

    route_name = conf["File"]["route"];

    map_name   = conf["File"]["opsm-map"];
    map_width  = -1;
    map_height = -1;
    map_data   = NULL;

    timer = new FPSTimer(conf["Viewer"]["fps"]);
    window->addMenuFPS(timer, tkg::parseStr(conf["Viewer"]["title"]));
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

}

WidgetGL::~WidgetGL()
{
    delete camera;
    delete map_data;

    printf("GL destructor\n");
}

bool WidgetGL::init()
{
    loadRoute();
    loadMap();
    smConnect(ssm_robot);
    smConnect(ssm_laser[0]);
    smConnect(ssm_laser[1]);

    return true;
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
    glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    */

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, width, height);

    // カメラの設定
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 500);

    camera->setpos(robot_x, robot_y,robot_t);
    camera->update();

    // モデルの描画
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 最新時刻の取得
    ssm_time = 1e10;
    if( smState(ssm_robot) )
    {
        smReadLast(ssm_robot);
        ssm_time = std::min(ssm_time, ssm_robot->time);

    }
    for(int i=0; i<SSM_LASER_SIZE; i++)
    {
        if( smState(ssm_laser[i]) )
        {
            smReadLast(ssm_laser[i]);
            ssm_time = std::min(ssm_time, ssm_laser[i]->time);
        }
    }

    drawMap();
    drawGround();
    drawRoute();
    drawRobot();
    drawLaser(0);
    drawLaser(1);

    glFlush();
}

bool WidgetGL::loadMap()
{
    const char *dirname = map_name.c_str();
    if( dirname == NULL) return false;
    if(*dirname == '\0') return false;

    gnd::opsm::map_t  opsm_map;
    gnd::opsm::cmap_t cnt_map;
    gnd::bmp8_t       bmp_map;

    // read  map raw map_data
    if( gnd::opsm::read_counting_map(&cnt_map, dirname, "psm") < 0)
    {
        window->message()->add_message("マップの読込に失敗しました。[");
        window->message()->add_message(dirname);
        window->message()->add_message("]\n");
        return false;
    }

    if( gnd::opsm::build_map(&opsm_map, &cnt_map, gnd_mm2dist(1)) < 0 ) {
        window->message()->add_message("マップの作成に失敗しました。\n");
        return false;
    }

    if( gnd::opsm::build_bmp(&bmp_map, &opsm_map, gnd_m2dist(1.0/10)) < 0 )
    {
        window->message()->add_message("マップの画像化に失敗しました。\n");
        return false;
    }
    window->message()->add_message("マップの読込に成功しました。\n");

    map_width  = bmp_map.column();
    map_height = bmp_map.row();
    map_base_x = bmp_map.xorg();
    map_base_y = bmp_map.yorg();
    map_unit_x = bmp_map.xrsl();
    map_unit_y = bmp_map.yrsl();

    map_data = new unsigned char[map_width*map_height*3];

    for(int y=0; y<map_height; y++)
    for(int x=0; x<map_width;  x++)
    {
        map_data[(y*map_width+x)*3+0] = bmp_map.value(y, x);
        map_data[(y*map_width+x)*3+1] = bmp_map.value(y, x);
        map_data[(y*map_width+x)*3+2] = bmp_map.value(y, x);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &map_image);
    glBindTexture(GL_TEXTURE_2D, map_image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, map_width, map_height, GL_RGB, GL_UNSIGNED_BYTE, map_data);

    return true;
}

void WidgetGL::drawMap()
{
    glColor3d(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, map_image);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_POLYGON);
    glTexCoord2d(0, 0); glVertex3d(map_base_x,                        map_base_y,                         0.0);
    glTexCoord2d(1, 0); glVertex3d(map_base_x + map_width*map_unit_x, map_base_y,                         0.0);
    glTexCoord2d(1, 1); glVertex3d(map_base_x + map_width*map_unit_x, map_base_y + map_height*map_unit_y, 0.0);
    glTexCoord2d(0, 1); glVertex3d(map_base_x,                        map_base_y + map_height*map_unit_y, 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

bool WidgetGL::loadRoute()
{
    std::ifstream fin(route_name.c_str());
    if(!fin)
    {
        window->message()->add_message("ルートの読込に失敗しました。[");
        window->message()->add_message(route_name.c_str());
        window->message()->add_message("]\n");
        return false;
    }

    while(!fin.eof())
    {
        char c; double x,y;
        fin >> c >> x >> y;
        route_node.push_back( tkg::point3(x,y,0) );
    }

    for(uint i=1; i<route_node.size(); i++)
    {
        route_edge.push_back( std::make_pair(i-1, i) );
    }

    window->message()->add_message("ルートの読込に成功しました。\n");
    return true;
}

void WidgetGL::drawRoute()
{
    //glColor4dv(color_point[id].rgba);
    glColor3d(1.0, 1.0, 0.0);
    glPointSize(5);
    glBegin(GL_POINTS);
    for(uint i=0; i<route_node.size(); i++)
    {
        tkg::ggVertex(route_node[i]);
    }
    glEnd();
    glPointSize(1);

    //glColor4dv(color_point[id].rgba);
    glColor3d(1.0, 1.0, 0.0);
    glLineWidth(1);
    glBegin(GL_LINES);
    for(uint i=0; i<route_edge.size(); i++)
    {
        tkg::ggVertex(route_node[route_edge[i].first ]);
        tkg::ggVertex(route_node[route_edge[i].second]);
    }
    glEnd();

    /*
    glColor3d(1.0, 1.0, 1.0);
    for(int i=0; i<route.size(); i++)
    {
        myString(route[i]+tkg::point3(0,0,0.2), strprintf("%d",i).c_str());
    }
    */
}

void WidgetGL::drawGround()
{
    double sx = floor(map_base_x), gx = ceil(map_base_x + map_width  * map_unit_x);
    double sy = floor(map_base_y), gy = ceil(map_base_y + map_height * map_unit_y);

    glColor3d(0.2,0.2,0.2);
    glBegin(GL_LINES);
    for(double x=sx; x<=gx; x+=1.0)
    {
        glVertex3d(x, sy, 0.0);
        glVertex3d(x, gy, 0.0);
    }
    for(double y=sy; y<=gy; y+=1.0)
    {
        glVertex3d(sx, y, 0.0);
        glVertex3d(gx, y, 0.0);
    }
    glEnd();
}

void WidgetGL::drawRobot()
{
    SSMApi<Spur_Odometry> *ssmapi = ssm_robot;

    smReadTime(ssmapi, ssm_time);
    if(!ssmapi->isOpen()) return;
    Spur_Odometry &data = ssmapi->data;

    robot_x = data.x;
    robot_y = data.y;
    robot_t = data.theta;
    tkg::point3 robot_p = tkg::point3(robot_x, robot_y, 0);

    glColor3d(1,0,0);
    glLineWidth(3);
    ggDrawCross(robot_p, robot_t, 1);
    glLineWidth(1);
}

void WidgetGL::drawLaser(int id)
{
    SSMSOKUIKIData3D *ssmapi = ssm_laser[id];

    smReadTime(ssmapi, ssm_time);
    if( !smState(ssmapi) ) return;
    SOKUIKIData3D &data = ssmapi->data;

    tkg::point3 robot_p = tkg::point3(robot_x, robot_y, 0);

    //if(laser[s]->view_state & 1)
    {
        glColor4dv(color_point[id].rgba);
        glPointSize(3);
        glBegin(GL_POINTS);

        for(uint i=0; i<data.numPoints(); i++)
        {
            if(data[i].isWarning()) continue;

            tkg::point3 ref(data[i].reflect.vec);
            ref.rotZ(robot_t);
            ggVertex(robot_p + ref);
        }
        glEnd();
        glPointSize(1);
    }

    //if(laser[s]->view_state & 2)
    {
        glColor4dv(color_laser[id].rgba);
        glLineWidth(1);
        glBegin(GL_LINES);
        for(uint i=0; i<data.numPoints(); i++)
        {
            if(data[i].isWarning()) continue;

            tkg::point3 ori(data[i].origin.vec);
            tkg::point3 ref(data[i].reflect.vec);
            ori.rotZ(robot_t);
            ref.rotZ(robot_t);
            ggVertex(robot_p + ori);
            ggVertex(robot_p + ref);
        }
        glEnd();
    }
}

void WidgetGL::keyPressEvent(QKeyEvent *event)
{
    double dx = 0;
    double dy = 0;
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

