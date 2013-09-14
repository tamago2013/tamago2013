#include "menu-handler.hpp"
#include "tkg-utility.hpp"

#include <cstdio>

MenuHandler::MenuHandler(QObject *parent) : QObject(parent)
{
}

MenuHandler::~MenuHandler()
{
    std::printf("delete MenuHander\n");
}

void MenuHandler::receive(int val)
{
}


ViewMenuHandler::ViewMenuHandler(QObject *parent) : MenuHandler(parent)
{
}

ViewMenuHandler::~ViewMenuHandler()
{
    std::printf("delete ViewMenuHander\n");
}

void ViewMenuHandler::receive(int val)
{
    value = val;
}


FpsMenuHandler::FpsMenuHandler(QObject *parent) : MenuHandler(parent)
{
    timer = new QTimer(this);
}

FpsMenuHandler::~FpsMenuHandler()
{
    std::printf("delete FpsMenuHander\n");
}

void FpsMenuHandler::receive(int val)
{
    timer->start(1000/val);
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
