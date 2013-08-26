#include <QTimer>
#include <QMenu>
#include "fps-timer.hpp"
#include "tkg-utility.hpp"

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
