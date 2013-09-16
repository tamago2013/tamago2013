#include "widget-msg.hpp"
#include "tkg-debug.hpp"

WidgetMSG::WidgetMSG() : QLabel()
{
    tkg::debug("new WidgetMSG\n");
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

WidgetMSG::~WidgetMSG()
{
    tkg::debug("delete WidgetMSG\n");
}

void WidgetMSG::set_message(const char *msg)
{
    setText(msg);
}

void WidgetMSG::add_message(const char *msg)
{
    setText(text() + msg);
}

