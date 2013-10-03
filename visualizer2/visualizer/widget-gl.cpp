#include <GL/glu.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include "window.hpp"
#include "widget-gl.hpp"
#include "widget-msg.hpp"
#include "menu-handler.hpp"
#include "tkg-opengl.hpp"
#include "map-viewer.hpp"

#include "ssm-message.hpp"
#include "tkg-debug.hpp"

WidgetGL::WidgetGL(Window *parent, tkg::ConfigFile &conf) : QGLWidget()
{
    tkg::debug("new WidgetGL\n");

    setFocusPolicy(Qt::StrongFocus);

    window = parent;

    position.sync = (conf["Position"]["synchronize"]=="true");
    position.ssm  = new SSMApi<Spur_Odometry> (tkg::parseStr(conf["Position"]["ssm-name"]), tkg::parseInt(conf["Position"]["ssm-id"]));

    for(int i=1; true; i++)
    {
        std::string group = tkg::strf("Laser%d", i);
        if(conf.find(group) == conf.end()) { break; }


        LaserStream laser;

        laser.sync        = (conf[group]["synchronize"]=="true");
        laser.point_color = tkg::Color4(conf[group]["point-color"]);
        laser.laser_color = tkg::Color4(conf[group]["laser-color"]);

        laser.ssm  = new SSMSOKUIKIData3D(tkg::parseStr(conf[group]["ssm-name"]), tkg::parseInt(conf[group]["ssm-id"]));
        laser.menu = new SelectMenuHandler(this);

        laser.menu->title = conf[group]["title"];
        laser.menu->value  = 0;
        laser.menu->value |= (conf[group]["view-point"]=="true" ? 1 : 0);
        laser.menu->value |= (conf[group]["view-laser"]=="true" ? 2 : 0);

        laser.menu->list.push_back( SelectMenuElement("non-display", 0) );
        laser.menu->list.push_back( SelectMenuElement("point"      , 1) );
        laser.menu->list.push_back( SelectMenuElement("laser"      , 2) );
        laser.menu->list.push_back( SelectMenuElement("point+laser", 3) );

        window->addMenuView(laser.menu);

        lasers.push_back(laser);
    }


    std::vector<std::string> fps = tkg::parseArray(conf["Viewer"]["fps"]);
    fps_timer = new FpsMenuHandler(this);
    fps_timer->title = conf["Viewer"]["title"];
    fps_timer->value = fps.empty() ? 1 : tkg::parseInt(fps.front());
    for(uint i=0; i<fps.size(); i++)
    {
        fps_timer->list.push_back( SelectMenuElement(fps[i]+" fps", tkg::parseInt(fps[i])) );
    }
    connect(fps_timer->timer, SIGNAL(timeout()), this, SLOT(update()));
    window->addMenuFps(fps_timer);

    particle.sync = (conf["Particle"]["synchronize"]=="true");
    particle.ssm  = new SSMParticles(tkg::parseStr(conf["Particle"]["ssm-name"]), tkg::parseInt(conf["Particle"]["ssm-id"]));
    particle.menu = new ToggleMenuHandler(this);
    particle.menu->title = conf["Particle"]["title"];
    particle.menu->value = true;
    window->addMenuView(particle.menu);

    ptzcamera.sync = (conf["PTZCamera"]["synchronize"]=="true");
    ptzcamera.ssm  = new SSMApi<ysd::PTZ>(tkg::parseStr(conf["PTZCamera"]["ssm-name"]), tkg::parseInt(conf["PTZCamera"]["ssm-id"]));


    stream.push_back(&position);
    stream.push_back(&particle);
    stream.push_back(&ptzcamera);
    for(uint i=0; i<lasers.size(); i++)
    {
        stream.push_back(&lasers[i]);
    }

    camera = new Camera;

    field.opsm = new MapViewer(conf["Field"]["file"]);
    field.menu = new ToggleMenuHandler(this);
    field.menu->title = conf["Field"]["title"];
    field.menu->value = (conf["Field"]["view-state"] == "true");
    window->addMenuView(field.menu);


    route.file = conf["Route"]["file"];
    route.menu = new ToggleMenuHandler(this);
    route.menu->title =  conf["Route"]["title"];
    route.menu->value = (conf["Route"]["view-state"] == "true");
    window->addMenuView(route.menu);
}

