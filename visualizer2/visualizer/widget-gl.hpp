#ifndef WIDGET_GL_HPP
#define WIDGET_GL_HPP

#include <QtOpenGL/QGLWidget>
#include <QTimer>
#include "camera.hpp"
#include "tkg-config.hpp"

class Window;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class ViewerMap;

class FPSTimer;
class SSMApiBase;

class WidgetGL : public QGLWidget
{
    Q_OBJECT

    public:  // functions

        WidgetGL(Window *parent, tkg::ConfigFile &conf);
        ~WidgetGL();

         bool init();
         ViewerMap* get_vmap() { return vmap; }
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

        void drawGround();
        void drawRobot(int id);
        void drawLaser(int id);

    private: // variables

        static const int SSM_COUNT = 3;
        SSMApiBase *ssm[SSM_COUNT];

        // viewmode[STREAM_MAX]

        //
        Window   *window;
        FPSTimer *timer;


        //
        double robot_x;
        double robot_y;
        double robot_t;

        ViewerMap *vmap;
        //ViewerSSM *vssm;

        // 画面サイズ関連
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
