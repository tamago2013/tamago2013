#ifndef WIDGET_GL_HPP
#define WIDGET_GL_HPP

#include <QtOpenGL/QGLWidget>
#include "camera.hpp"
#include "tkg-utility.hpp"
#include "tkg-config.hpp"
#include "tkg-opengl.hpp"
#include "ssm-laser.hpp"
#include "ssm-particles.hpp"
#include "ssmtype/spur-odometry.h"

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

    private: // variables

        // object
        Window   *window;
        FpsMenuHandler *fps_timer;

        // robot
        tkg::Point3 robot_p;
        double      robot_t;
        std::vector<tkg::Point3> robot_log_p;
        std::vector<double>      robot_log_t;


        // opsm map
        MapViewer         *map_opsm;
        ToggleMenuHandler *tmh_opsm;

        // route
        //RouteViewer       *map_route;
        ToggleMenuHandler *tmh_route;
        std::string                      route_name;
        std::vector<tkg::Point3>         route_node;
        std::vector<std::pair<int,int> > route_edge;

        // ssm odometry
        SSMApi<Spur_Odometry> *ssm_robot;

        // ssm particle
        SSMParticles      *ssm_particle;
        ToggleMenuHandler *tmh_particle;

        // ssm laser
        static const int      SSM_LASER_SIZE = 2;
        SSMSOKUIKIData3D      *ssm_laser[SSM_LASER_SIZE];
        SelectMenuHandler     *smh_laser[SSM_LASER_SIZE];
        tkg::Color4          color_point[SSM_LASER_SIZE];
        tkg::Color4          color_laser[SSM_LASER_SIZE];

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