WidgetGL::~WidgetGL()
{
    tkg::debug("delete WidgetGL\n");

    delete camera;

    delete field.opsm;
    delete field.menu;

    delete route.menu;


    //各クラスのデストラクタに移動
    delete position.ssm;

    delete particle.ssm;
    delete particle.menu;

    delete ptzcamera.ssm;

    for(uint i=0; i<lasers.size(); i++)
    {
        delete lasers[i].ssm;
        delete lasers[i].menu;
    }
}

bool WidgetGL::init()
{
    loadRoute();
    loadMap();

    for(uint i=0; i<stream.size(); i++)
    {
        smConnect( stream[i]->ssm );
    }

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

    camera->setpos(position.robot.pos.x, position.robot.pos.y, position.robot.ang);
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
    for(uint i=0; i<lasers.size(); i++)
    {
        drawLaser(i);
    }
    drawPTZ();

    //tkg::glString("");

    glFlush();
}

void WidgetGL::updateStream()
{
    ssmTimeT ssm_time = 1e10;
    for(uint i=0; i<stream.size(); i++)
    {
        if( !stream[i]->sync ) continue;
        if( !smState(stream[i]->ssm) ) continue;

        smReadLast(stream[i]->ssm);
        ssm_time = std::min(ssm_time, stream[i]->ssm->time);
    }

    for(uint i=0; i<stream.size(); i++)
    {
        if( !stream[i]->sync )
        {
            if(!stream[i]->ssm->isUpdate()) continue;

            smReadLast(stream[i]->ssm);
            if(stream[i]->ssm != position.ssm)
            {
                stream[i]->robot = position.robot;
            }
        }
        else
        {
            smReadTime(stream[i]->ssm, ssm_time);
            if(stream[i]->ssm != position.ssm)
            {
                stream[i]->robot = position.robot;
            }
        }
    }



    if(robot_log.empty() || (robot_log.back().pos - position.robot.pos).abs() > 3.0)
    {
        robot_log.push_back(position.robot);
    }
}

bool WidgetGL::loadMap()
{
    field.opsm->load();
    return true;
}

void WidgetGL::drawMap()
{
    if(field.menu->value) { field.opsm->draw(); }
}

bool WidgetGL::loadRoute()
{
    std::ifstream fin(route.file.c_str());
    if(!fin)
    {
        window->message()->add_message("ルートの読込に失敗しました。[");
        window->message()->add_message(route.file.c_str());
        window->message()->add_message("]\n");
        return false;
    }

    while(!fin.eof())
    {
        char c; double x,y;
        fin >> c >> x >> y;
        route.node.push_back( tkg::Point3(x,y,0) );
    }

    for(uint i=1; i<route.node.size(); i++)
    {
        route.edge.push_back( std::make_pair(i-1, i) );
    }

    window->message()->add_message("ルートの読込に成功しました。\n");
    return true;
}

void WidgetGL::drawRoute()
{
    if( !route.menu->value ) return;

    //glColor4dv(color_point[id].rgba);
    glColor3d(1.0, 1.0, 0.0);
    glPointSize(5);
    glBegin(GL_POINTS);
    for(uint i=0; i<route.node.size(); i++)
    {
        tkg::glVertex(route.node[i]);
    }
    glEnd();
    glPointSize(1);

    //glColor4dv(color_point[id].rgba);
    glColor3d(1.0, 1.0, 0.0);
    glLineWidth(1);
    glBegin(GL_LINES);
    for(uint i=0; i<route.edge.size(); i++)
    {
        tkg::glVertex(route.node[route.edge[i].first ]);
        tkg::glVertex(route.node[route.edge[i].second]);
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
    if( !field.opsm->good() || !field.menu->value )
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

            glVertex3dv((position.robot.pos + sx.rotateZ(position.robot.ang)).vec);
            glVertex3dv((position.robot.pos + gx.rotateZ(position.robot.ang)).vec);
            glVertex3dv((position.robot.pos + sy.rotateZ(position.robot.ang)).vec);
            glVertex3dv((position.robot.pos + gy.rotateZ(position.robot.ang)).vec);
        }
        glEnd();
    }
}

