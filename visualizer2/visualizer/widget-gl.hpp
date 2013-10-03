#ifndef WIDGET_GL_HPP
#define WIDGET_GL_HPP

#include <QtOpenGL/QGLWidget>
#include "camera.hpp"
#include "tkg-utility.hpp"
#include "tkg-config.hpp"
#include "tkg-opengl.hpp"
#include "ssm-particles.hpp"
#include "ssm-ptz.hpp"
#include "ssmtype/spur-odometry.h"
#include "structure.hpp"

class Window;
class MapViewer;
class ToggleMenuHandler;
class SelectMenuHandler;
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

        bool loadMap();
        bool loadRoute();

        void updateStream();

        void drawGround();
        void drawMap();
        void drawRoute();
        void drawRobot();
        void drawLaser(int);
        void drawParticles();
        void drawPTZ();

    private: // variables

        // object
        Window   *window;
        FpsMenuHandler *fps_timer;

        // robot
        std::vector<Robot> robot_log;

        // map
        Field field;
        Route route;

        // ssm
        PositionStream  position;
        ParticleStream  particle;
        PTZCameraStream ptzcamera;
        std::vector<LaserStream>   lasers;
        std::vector<ViewerStream*> stream;

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
