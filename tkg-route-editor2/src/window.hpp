#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QMainWindow>
#include "tkg-config.hpp"

class WidgetGL;
class WidgetMSG;
class QTableWidget;
class ToggleMenuHandler;
class SelectMenuHandler;

class Window : public QMainWindow
{
    Q_OBJECT

    public:

        Window(tkg::ConfigFile& config);
        ~Window();

        bool init();

        void addMenuView(SelectMenuHandler *handler);
        void addMenuView(ToggleMenuHandler *handler);

        WidgetGL*     viewer () { return w_viewer;  }
        //WidgetMSG*    status () { return w_status;  }
        //WidgetMSG*    message() { return w_message; }
        QTableWidget* table  () { return w_table;   }

    private:


        tkg::ConfigFile *config_f;

        // widgets

        WidgetGL     *w_viewer;
        //WidgetMSG    *w_status;
        //WidgetMSG    *w_message;
        QTableWidget *w_table;

        //int fps;
        QMenu *m_ssm;
        QMenu *m_fps;
};

#endif
