#ifndef ROUTE_EDIT_HPP
#define ROUTE_EDIT_HPP

#include <vector>
#include "tkg-config.hpp"
#include "tkg-opengl.hpp"

class QTableWidget;


struct WayPoint
{
    char flag;
    tkg::Point3 pos;
    double right;
    double left;
    double ex;
    double spd;
};

enum LogType
{
    NONE,
    PUSH,
    MOVE,
    DELETE,
    INSERT
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

        RouteEdit(tkg::ConfigFile &conf);

        void load();
        void loadReference();
        void save();
        void draw();
        void drawReference();
        void drawTable(QTableWidget *table);
        void readTable(QTableWidget *table);

        bool selected();
        void undo();
        void reset();
        void set (double x, double y, double r);
        void push(double x, double y, double r);
        void move(double x, double y);
        void change(double s);

    private:

        bool   update;
        double detect;
        int    node_select;
        int    edge_select;

        LogElement tmplog;
        std::vector<LogElement>  log;
        std::vector<WayPoint> node;
        std::string file;
        tkg::Color4 text_color;
        tkg::Color4 node_color[26];
        tkg::Color4 edge_color[26];
        tkg::Color4 inner_color;
        tkg::Color4 outer_color;

        std::vector< std::string >           reference_file;
        std::vector< tkg::Color4 >           reference_color;
        std::vector< std::vector<WayPoint> > reference_node;

        std::vector< tkg::Point3 > inner_box(int i);
        std::vector< tkg::Point3 > outer_box(int i);
};

#endif // ROUTE_EDIT_HPP
