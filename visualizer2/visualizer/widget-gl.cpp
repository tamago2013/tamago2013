#include <GL/glu.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include "window.hpp"
#include "widget-gl.hpp"
#include "widget-msg.hpp"
#include "menu-handler.hpp"
#include "ssm-message.hpp"
#include "tkg-opengl.hpp"
#include "gnd-bmp.hpp"
#include "gnd-opsm.hpp"

#include <cstdlib>
using namespace ssm;

WidgetGL::WidgetGL(Window *parent, tkg::ConfigFile &conf) : QGLWidget()
{
    setFocusPolicy(Qt::StrongFocus);

    window = parent;

    robot_p = 0.0;
    robot_t = 0.0;

    ssm_robot    = new SSMApi<Spur_Odometry> (tkg::parseStr(conf["Odometry"]["ssm-name"]), tkg::parseInt(conf["Odometry"]["ssm-id"]));
    ssm_particle = new SSMParticles          (tkg::parseStr(conf["Particle"]["ssm-name"]), tkg::parseInt(conf["Particle"]["ssm-id"]));

    for(int i=0; i<SSM_LASER_SIZE; i++)
    {
        std::string group = tkg::strf("Urg%d", i+1);
        ssm_laser[i] = new SSMSOKUIKIData3D(tkg::parseStr(conf[group]["ssm-name"]), tkg::parseInt(conf[group]["ssm-id"]));

        color_point [i]  = tkg::Color4(conf[group]["point-color"]);
        color_laser [i]  = tkg::Color4(conf[group]["laser-color"]);
        //laser_view  [i] |= (conf[group]["view-point"]=="true" ? 1 : 0);
        //laser_view  [i] |= (conf[group]["view-laser"]=="true" ? 2 : 0);

        vmh_laser[i] = new ViewMenuHandler(this);
        vmh_laser[i]->title = conf[group]["title"];
        vmh_laser[i]->list.push_back( MenuElement("non-display", 0, false) );
        vmh_laser[i]->list.push_back( MenuElement("point"      , 1, false) );
        vmh_laser[i]->list.push_back( MenuElement("laser"      , 2, false) );
        vmh_laser[i]->list.push_back( MenuElement("point+laser", 3, true ) );

        window->addMenuView(vmh_laser[i]);
    }

    fps_timer = new FpsMenuHandler(this);
    fps_timer->title = conf["Viewer"]["title"];
    std::vector<std::string> fps = tkg::parseArray(conf["Viewer"]["fps"]);
    for(int i=0; i<fps.size(); i++)
    {
        fps_timer->list.push_back( MenuElement(fps[i]+" fps", tkg::parseInt(fps[i]), i==0) );
    }
    connect(fps_timer->timer, SIGNAL(timeout()), this, SLOT(update()));
    window->addMenuFps(fps_timer);


    camera = new Camera;

    route_name = conf["File"]["route"];

    map_name   = conf["File"]["opsm-map"];
    map_width  = -1;
    map_height = -1;
    map_data   = NULL;


}

WidgetGL::~WidgetGL()
{
    delete camera;
    delete map_data;

    delete ssm_robot;
    delete ssm_laser[0];
    delete ssm_laser[1];
    delete ssm_particle;
}

bool WidgetGL::init()
{
    loadRoute();
    loadMap();
    smConnect(ssm_robot);
    smConnect(ssm_laser[0]);
    smConnect(ssm_laser[1]);
    smConnect(ssm_particle);

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

    camera->setpos(robot_p.x, robot_p.y,robot_t);
    camera->update();

    // モデルの描画
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    updateStream();

    drawMap();
    drawGround();
    drawRoute();
    drawRobot();
    drawParticles();
    for(int i=0; i<SSM_LASER_SIZE; i++)
    {
        drawLaser(i);
    }

    glFlush();
}

