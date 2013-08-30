#include "widget-msg.hpp"

WidgetMSG::WidgetMSG() : QLabel()
{
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}


#include <cstdio>
using namespace std;

WidgetMSG::~WidgetMSG()
{
}

void WidgetMSG::set_message(const char *msg)
{
    setText(msg);
}

void WidgetMSG::add_message(const char *msg)
{
    setText(text() + msg);
}