void WidgetGL::drawRobot()
{
    SSMApi<Spur_Odometry> *ssmapi = (SSMApi<Spur_Odometry>*)position.ssm;

    if(!smState(ssmapi)) return;
    Spur_Odometry &data = ssmapi->data;

    // exec (not draw)
    position.robot = Robot( tkg::Point3(data.x, data.y), data.theta );

    glColor3d(1.0, 0.5, 0);
    glArrow(position.robot.pos, position.robot.ang, 0.5);

    glColor4d(1.0, 0.5, 0, 0.5);
    for(uint i=0; i<robot_log.size(); i++)
    {
        glArrow(robot_log[i].pos, robot_log[i].ang, 0.5);
    }
}

void WidgetGL::drawParticles()
{
    if( !particle.menu->value ) return;

    SSMParticles *ssmapi = (SSMParticles*)particle.ssm;

    if( !smState(ssmapi) ) return;
    particle_set_c &data = ssmapi->data;

    for(uint i=0; i<data.size(); i++)
    {
        glColor3d(1.0, 0.0, 0.0);
        tkg::Point3 pos(data[i][0][PARTICLE_X], data[i][0][PARTICLE_Y], 0.0 );
        glArrow(pos, data[i][0][PARTICLE_THETA], 0.2);
    }
}

void WidgetGL::drawLaser(int id)
{
    SSMSOKUIKIData3D *ssm = (SSMSOKUIKIData3D*)lasers[id].ssm;

    if( !smState(ssm) ) return;
    ssm::SOKUIKIData3D &data = ssm->data;

    if(lasers[id].menu->value & 1)
    {
        glColor4dv(lasers[id].point_color.rgba);
        glPointSize(3);
        glBegin(GL_POINTS);

        for(uint i=0; i<data.numPoints(); i++)
        {
            if(data[i].isWarning()) continue;

            tkg::Point3 ref(data[i].reflect.vec);
            glVertex(lasers[id].robot.pos + ref.rotateZ(lasers[id].robot.ang));
        }
        glEnd();
        glPointSize(1);
    }

    if(lasers[id].menu->value & 2)
    {
        glColor4dv(lasers[id].laser_color.rgba);
        glLineWidth(1);
        glBegin(GL_LINES);
        for(uint i=0; i<data.numPoints(); i++)
        {
            if(data[i].isWarning()) continue;

            tkg::Point3 ori(data[i].origin.vec);
            tkg::Point3 ref(data[i].reflect.vec);
            glVertex(lasers[id].robot.pos + ori);
            glVertex(lasers[id].robot.pos + ref.rotateZ(lasers[id].robot.ang));
        }
        glEnd();
    }
}

void WidgetGL::drawPTZ()
{
    SSMApi<ysd::PTZ> *ssmapi = (SSMApi<ysd::PTZ>*)ptzcamera.ssm;

    if( !smState(ssmapi) ) return;
    ysd::PTZ &data = ssmapi->data;

    glColor3d(1.0, 1.0, 0.0);

    glLineWidth(3);
    glBegin(GL_LINES);
    double v = (-data.tilt + 90) * tkg::pi / 180;
    double h = (-data.pan  - 90) * tkg::pi / 180;
    tkg::Point3 ori(0, 0, 1.0);
    tkg::Point3 dir = tkg::Point3::polar(30.0, v, h);
    glVertex(ptzcamera.robot.pos + ori);
    glVertex(ptzcamera.robot.pos + ori + dir.rotateZ(ptzcamera.robot.ang));
    glEnd();
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

