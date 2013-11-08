#include "route-edit.hpp"
#include "tkg-utility.hpp"
#include <fstream>
#include <QTableWidget>

RouteEdit::RouteEdit(tkg::ConfigFile &conf)
{
    update = false;
    select = -1;
    radius = 1.0;
    tmplog.type = NONE;

    file         = conf["Route"]["file"];
    radius       = tkg::parseInt(conf["Route"]["radius"]);

    text_color   = tkg::Color4(conf["Route"]["text-color"]);
    circle_color = tkg::Color4(conf["Route"]["circle-color"]);

    for(char i=0; i<26; i++)
    {
        node_color[i] = tkg::Color4(conf["Route"][std::string("node-color-") + (char)(i+'A')]);
        edge_color[i] = tkg::Color4(conf["Route"][std::string("edge-color-") + (char)(i+'A')]);
    }

    for(int i=1; true; i++)
    {
        std::string group = tkg::strf("ReferenceRoute%d", i);

        if(conf[group].empty()) break;
        reference_file .push_back( conf[group]["file"] );
        reference_color.push_back( tkg::Color4(conf[group]["color"]) );
    }
}

void RouteEdit::load()
{
    std::ifstream fin(file.c_str());
    if( !fin ) return;

    WayPoint w;

    while(fin >> w.flag, fin.good())
    {
        fin >> w.pos.x >> w.pos.y >> w.rad >> w.spd;
        node.push_back( w );
    }
    update = true;
}

