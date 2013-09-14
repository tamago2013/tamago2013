#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QMainWindow>
#include "tkg-config.hpp"

class FPSTimer;
class WidgetGL;
class WidgetIMG;
class WidgetMSG;

class MenuHandler;

class Window : public QMainWindow
{
    Q_OBJECT

    public:

        Window(tkg::ConfigFile& config);
        ~Window();

        bool init();

        void addMenuFps (MenuHandler *handler);
        void addMenuView(MenuHandler *handler);

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
        QMenu *m_ssm;
        QMenu *m_fps;


};

#endif
