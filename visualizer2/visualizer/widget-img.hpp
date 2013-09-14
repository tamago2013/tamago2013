#ifndef WIDGET_IMG_HPP
#define WIDGET_IMG_HPP

#include <QWidget>
#include "tkg-config.hpp"

class Window;
class FpsMenuHandler;
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
        FpsMenuHandler *fps_timer;

        QImage *image;
        SSMApiBase *ssmapi;

};


#endif // WIDGET_IMG_HPP
