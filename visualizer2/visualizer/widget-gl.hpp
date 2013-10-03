#ifndef WIDGET_GL_HPP
#define WIDGET_GL_HPP

#include <QtOpenGL/QGLWidget>
#include "camera.hpp"
#include "tkg-config.hpp"
#include "structure.hpp"

class Window;
class FpsMenuHandler;

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class WidgetGL : public QGLWidget
{
    Q_OBJECT

    public:  // functions

        WidgetGL(Window *parent, tkg::ConfigFile &conf);
        ~WidgetGL();

        bool init();
        Camera*  get_camera() { return camera; }

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

    private:  // functions

        void updateStream();

    private: // variables

        // object
        Window   *window;
        FpsMenuHandler *fps_timer;

        // robot
        std::vector<Robot> robot_log;

        // map viewer
        FieldViewer *field;
        RouteViewer *route;

        // ssm viewer
        PositionViewer  *position;
        ParticleViewer  *particle;
        ClusterViewer   *cluster;
        PTZCameraViewer *ptzcamera;
        std::vector<LaserViewer*>  lasers;
        std::vector<StreamViewer*> stream;

        // screen
        int    width;
        int    height;
        double aspect;

        // カメラ
        Camera *camera;

        // マウス制御クラスに移動予定
        int mouse_prev_x;
        int mouse_prev_y;
        int mouse_prev_b;
};

#endif // WIDGET_GL_HPP
