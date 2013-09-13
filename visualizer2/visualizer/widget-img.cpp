#include <QPainter>
#include <ssm.hpp>
#include "window.hpp"
#include "widget-img.hpp"
#include "fps-timer.hpp"
#include "ssm-message.hpp"
#include "tkg-config.hpp"
#include "tkg-utility.hpp"
using namespace tkg;

typedef uchar ssmimage[640*480*3];

WidgetIMG::WidgetIMG(Window* parent, tkg::ConfigGroup &conf)
{
    window = parent;
    timer  = new FPSTimer(tkg::parseArray(conf["fps"]));
    image  = NULL;
    ssmapi = new SSMApi<ssmimage>(tkg::parseStr(conf["ssm-name"]), tkg::parseInt(conf["ssm-id"]));

    window->addMenuFPS(timer, tkg::parseStr(conf["title"]));
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
}

WidgetIMG::~WidgetIMG()
{
    delete image;
    delete ssmapi;
    delete timer;
}

int WidgetIMG::heightForWidth(int w) const
{
    return w*3/4;
}

void WidgetIMG::init()
{
    smConnect(ssmapi);
}

void WidgetIMG::paintEvent(QPaintEvent *)
{
    smReadLast(ssmapi);
    {
        ssmimage &data = (*(ssmimage*)(ssmapi->data()));

        QImage testimage(data, 640, 480, QImage::Format_RGB888 );
        testimage = testimage.rgbSwapped();

        QPainter painter(this);
        painter.drawImage(0, 0, testimage.scaled(width(), height(), Qt::KeepAspectRatio,Qt::FastTransformation));
    }
}
