#ifndef ROUTE_EDIT_HPP
#define ROUTE_EDIT_HPP

#include <vector>
#include "tkg-config.hpp"
#include "tkg-opengl.hpp"

class QTableWidget;


struct WayPoint
{
    char flag;
    double rad;
    double spd;
    tkg::Point3 pos;
};

enum LogType
{
    NONE,
    PUSH,
    MOVE
};

struct LogElement
{
    LogType type;
    int select;
    WayPoint pos;
};

class RouteEdit
{
    public:

        RouteEdit(tkg::ConfigGroup &conf);

        void load();
        void save();
        void draw();
        void drawTable(QTableWidget *table);
        void readTable(QTableWidget *table);

        bool selected();
        void undo();
        void reset();
        void set (double x, double y, double r);
        void push(double x, double y);
        void move(double x, double y);
        void change(double r, double s);

    private:

        bool   update;
        int    select;
        double radius;
        LogElement tmplog;
        std::vector<LogElement>  log;
        std::vector<WayPoint> node;
        std::string file;
        tkg::Color4 node_color[26];
        tkg::Color4 edge_color[26];
        tkg::Color4 text_color[26];
        tkg::Color4 circle_color[26];
};

#endif // ROUTE_EDIT_HPP
