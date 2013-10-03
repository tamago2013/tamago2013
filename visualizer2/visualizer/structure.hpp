#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include "ssm-laser.hpp"
#include "tkg-geometry.hpp"

class MapViewer;
class ToggleMenuHandler;
class SelectMenuHandler;
class FpsMenuHandler;

struct Robot
{
    tkg::Point3 pos;
    double      ang;
    Robot(const tkg::Point3 &p = 0.0, double a = 0.0) : pos(p), ang(a) {}
};

struct Field
{
    MapViewer         *opsm;
    ToggleMenuHandler *menu;
};

struct Route
{
    ToggleMenuHandler *menu;
    std::string                      file;
    std::vector<tkg::Point3>         node;
    std::vector<std::pair<int,int> > edge;
};

struct ViewerStream
{
    bool  sync;
    Robot robot;
    SSMApiBase *ssm;
};

struct PositionStream : public ViewerStream
{

};

struct PTZCameraStream : public ViewerStream
{

};

struct LaserStream : public ViewerStream
{
    SelectMenuHandler *menu;
    tkg::Color4 point_color;
    tkg::Color4 laser_color;
};

struct ParticleStream : public ViewerStream
{
    ToggleMenuHandler *menu;
    tkg::Color4 point_color;
    tkg::Color4 laser_color;
};


#endif // STRUCTURE_HPP
