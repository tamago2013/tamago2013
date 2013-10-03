#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include <ssm.hpp>
#include "tkg-opengl.hpp"
#include "tkg-config.hpp"
#include "tkg-geometry.hpp"

class Window;
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



class FieldViewer
{
    public:

        FieldViewer(Window *window, tkg::ConfigGroup &conf);
        ~FieldViewer();

        std::string load();
        void draw(const Robot &robot);

    private:

        ToggleMenuHandler *menu;
        MapViewer         *opsm;
};



class RouteViewer
{
    public:

        RouteViewer(Window *window, tkg::ConfigGroup &conf);
        ~RouteViewer();

        std::string load();
        void draw();

    private:

        ToggleMenuHandler *menu;
        std::string file;
        tkg::Color4 node_color;
        tkg::Color4 edge_color;
        std::vector<tkg::Point3>         node;
        std::vector<std::pair<int,int> > edge;
};



class StreamViewer
{
    public:

        StreamViewer() { ssm = NULL; }
        StreamViewer(tkg::ConfigGroup &conf);
        ~StreamViewer();

        virtual void draw() = 0;

        //std::string init();
        //update();

    bool  sync;
    Robot robot;
    SSMApiBase *ssm;
};



struct PositionViewer : public StreamViewer
{
    public:

        PositionViewer(Window *window, tkg::ConfigGroup &conf);
        ~PositionViewer();

        void draw();



    // tmp
    std::vector<Robot> history;

    private:

        ToggleMenuHandler *menu;
        tkg::Color4 now_color;
        tkg::Color4 log_color;
};



struct PTZCameraViewer : public StreamViewer
{
    public:

        PTZCameraViewer(Window *window, tkg::ConfigGroup &conf);
        ~PTZCameraViewer();

        void draw();

    private:

        SelectMenuHandler *menu;
        double height;
        tkg::Color4 color;
};



class ClusterViewer : public StreamViewer
{
    public:

        ClusterViewer(Window *window, tkg::ConfigGroup &conf);
        ~ClusterViewer();

        void draw();

    private:

        ToggleMenuHandler *menu;
        double height;
        tkg::Color4 color;
};



struct ParticleViewer : public StreamViewer
{
    public:

        ParticleViewer(Window *window, tkg::ConfigGroup &conf);
        ~ParticleViewer();

        void draw();

    private:

        ToggleMenuHandler *menu;
        tkg::Color4 color;
};



struct LaserViewer : public StreamViewer
{
    public:

        LaserViewer(Window *window, tkg::ConfigGroup &conf);
        ~LaserViewer();

        void draw();

    private:

        SelectMenuHandler *menu;
        tkg::Color4 point_color;
        tkg::Color4 laser_color;
};



#endif // STRUCTURE_HPP
