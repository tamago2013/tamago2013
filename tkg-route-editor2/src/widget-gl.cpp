#include <GL/glu.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include "window.hpp"
#include "widget-gl.hpp"
#include "widget-msg.hpp"
#include "map-viewer.hpp"
#include "route-edit.hpp"
#include "menu-handler.hpp"
#include "tkg-opengl.hpp"
#include "tkg-debug.hpp"

WidgetGL::WidgetGL(Window *parent, tkg::ConfigFile &conf) : QGLWidget()
{
    tkg::debug("new WidgetGL\n");

    setFocusPolicy(Qt::StrongFocus);

    window = parent;
    route  = new RouteEdit(conf);
    field  = new MapViewer(conf["Field"]["file"]);

    savetimer = 0;

    camera_x =  0.0;
    camera_y =  0.0;
    camera_t =  0.0;
    camera_s = 10.0;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(50);
}

WidgetGL::~WidgetGL()
{
    tkg::debug("delete WidgetGL\n");
    delete field;
    delete route;
}

bool WidgetGL::init()
{
    field->load();
    route->load();
    route->loadReference();
    return true;
}

void WidgetGL::initializeGL()
{
    // 背景色の指定
    glClearColor(0, 0, 0, 0);
}

void WidgetGL::resizeGL(int w, int h)
{
    wid = w;
    hei = h;
    asp = (double)w/h;
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
    glViewport(0, 0, wid, hei);

    // カメラの設定
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double L = camera_x - asp * camera_s;
    double R = camera_x + asp * camera_s;
    double B = camera_y - 1.0 * camera_s;
    double T = camera_y + 1.0 * camera_s;
    glOrtho(L, R, B, T, -1.0, 1.0);

    // モデルの描画
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    field->draw();
    route->drawReference();
    route->draw();
    route->drawTable(window->table());
    route->readTable(window->table());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10.0, 10.0, -10.0, 10.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if(savetimer)
    {
        savetimer--;
        glColor3d(0.0, 0.03*savetimer, 0.03*savetimer);
        tkg::glString("File Saved", 0.0, 1.0, 0.0, 0.0);
    }

    glFlush();

    //window->status()->set_message("");
}

double WidgetGL::convertX(int mx) { return + (2.0 * mx - wid) / wid * asp * camera_s + camera_x; }
double WidgetGL::convertY(int my) { return - (2.0 * my - hei) / hei * 1.0 * camera_s + camera_y; }

void WidgetGL::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() & Qt::ControlModifier)
    {
        if(event->key() == Qt::Key_S) { route->save(); savetimer = 30; }
        if(event->key() == Qt::Key_Z) { route->undo(); }
    }
    else
    {
        double dx = 0;
        double dy = 0;

        if(event->key() == Qt::Key_Up   ) dy -= 0.1;
        if(event->key() == Qt::Key_Down ) dy += 0.1;
        if(event->key() == Qt::Key_Left ) dx -= 0.1;
        if(event->key() == Qt::Key_Right) dx += 0.1;

        if(event->key() == Qt::Key_W ) dy -= 0.1;
        if(event->key() == Qt::Key_S ) dy += 0.1;
        if(event->key() == Qt::Key_A ) dx -= 0.1;
        if(event->key() == Qt::Key_D ) dx += 0.1;

        if(route->selected())   route->change(-10*dy,dx);
        else                    camera_s = std::max(1.0, camera_s + dy);
    }
}

void WidgetGL::keyReleaseEvent(QKeyEvent *event)
{

}

void WidgetGL::wheelEvent(QWheelEvent *event)
{
    double dd = (double) event->delta() / 120;

    if(route->selected())   route->change(dd, 0);
    else                    camera_s = std::max(1.0, camera_s - dd);
}

void WidgetGL::mouseMoveEvent(QMouseEvent *event)
{
    if(mouse_prev_b == Qt::LeftButton)
    {
        camera_x += 2.0 * (mouse_prev_x - event->x()) / wid * asp * camera_s;
        camera_y -= 2.0 * (mouse_prev_y - event->y()) / hei * 1.0 * camera_s;
    }

    if(mouse_prev_b == Qt::RightButton)
    {
        double px = convertX( event->x() );
        double py = convertY( event->y() );
        route->move(px, py);
    }

    mouse_prev_x = event->x();
    mouse_prev_y = event->y();
}

void WidgetGL::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        double px = convertX( event->x() );
        double py = convertY( event->y() );
        route->set(px, py, camera_s/50.0);
    }

    mouse_prev_x = event->x();
    mouse_prev_y = event->y();
    mouse_prev_b = event->button();
}

void WidgetGL::mouseReleaseEvent(QMouseEvent *event)
{
    route->reset();

    mouse_prev_x = event->x();
    mouse_prev_y = event->y();
    mouse_prev_b = event->button();
}

void WidgetGL::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        double px = convertX( event->x() );
        double py = convertY( event->y() );
        route->push(px, py, camera_s/50.0);
    }
}