void RouteEdit::loadReference()
{
    for(uint i=0; i<reference_file.size(); i++)
    {
        std::ifstream fin(reference_file[i].c_str());
        if( !fin ) return;

        WayPoint w;
        reference_node.push_back( std::vector<WayPoint>() );

        while(fin >> w.flag, fin.good())
        {
            fin >> w.pos.x >> w.pos.y >> w.rad >> w.spd;
            reference_node.back().push_back( w );
        }
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

    /*
   if(select<0 && select<node.size())
   {
        QTableWidgetItem *pItem = table->item(select, 1);
        table->scrollToItem( pItem );
   }
   */

    table->setRowCount(0);
    for(uint i=0; i<node.size(); i++)
    {
        table->insertRow(i);
        table->setItem(i, 0, new QTableWidgetItem(tkg::strf("%d",    i            ).c_str()));
        table->setItem(i, 1, new QTableWidgetItem(tkg::strf("%c",    node[i].flag ).c_str()));
        table->setItem(i, 2, new QTableWidgetItem(tkg::strf("%+.2f", node[i].pos.x).c_str()));
        table->setItem(i, 3, new QTableWidgetItem(tkg::strf("%+.2f", node[i].pos.y).c_str()));
        table->setItem(i, 4, new QTableWidgetItem(tkg::strf("%+.2f", node[i].rad  ).c_str()));
        table->setItem(i, 5, new QTableWidgetItem(tkg::strf("%+.2f", node[i].spd  ).c_str()));
    }

}

void RouteEdit::readTable(QTableWidget *table)
{
    for(uint i=0; i<table->rowCount(); i++)
    {
        WayPoint w;
        QByteArray old_str[6], new_str[6];

        // Noは変更不可 & FlagはACEのみ

        if( table->item(i,0)->text().toAscii() != QString(tkg::strf("%d",i).c_str()) )
        {
            table->setItem(i, 0, new QTableWidgetItem(QString(tkg::strf("%d",i).c_str())));
        }

        if( table->item(i,1)->text().toAscii() != QString(tkg::strf("A").c_str()) )
        if( table->item(i,1)->text().toAscii() != QString(tkg::strf("C").c_str()) )
        if( table->item(i,1)->text().toAscii() != QString(tkg::strf("E").c_str()) )
        {
            table->setItem(i, 1, new QTableWidgetItem(QString(tkg::strf("%c",node[i].flag).c_str())));
        }

        // 新しい数値を読み込み変更されていたらログをとる

        w = node[i];

        old_str[1] = tkg::strf("%c",    node[i].flag ).c_str();
        old_str[2] = tkg::strf("%+.2f", node[i].pos.x).c_str();
        old_str[3] = tkg::strf("%+.2f", node[i].pos.y).c_str();
        old_str[4] = tkg::strf("%+.2f", node[i].rad  ).c_str();
        old_str[5] = tkg::strf("%+.2f", node[i].spd  ).c_str();

        node[i].flag  = table->item(i,1)->text().toAscii().at(0);
        node[i].pos.x = table->item(i,2)->text().toAscii().toDouble();
        node[i].pos.y = table->item(i,3)->text().toAscii().toDouble();
        node[i].rad   = table->item(i,4)->text().toAscii().toDouble();
        node[i].spd   = table->item(i,5)->text().toAscii().toDouble();

        new_str[1] = tkg::strf("%c",    node[i].flag ).c_str();
        new_str[2] = tkg::strf("%+.2f", node[i].pos.x).c_str();
        new_str[3] = tkg::strf("%+.2f", node[i].pos.y).c_str();
        new_str[4] = tkg::strf("%+.2f", node[i].rad  ).c_str();
        new_str[5] = tkg::strf("%+.2f", node[i].spd  ).c_str();

        bool changed = false;
        for(int j=1; j<6; j++)
        {
            if(old_str[j] != new_str[j])
            {
                if(j != 0) changed = true;
                table->setItem(i, j, new QTableWidgetItem(QString(new_str[j])));
            }
        }

        if(changed)
        {
            tmplog.type   = MOVE;
            tmplog.pos    = w;
            tmplog.select = i;
            log.push_back(tmplog);
            tmplog.type   = NONE;
        }
    }
}

void RouteEdit::draw()
{
    glColor3d(1.0, 1.0, 0.0);

    glLineWidth(1);
    glBegin(GL_LINES);
    for(uint i=1; i<node.size(); i++)
    {
        glColor4dv(edge_color[node[i-1].flag-'A'].rgba);
        tkg::glVertex( node[i-1].pos );
        tkg::glVertex( node[i  ].pos );
    }
    glEnd();

    glPointSize(7);
    glBegin(GL_POINTS);
    for(uint i=0; i<node.size(); i++)
    {
        glColor4dv(node_color[node[i].flag-'A'].rgba);
        tkg::glVertex( node[i].pos );
    }
    glEnd();

    glColor4dv(circle_color.rgba);
    for(uint i=0; i<node.size(); i++)
    {
        if( node[i].flag != 'C') continue;
        tkg::glCircle(node[i].pos.x, node[i].pos.y, node[i].rad);
    }

    glColor4dv(text_color.rgba);
    for(uint i=0; i<node.size(); i++)
    {
        tkg::glString(tkg::strf("%d",i), node[i].pos+tkg::Point3(0,1,0), 0.5, 0, 0);
    }
}

void RouteEdit::drawReference()
{
    for(uint j=0; j<reference_node.size(); j++)
    {
        glColor4dv(reference_color[j].rgba);

        glLineWidth(1);
        glBegin(GL_LINES);
        for(uint i=1; i<reference_node[j].size(); i++)
        {
            tkg::glVertex( reference_node[j][i-1].pos );
            tkg::glVertex( reference_node[j][i  ].pos );
        }
        glEnd();

        glPointSize(7);
        glBegin(GL_POINTS);
        for(uint i=0; i<reference_node[j].size(); i++)
        {
            tkg::glVertex( reference_node[j][i].pos );
        }
        glEnd();

        for(uint i=0; i<reference_node[j].size(); i++)
        {
            tkg::glString(tkg::strf("%d",i), reference_node[j][i].pos+tkg::Point3(0,1,0), 0.5, 0, 0);
        }
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

        case DELETE:
            node.insert(node.begin() + g.select, g.pos);
            break;

        case INSERT:
            node.erase(node.begin() + g.select);
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

void RouteEdit::push(double x, double y, double r)
{
    tkg::Point3 pos(x,y,0);
    int    target  = -1;
    double mindist = tkg::inf;

    // delete mode -----------------------------

    for(uint i=0; i<node.size(); i++)
    {
        double dist = (node[i].pos - pos).abs();
        if( dist > r) continue;
        if( dist < mindist )
        {
            target  = i;
            mindist = dist;
        }
    }

    if(target != -1)
    {
        if(!log.empty()) log.pop_back();

        tmplog.type   = DELETE;
        tmplog.select = target;
        tmplog.pos    = node[target];
        log.push_back( tmplog );
        tmplog.type = NONE;

        node.erase(node.begin() + target);

        update = true; return;
    }

    // insert mode -----------------------------

    for(uint i=1; i<node.size(); i++)
    {
        tkg::Point3 u = node[i].pos - node[i-1].pos;
        tkg::Point3 v =         pos - node[i-1].pos;

        double dot = ( u.x * v.x + u.y * v.y ) / u.abs() / u.abs();
        if(dot < 0.0 || 1.0 < dot) continue;

        double dist = abs( u.x * v.y - u.y * v.x ) / u.abs();
        if( dist > r) continue;
        if( dist < mindist )
        {
            target  = i;
            mindist = dist;
        }
    }

    if(target != -1)
    {
        tmplog.type   = INSERT;
        tmplog.select = target;
        log.push_back( tmplog );
        tmplog.type = NONE;

        WayPoint w;
        w.flag = node[target-1].flag;
        w.pos  = pos;
        w.rad  = node[target-1].rad;
        w.spd  = node[target-1].spd;
        node.insert(node.begin() + target, w);

        update = true; return;
    }

    // push mode -------------------------------

    tmplog.type = PUSH;
    log.push_back( tmplog );
    tmplog.type = NONE;

    WayPoint w;
    w.flag = 'A';
    w.pos  = pos;
    w.rad  = radius;
    w.spd =  0.7;

    node.push_back( w );
    update = true; return;

    // -----------------------------------------
}

void RouteEdit::move(double x, double y)
{
    if(select<0 || node.size()<=select) return;
    node[select].pos = tkg::Point3(x,y,0);

    update = true;
}

void RouteEdit::change(double r, double s)
{
    if(select<0 || node.size()<=select) return;

    node[select].rad += r;
    if(node[select].rad < 0) node[select].rad = 0;

    node[select].spd += s;
    if(node[select].spd < 0) node[select].spd = 0;

    update = true;
}
