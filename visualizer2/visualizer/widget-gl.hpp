#ifndef WIDGET_GL_HPP
#define WIDGET_GL_HPP

#include <QtOpenGL/QGLWidget>
#include "camera.hpp"
#include "tkg-utility.hpp"
#include "tkg-config.hpp"
#include "ssm-laser.hpp"
#include "ssmtype/spur-odometry.h"

class Window;
class FPSTimer;

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

        void drawGround();
        void drawMap();
        void drawRoute();
        void drawRobot();
        void drawLaser(int);

    private: // variables

        // object
        Window   *window;
        FPSTimer *timer;

        // robot state
        double robot_x;
        double robot_y;
        double robot_t;

        // map
        std::string     map_name;
        unsigned int    map_image;
        unsigned char  *map_data;
        int    map_width,  map_height;
        double map_base_x, map_base_y;
        double map_unit_x, map_unit_y;

        // route
        std::string                      route_name;
        std::vector<tkg::point3>         route_node;
        std::vector<std::pair<int,int> > route_edge;

        // ssm
        static const int SSM_LASER_SIZE = 2;
        SSMApi<Spur_Odometry> *ssm_robot;
        SSMSOKUIKIData3D      *ssm_laser[SSM_LASER_SIZE];
        tkg::color color_point[SSM_LASER_SIZE];
        tkg::color color_laser[SSM_LASER_SIZE];
        double ssm_time;

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
