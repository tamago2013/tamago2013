#ifndef ROUTE_EDIT_HPP
#define ROUTE_EDIT_HPP

#include <vector>
#include "tkg-config.hpp"
#include "tkg-opengl.hpp"

class QTableWidget;

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
    tkg::Point3 pos;
};

class RouteEdit
{
    public:

        RouteEdit(tkg::ConfigGroup &conf);

        void load();
        void save();
        void draw();
        void drawTable(QTableWidget *table);

        void undo();
        void reset();
        void set (double x, double y, double r);
        void push(double x, double y);
        void move(double x, double y);

    private:

        int    select;
        double radius;
        LogElement tmplog;
        std::vector<LogElement>  log;
        std::vector<tkg::Point3> node;
        std::string file;
        tkg::Color4 node_color;
        tkg::Color4 edge_color;
        tkg::Color4 text_color;
        tkg::Color4 circle_color;
};

#endif // ROUTE_EDIT_HPP
