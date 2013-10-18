#include "route-edit.hpp"
#include "tkg-utility.hpp"
#include <fstream>
#include <QTableWidget>

RouteEdit::RouteEdit(tkg::ConfigGroup &conf)
{
    select = -1;
    radius = 1.0;
    tmplog.type = NONE;

    file         = conf["file"];
    radius       = tkg::parseInt(conf["radius"]);
    node_color   = tkg::Color4(conf["node-color"]);
    edge_color   = tkg::Color4(conf["edge-color"]);
    text_color   = tkg::Color4(conf["text-color"]);
    circle_color = tkg::Color4(conf["circle-color"]);

    load();
}

void RouteEdit::load()
{
    std::ifstream fin(file.c_str());
    if( !fin ) return;

    node.clear();
    while(fin.good())
    {
        char c;
        double x,y;
        fin >> c >> x >> y;
        node.push_back(tkg::Point3(x,y,0));
    }
}

void RouteEdit::save()
{
    std::ofstream fout(file.c_str());
    if( !fout ) return;

    for(uint i=0; i<node.size(); i++)
    {
        fout << 'A' << node[i].x << " " << node[i].y << std::endl;
    }
}

void RouteEdit::drawTable(QTableWidget *table)
{
    //w_table->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    //w_table->verticalHeader()->hide();
    //w_table->setShowGrid(true);

    table->setRowCount(0);
    for(uint i=0; i<node.size(); i++)
    {
        table->insertRow(i);
        table->setItem(i, 0, new QTableWidgetItem(tkg::strf("%f", node[i].x).c_str()));
        table->setItem(i, 1, new QTableWidgetItem(tkg::strf("%f", node[i].y).c_str()));
    }

}

void RouteEdit::draw()
{
    glColor3d(1.0, 1.0, 0.0);

    glColor4dv(node_color.rgba);
    glPointSize(5);
    glBegin(GL_POINTS);
    for(uint i=0; i<node.size(); i++)
    {
        tkg::glVertex( node[i] );
    }
    glEnd();

    glColor4dv(edge_color.rgba);
    glLineWidth(1);
    glBegin(GL_LINES);
    for(uint i=1; i<node.size(); i++)
    {
        tkg::glVertex( node[i-1] );
        tkg::glVertex( node[i  ] );
    }
    glEnd();

    glColor4dv(circle_color.rgba);
    for(uint i=0; i<node.size(); i++)
    {
        tkg::glCircle(node[i].x, node[i].y, radius);
    }

    glColor4dv(text_color.rgba);
    for(uint i=0; i<node.size(); i++)
    {
        tkg::glString(tkg::strf("%d",i+1), node[i]+tkg::Point3(0,1,0), 0.5, 0, 0);
    }
}

void RouteEdit::undo()
{
    if(log.empty()) return;
    LogElement g = log.back(); log.pop_back();

    switch(g.type)
    {
        case PUSH:
            node.pop_back();
            break;

        case MOVE:
            node[g.select] = g.pos;
            break;
    }
}

void RouteEdit::reset()
{
    select = -1;
    if(tmplog.type == NONE) return;

    log.push_back(tmplog);
    tmplog.type = NONE;
}

void RouteEdit::set(double x, double y, double r)
{
    tkg::Point3 p(x,y,0);
    select = -1;
    double mindist = tkg::inf;

    for(uint i=0; i<node.size(); i++)
    {
        double dist = (node[i] - p).abs();
        if( dist > r) continue;
        if( dist < mindist )
        {
            select  = i;
            mindist = dist;
        }
    }

    if(0 <= select && select < node.size())
    {
        tmplog.type   = MOVE;
        tmplog.select = select;
        tmplog.pos    = node[select];
    }
}

void RouteEdit::push(double x, double y)
{
    tmplog.type   = PUSH;
    node.push_back( tkg::Point3(x,y,0) );
}

void RouteEdit::move(double x, double y)
{
    if(select<0 && node.size()<=select) return;
    node[select] = tkg::Point3(x,y,0);
}
