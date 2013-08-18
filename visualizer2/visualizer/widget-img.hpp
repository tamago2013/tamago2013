#ifndef WIDGETIMG_HPP
#define WIDGETIMG_HPP

#include <QWidget>
#include "tkg-config.hpp"

class Window;
class FPSTimer;
class SSMApiBase;

class WidgetIMG : public QWidget
{
    Q_OBJECT

    public:  // functions

        WidgetIMG(Window *parent, tkg::ConfigGroup &conf);
        ~WidgetIMG();

        void init();

    private: // functions

        int  heightForWidth(int) const;
        void paintEvent(QPaintEvent *);

    private: // variables

        Window   *window;
        FPSTimer *timer;

        QImage *image;
        SSMApiBase *ssmapi;

};


#endif // WIDGETIMG_HPP
