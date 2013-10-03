#include <QPainter>
#include <ssm.hpp>
#include "window.hpp"
#include "widget-img.hpp"
#include "menu-handler.hpp"
#include "ssm-message.hpp"
#include "tkg-config.hpp"
#include "tkg-utility.hpp"
#include "tkg-debug.hpp"

typedef uchar ssmimage[640*480*3];

WidgetIMG::WidgetIMG(Window* parent, tkg::ConfigGroup &conf)
{
    tkg::debug("new WidgetIMG\n");

    window = parent;
    image  = NULL;
    ssmapi = new SSMApi<ssmimage>(tkg::parseStr(conf["ssm-name"]), tkg::parseInt(conf["ssm-id"]));

    std::vector<std::string> fps = tkg::parseArray(conf["fps"]);
    fps_timer = new FpsMenuHandler(this);
    fps_timer->title = conf["title"];
    fps_timer->value = fps.empty() ? 1 : tkg::parseInt(fps.front());
    for(uint i=0; i<fps.size(); i++)
    {
        fps_timer->list.push_back( SelectMenuElement(fps[i]+" fps", tkg::parseInt(fps[i])) );
    }
    connect(fps_timer->timer, SIGNAL(timeout()), this, SLOT(update()));
    window->addMenuFps(fps_timer);
}

WidgetIMG::~WidgetIMG()
{
    tkg::debug("delete WidgetIMG\n");
    delete image;
    delete ssmapi;
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
