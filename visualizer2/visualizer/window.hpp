#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QMainWindow>
#include "tkg-config.hpp"

class FPSTimer;
class WidgetGL;
class WidgetIMG;
class WidgetMSG;


class Window : public QMainWindow
{
    Q_OBJECT

    public:

        Window(tkg::ConfigFile& config);
        ~Window();

        bool init();


        void addMenuFPS (FPSTimer *obj, const char *str);
        //void addMenuView(const char *str)

        WidgetGL*  viewer () { return w_viewer;  }
        WidgetIMG* camera1() { return w_camera1; }
        WidgetIMG* camera2() { return w_camera2; }
        WidgetMSG* status () { return w_status;  }
        WidgetMSG* message() { return w_message; }
        WidgetMSG* control() { return w_control; }

    private:


        tkg::ConfigFile *config_f;

        // widgets

        WidgetGL  *w_viewer;
        WidgetIMG *w_camera1;
        WidgetIMG *w_camera2;
        WidgetMSG *w_status;
        WidgetMSG *w_message;
        WidgetMSG *w_control;

        //int fps;
        QMenu *ssm_menu;
        QMenu *m_fps;


};

#endif
