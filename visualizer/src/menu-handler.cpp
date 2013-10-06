#include "menu-handler.hpp"
#include "tkg-utility.hpp"
#include "tkg-debug.hpp"


ToggleMenuHandler::ToggleMenuHandler()
{
    tkg::debug("new ToggleMenuHandler\n");
}

ToggleMenuHandler::~ToggleMenuHandler()
{
    tkg::debug("delete ToggleMenuHandler\n");
}

void ToggleMenuHandler::receive(bool val)
{
    value = val;
}



SelectMenuHandler::SelectMenuHandler()
{
    tkg::debug("new SelectMenuHandler\n");
}

SelectMenuHandler::~SelectMenuHandler()
{
    tkg::debug("delete SelectMenuHandler\n");
}

void SelectMenuHandler::receive(int val)
{
    value = val;
}



FpsMenuHandler::FpsMenuHandler()
{
    tkg::debug("new FpsMenuHandler\n");
    timer = new QTimer(this);
}

FpsMenuHandler::~FpsMenuHandler()
{
    tkg::debug("delete FpsMenuHandler\n");
    delete timer;
}

void FpsMenuHandler::receive(int val)
{
    timer->start(1000/val);
    SelectMenuHandler::receive(val);
}


/*
FPSTimer::FPSTimer(const std::vector<std::string> &fps)
{
    for(uint i=0; i<fps.size(); i++)
    {
        fpslist.push_back( tkg::parseInt(fps[i]) );
    }
}

std::vector<int> FPSTimer::getFPS()
{
    return fpslist;
}

void FPSTimer::setFPS(int fps)
{
    start(1000/fps);
}
*/
