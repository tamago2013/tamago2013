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

void WidgetMSG::set_message(const std::string &str)
{
    setText(str.c_str());
}

void WidgetMSG::add_message(const std::string &str)
{
    setText(text() + str.c_str());
}

