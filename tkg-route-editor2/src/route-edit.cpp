#include "route-edit.hpp"
#include "tkg-utility.hpp"
#include <fstream>
#include <QTableWidget>

RouteEdit::RouteEdit(tkg::ConfigGroup &conf)
{
    update = false;
    select = -1;
    radius = 1.0;
    tmplog.type = NONE;

    file         = conf["file"];
    radius       = tkg::parseInt(conf["radius"]);

    for(char i=0; i<26; i++)
    {
        node_color[i]   = tkg::Color4(conf[std::string("node-color-") + (char)(i+'A')]);
        edge_color[i]   = tkg::Color4(conf[std::string("edge-color-") + (char)(i+'A')]);
        text_color[i]   = tkg::Color4(conf[std::string("text-color-") + (char)(i+'A')]);
        circle_color[i] = tkg::Color4(conf[std::string("circle-color-") + (char)(i+'A')]);
    }
    load();
}

void RouteEdit::load()
{
    std::ifstream fin(file.c_str());
    if( !fin ) return;

    node.clear();
    while(fin.good())
    {
        WayPoint w;
        fin >> w.flag >> w.pos.x >> w.pos.y >> w.rad >> w.spd;
        node.push_back( w );
    }
}

void RouteEdit::save()
{
    std::ofstream fout(file.c_str());
    if( !fout ) return;

    for(uint i=0; i<node.size(); i++)
    {
        WayPoint &w = node[i];
        fout << w.flag << " " << w.pos.x << " " << w.pos.y << " " << w.rad << " " << w.spd << std::endl;
    }
}

void RouteEdit::drawTable(QTableWidget *table)
{
    if(!update) return;
    update=false;

   if(select<0 && select<node.size())
   {
        QTableWidgetItem *pItem = table->item(select, 1);
        table->scrollToItem( pItem );
   }

    table->setRowCount(0);
    for(uint i=0; i<node.size(); i++)
    {
        table->insertRow(i);
        table->setItem(i, 0, new QTableWidgetItem(tkg::strf("%c",    node[i].flag ).c_str()));
        table->setItem(i, 1, new QTableWidgetItem(tkg::strf("%+.2f", node[i].pos.x).c_str()));
        table->setItem(i, 2, new QTableWidgetItem(tkg::strf("%+.2f", node[i].pos.y).c_str()));
        table->setItem(i, 3, new QTableWidgetItem(tkg::strf("%+.2f", node[i].rad  ).c_str()));
        table->setItem(i, 4, new QTableWidgetItem(tkg::strf("%+.2f", node[i].spd  ).c_str()));
    }

}

void RouteEdit::readTable(QTableWidget *table)
{
    for(uint i=0; i<node.size(); i++)
    {
        node[i].flag  = table->item(i,0)->text().toAscii().at(0);
        node[i].pos.x = table->item(i,1)->text().toAscii().toDouble();
        node[i].pos.y = table->item(i,2)->text().toAscii().toDouble();
        node[i].rad   = table->item(i,3)->text().toAscii().toDouble();
        node[i].spd   = table->item(i,4)->text().toAscii().toDouble();
    }
}

void RouteEdit::draw()
{
    glColor3d(1.0, 1.0, 0.0);

    glPointSize(5);
    glBegin(GL_POINTS);
    for(uint i=0; i<node.size(); i++)
    {
        glColor4dv(edge_color[node[i].flag-'A'].rgba);
        tkg::glVertex( node[i].pos );
    }
    glEnd();

    glLineWidth(1);
    glBegin(GL_LINES);
    for(uint i=1; i<node.size(); i++)
    {
        glColor4dv(edge_color[node[i-1].flag-'A'].rgba);
        tkg::glVertex( node[i-1].pos );
        tkg::glVertex( node[i  ].pos );
    }
    glEnd();

    for(uint i=0; i<node.size(); i++)
    {
        glColor4dv(edge_color[node[i].flag-'A'].rgba);
        tkg::glCircle(node[i].pos.x, node[i].pos.y, node[i].rad);
    }

    for(uint i=0; i<node.size(); i++)
    {
        glColor4dv(edge_color[node[i].flag-'A'].rgba);
        tkg::glString(tkg::strf("%d",i+1), node[i].pos+tkg::Point3(0,1,0), 0.5, 0, 0);
    }
}

bool RouteEdit::selected()
{
    return (select != -1);
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

    update = true;
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
        double dist = (node[i].pos - p).abs();
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

    WayPoint w;
    w.flag = 'A';
    w.pos  = tkg::Point3(x,y);
    w.rad  = radius;
    w.spd =  0.7;

    node.push_back( w );
    update = true;
}

void RouteEdit::move(double x, double y)
{
    if(select<0 || node.size()<=select) return;
    node[select].pos = tkg::Point3(x,y,0);

    update = true;
}

void RouteEdit::rad(double r)
{
    if(select<0 || node.size()<=select) return;
    node[select].rad += r;
    if(node[select].rad < 0) node[select].rad = 0;

    update = true;
}