void WidgetGL::updateStream()
{
    ssmTimeT ssm_time = 1e10;
    std::vector<SSMApiBase*> ssmapi;
    ssmapi.push_back(ssm_robot);
    ssmapi.push_back(ssm_laser[0]);
    ssmapi.push_back(ssm_laser[1]);
    ssmapi.push_back(ssm_particle);
    for(uint i=0; i<ssmapi.size(); i++)
    {
        if( smState(ssmapi[i]) )
        {
            smReadLast(ssmapi[i]);
            ssm_time = std::min(ssm_time, ssmapi[i]->time);
        }    glArrow(robot_p, robot_t, 0.5);
    }
    for(uint i=0; i<ssmapi.size(); i++)
    {
        smReadTime(ssmapi[i], ssm_time);
    }

    if(robot_log_p.empty() || (robot_log_p.back() - robot_p).abs() > 3.0)
    {
        robot_log_p.push_back(robot_p);
        robot_log_t.push_back(robot_t);
    }
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
    if( gnd::opsm::read_counting_map(&cnt_map, dirname) < 0)
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
        route_node.push_back( tkg::Point3(x,y,0) );
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
        tkg::glVertex(route_node[i]);
    }
    glEnd();
    glPointSize(1);

    //glColor4dv(color_point[id].rgba);
    glColor3d(1.0, 1.0, 0.0);
    glLineWidth(1);
    glBegin(GL_LINES);
    for(uint i=0; i<route_edge.size(); i++)
    {
        tkg::glVertex(route_node[route_edge[i].first ]);
        tkg::glVertex(route_node[route_edge[i].second]);
    }
    glEnd();

    /*
    glColor3d(1.0, 1.0, 1.0);
    for(int i=0; i<route.size(); i++)
    {
        myString(route[i]+tkg::Point3(0,0,0.2), strprintf("%d",i).c_str());
    }
    */
}

void WidgetGL::drawGround()
{
    if(map_width != -1 && map_height != -1)
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
    else
    {
        double s = -30.0, g = 30.0;

        glColor3d(0.2,0.2,0.2);
        glBegin(GL_LINES);
        for(double d=s; d<=g; d+=1.0)
        {
            tkg::Point3 sx(d, s, 0.0);
            tkg::Point3 gx(d, g, 0.0);
            tkg::Point3 sy(s, d, 0.0);
            tkg::Point3 gy(g, d, 0.0);

            glVertex3dv((robot_p + sx.rotateZ(robot_t)).vec);
            glVertex3dv((robot_p + gx.rotateZ(robot_t)).vec);
            glVertex3dv((robot_p + sy.rotateZ(robot_t)).vec);
            glVertex3dv((robot_p + gy.rotateZ(robot_t)).vec);
        }
        glEnd();
    }
}

void WidgetGL::drawRobot()
{
    SSMApi<Spur_Odometry> *ssmapi = ssm_robot;

    if(!smState(ssmapi)) return;
    Spur_Odometry &data = ssmapi->data;

    robot_p.x = data.x;
    robot_p.y = data.y;
    robot_t   = data.theta;

    glColor3d(1.0, 0.5, 0);
    glArrow(robot_p, robot_t, 0.5);

    glColor4d(1.0, 0.5, 0, 0.5);
    for(uint i=0; i<robot_log_p.size(); i++)
    {
        glArrow(robot_log_p[i], robot_log_t[i], 0.5);
    }
}

void WidgetGL::drawParticles()
{
    SSMParticles *ssmapi = ssm_particle;

    if( !smState(ssmapi) ) return;
    particle_set_c &data = ssmapi->data;

    for(int i=0; i<data.size(); i++)
    {
        glColor3d(1.0, 0.0, 0.0);
        tkg::Point3 pos(data[i][0][PARTICLE_X], data[i][0][PARTICLE_Y], 0.0 );
        glArrow(pos, data[i][0][PARTICLE_THETA], 0.2);
    }
}

void WidgetGL::drawLaser(int id)
{
    SSMSOKUIKIData3D *ssmapi = ssm_laser[id];

    if( !smState(ssmapi) ) return;
    SOKUIKIData3D &data = ssmapi->data;

    if(vmh_laser[id]->value & 1)
    {
        glColor4dv(color_point[id].rgba);
        glPointSize(3);
        glBegin(GL_POINTS);

        for(uint i=0; i<data.numPoints(); i++)
        {
            if(data[i].isWarning()) continue;

            tkg::Point3 ref(data[i].reflect.vec);
            glVertex(robot_p + ref.rotateZ(robot_t));
        }
        glEnd();
        glPointSize(1);
    }

    if(vmh_laser[id]->value & 2)
    {
        glColor4dv(color_laser[id].rgba);
        glLineWidth(1);
        glBegin(GL_LINES);
        for(uint i=0; i<data.numPoints(); i++)
        {
            if(data[i].isWarning()) continue;

            tkg::Point3 ori(data[i].origin.vec);
            tkg::Point3 ref(data[i].reflect.vec);
            glVertex(robot_p + ori);
            glVertex(robot_p + ref.rotateZ(robot_t));
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

    if(event->key() == Qt::Key_R) camera->rotv = 0;

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

