#ifndef WIDGET_GL_HPP
#define WIDGET_GL_HPP

#include <QtOpenGL/QGLWidget>
#include "tkg-config.hpp"

class Window;
class FpsMenuHandler;

class MapViewer;
class RouteEdit;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

namespace tkg { class Camera; }

class WidgetGL : public QGLWidget
{
    Q_OBJECT

    public:  // functions

        WidgetGL(Window *parent, tkg::ConfigFile &conf);
        ~WidgetGL();

        bool init();

    protected:  // functions

        void initializeGL();
        void paintGL();
        void resizeGL(int, int);

        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);
        void wheelEvent(QWheelEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseDoubleClickEvent(QMouseEvent *event);

    private: // variables

        double convertX(int mx);
        double convertY(int my);

        // object
        Window   *window;
        FpsMenuHandler *fps_timer;

        // map
        int savetimer;
        MapViewer *field;
        RouteEdit *route;

        // camera
        double camera_x;
        double camera_y;
        double camera_t;
        double camera_s;

        // screen
        int    wid;
        int    hei;
        double asp;

        // マウス制御クラスに移動予定
        int mouse_prev_x;
        int mouse_prev_y;
        int mouse_prev_b;
};

#endif // WIDGET_GL_HPP
