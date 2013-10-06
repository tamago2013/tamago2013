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
#include "tkg-debug.hpp"

#include "ssm-ptz.hpp"

WidgetGL::WidgetGL(Window *parent, tkg::ConfigFile &conf) : QGLWidget()
{
    tkg::debug("new WidgetGL\n");

    setFocusPolicy(Qt::StrongFocus);

    window = parent;

    field = new FieldViewer(window, conf["Field"]);
    route = new RouteViewer(window, conf["Route"]);

    position  = new PositionViewer (window, conf["Position"]);
    particle  = new ParticleViewer (window, conf["Particle"]);
    cluster   = new ClusterViewer  (window, conf["Cluster"]);
    ptzcamera = new PTZCameraViewer(window, conf["PTZCamera"]);

    for(int i=1; true; i++)
    {
        std::string group = tkg::strf("Laser%d", i);
        if(conf.find(group) == conf.end()) break;
        lasers.push_back( new LaserViewer(window, conf[group]) );
    }


    stream.push_back(position);
    stream.push_back(particle);
    stream.push_back(cluster);
    stream.push_back(ptzcamera);
    for(uint i=0; i<lasers.size(); i++)
    {
        stream.push_back(lasers[i]);
    }

    camera = new Camera;

    std::vector<std::string> fps = tkg::parseArray(conf["Viewer"]["fps"]);
    fps_timer = new FpsMenuHandler;
    fps_timer->title = conf["Viewer"]["title"];
    fps_timer->value = fps.empty() ? 1 : tkg::parseInt(fps.front());
    for(uint i=0; i<fps.size(); i++)
    {
        fps_timer->list.push_back( SelectMenuElement(fps[i]+" fps", tkg::parseInt(fps[i])) );
    }
    connect(fps_timer->timer, SIGNAL(timeout()), this, SLOT(update()));
    window->addMenuFps(fps_timer);
}

WidgetGL::~WidgetGL()
{
    tkg::debug("delete WidgetGL\n");

    delete camera;

    delete field;
    delete route;

    for(uint i=0; i<stream.size(); i++)
    {
        delete stream[i];
    }
}

bool WidgetGL::init()
{
    field->load();
    window->message()->add_message( route->load() );

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

    camera->setpos(position->robot.pos.x, position->robot.pos.y, position->robot.ang);
    camera->update();

    // モデルの描画
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    updateStream();

    field->draw(position->robot);
    route->draw();
    position->draw();
    particle->draw();
    for(uint i=0; i<lasers.size(); i++)
    {
        lasers[i]->draw();
    }
    ptzcamera->draw();
    cluster->draw();

    tkg::glString("01234567890123456789", 0.0, 0.5, camera->rotv, camera->roth);

    glFlush();

    window->status()->set_message("");
    window->status()->add_message( tkg::strf("robot_x = %f\n", position->robot.pos.x) );
    window->status()->add_message( tkg::strf("robot_y = %f\n", position->robot.pos.y) );
    window->status()->add_message( tkg::strf("robot_t = %f\n", position->robot.ang)   );
    window->status()->add_message( tkg::strf("camera_pan  = %f\n", ((ysd::PTZ*)ptzcamera->ssm->data())->pan)  );
    window->status()->add_message( tkg::strf("camera_tilt = %f\n", ((ysd::PTZ*)ptzcamera->ssm->data())->tilt) );
    window->status()->add_message( tkg::strf("camera_zoom = %f\n", ((ysd::PTZ*)ptzcamera->ssm->data())->zoom) );
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
            if(stream[i]->ssm != position->ssm)
            {
                stream[i]->robot = position->robot;
            }
        }
        else
        {
            smReadTime(stream[i]->ssm, ssm_time);
            if(stream[i]->ssm != position->ssm)
            {
                stream[i]->robot = position->robot;
            }
        }
    }

    if(position->history.empty() || (position->history.back().pos - position->robot.pos).abs() > 3.0)
    {
        position->history.push_back(position->robot);
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

