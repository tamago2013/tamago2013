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

    robot_p = 0.0;
    robot_t = 0.0;

    ssm_robot    = new SSMApi<Spur_Odometry> (tkg::parseStr(conf["Odometry"]["ssm-name"]), tkg::parseInt(conf["Odometry"]["ssm-id"]));


    for(int i=0; i<SSM_LASER_SIZE; i++)
    {
        std::string group = tkg::strf("Laser%d", i+1);
        ssm_laser[i] = new SSMSOKUIKIData3D(tkg::parseStr(conf[group]["ssm-name"]), tkg::parseInt(conf[group]["ssm-id"]));

        color_point [i]  = tkg::Color4(conf[group]["point-color"]);
        color_laser [i]  = tkg::Color4(conf[group]["laser-color"]);

        smh_laser[i] = new SelectMenuHandler(this);

        smh_laser[i]->title = conf[group]["title"];
        smh_laser[i]->value  = 0;
        smh_laser[i]->value |= (conf[group]["view-point"]=="true" ? 1 : 0);
        smh_laser[i]->value |= (conf[group]["view-laser"]=="true" ? 2 : 0);

        smh_laser[i]->list.push_back( SelectMenuElement("non-display", 0) );
        smh_laser[i]->list.push_back( SelectMenuElement("point"      , 1) );
        smh_laser[i]->list.push_back( SelectMenuElement("laser"      , 2) );
        smh_laser[i]->list.push_back( SelectMenuElement("point+laser", 3) );

        window->addMenuView(smh_laser[i]);
    }


    std::vector<std::string> fps = tkg::parseArray(conf["Viewer"]["fps"]);
    fps_timer = new FpsMenuHandler(this);
    fps_timer->title = conf["Viewer"]["title"];
    fps_timer->value = fps.empty() ? 1 : tkg::parseInt(fps.front());
    for(int i=0; i<fps.size(); i++)
    {
        fps_timer->list.push_back( SelectMenuElement(fps[i]+" fps", tkg::parseInt(fps[i])) );
    }
    connect(fps_timer->timer, SIGNAL(timeout()), this, SLOT(update()));
    window->addMenuFps(fps_timer);



    ssm_particle = new SSMParticles(tkg::parseStr(conf["Particle"]["ssm-name"]), tkg::parseInt(conf["Particle"]["ssm-id"]));
    tmh_particle = new ToggleMenuHandler(this);
    tmh_particle->title = conf["Particle"]["title"];
    tmh_particle->value = true;
    window->addMenuView(tmh_particle);

    ssm_ptz = new SSMApi<ysd::PTZ>(tkg::parseStr(conf["PTZ"]["ssm-name"]), tkg::parseInt(conf["PTZ"]["ssm-id"]));



    camera = new Camera;

    map_opsm = new MapViewer(conf["Field"]["file"]);
    tmh_opsm = new ToggleMenuHandler(this);
    tmh_opsm->title = conf["Field"]["title"];
    tmh_opsm->value = (conf["Field"]["view-state"] == "true");
    window->addMenuView(tmh_opsm);


    route_name = conf["Route"]["file"];
    //map_opsm = new MapViewer(conf["File"]["opsm-map"]);
    tmh_route = new ToggleMenuHandler(this);
    tmh_route->title =  conf["Route"]["title"];
    tmh_route->value = (conf["Route"]["view-state"] == "true");
    window->addMenuView(tmh_route);
}

WidgetGL::~WidgetGL()
{
    tkg::debug("delete WidgetGL\n");

    delete camera;

    delete map_opsm;
    delete tmh_opsm;

    delete tmh_route;

    delete ssm_robot;

    delete ssm_particle;
    delete tmh_particle;

    delete ssm_ptz;

    for(int i=0; i<SSM_LASER_SIZE; i++)
    {
        delete ssm_laser[i];
        delete smh_laser[i];
    }
}

bool WidgetGL::init()
{
    loadRoute();
    loadMap();

    smConnect(ssm_robot);
    smConnect(ssm_laser[0]);
    smConnect(ssm_laser[1]);
    smConnect(ssm_particle);
    smConnect(ssm_ptz);

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
    drawPTZ();

    //tkg::glString("");

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
    ssmapi.push_back(ssm_ptz);

    for(uint i=0; i<ssmapi.size(); i++)
    {
        if( smState(ssmapi[i]) )
        {
            if(ssmapi[i]->isUpdate())
            {
                smReadLast(ssmapi[i]);
                ssm_time = std::min(ssm_time, ssmapi[i]->time);
            }
        }
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
    map_opsm->load();
    return true;
}

void WidgetGL::drawMap()
{
    if(tmh_opsm->value) { map_opsm->draw(); }
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
    if( !tmh_route->value ) return;

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
    if( !map_opsm->good() || !tmh_opsm->value )
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
    if( !tmh_particle->value ) return;

    SSMParticles *ssmapi = ssm_particle;

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
    SSMSOKUIKIData3D *ssmapi = ssm_laser[id];

    if( !smState(ssmapi) ) return;
    ssm::SOKUIKIData3D &data = ssmapi->data;

    if(smh_laser[id]->value & 1)
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

    if(smh_laser[id]->value & 2)
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

void WidgetGL::drawPTZ()
{
    SSMApi<ysd::PTZ> *ssmapi = ssm_ptz;

    if( !smState(ssmapi) ) return;
    ysd::PTZ &data = ssmapi->data;

    glColor3d(1.0, 1.0, 0.0);

    glLineWidth(3);
    glBegin(GL_LINES);
    double v = (-data.tilt + 90) * tkg::pi / 180;
    double h = (-data.pan  - 90) * tkg::pi / 180;
    tkg::Point3 ori(0, 0, 1.0);
    tkg::Point3 dir = tkg::Point3::polar(30.0, v, h);
    glVertex(robot_p + ori);
    glVertex(robot_p + ori + dir.rotateZ(robot_t));
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

