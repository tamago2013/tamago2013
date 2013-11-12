#include "route-edit.hpp"
#include "tkg-utility.hpp"
#include <fstream>
#include <sstream>
#include <QTableWidget>

std::vector<tkg::Point3> RouteEdit::inner_box(int i)
{
    tkg::Point3 v = (node[i+1].pos - node[i].pos).unit();
    tkg::Point3 w = v.rotateZ(tkg::pi/2);

    std::vector<tkg::Point3> ret(4);
    ret[0] = node[i  ].pos + w * node[i].left;
    ret[1] = node[i+1].pos + w * node[i].left  + v * node[i].ex;
    ret[2] = node[i+1].pos - w * node[i].right + v * node[i].ex;
    ret[3] = node[i  ].pos - w * node[i].right;
    return ret;
}

std::vector<tkg::Point3> RouteEdit::outer_box(int i)
{
    tkg::Point3 v = (node[i+1].pos - node[i].pos).unit();
    tkg::Point3 w = v.rotateZ(tkg::pi/2);

    std::vector<tkg::Point3> ret(4);
    ret[0] = node[i  ].pos + w * (node[i].left  + detect);
    ret[1] = node[i+1].pos + w * (node[i].left  + detect) + v * node[i].ex;
    ret[2] = node[i+1].pos - w * (node[i].right + detect) + v * node[i].ex;
    ret[3] = node[i  ].pos - w * (node[i].right + detect);
    return ret;
}

RouteEdit::RouteEdit(tkg::ConfigFile &conf)
{
    update      = false;
    node_select = -1;
    edge_select = -1;
    tmplog.type = NONE;

    file        = conf["Route"]["file"];
    detect      = tkg::parseInt(conf["Route"]["detect-length"]) / 1000.0;

    text_color  = tkg::Color4(conf["Route"]["text-color"]);
    inner_color = tkg::Color4(conf["Route"]["innerbox-color"]);
    outer_color = tkg::Color4(conf["Route"]["outerbox-color"]);

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
        fin >> w.pos.x >> w.pos.y >> w.right >> w.left >> w.ex >> w.spd;
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
            std::string str;
            std::getline(fin, str);
            std::istringstream sin(str);

            sin >> w.pos.x >> w.pos.y;
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
        fout << w.flag << " " << w.pos.x << " " << w.pos.y << " ";
        fout << w.right << " " << w.left << " " << w.ex << " " << w.spd << std::endl;
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
        table->setItem(i, 2, new QTableWidgetItem(tkg::strf("%+.1f", node[i].pos.x).c_str()));
        table->setItem(i, 3, new QTableWidgetItem(tkg::strf("%+.1f", node[i].pos.y).c_str()));
        table->setItem(i, 4, new QTableWidgetItem(tkg::strf("%+.2f", node[i].right).c_str()));
        table->setItem(i, 5, new QTableWidgetItem(tkg::strf("%+.2f", node[i].left ).c_str()));
        table->setItem(i, 6, new QTableWidgetItem(tkg::strf("%+.2f", node[i].ex   ).c_str()));
        table->setItem(i, 7, new QTableWidgetItem(tkg::strf("%+.2f", node[i].spd  ).c_str()));
    }

}

void RouteEdit::readTable(QTableWidget *table)
{
    for(uint i=0; i<table->rowCount(); i++)
    {
        WayPoint w;
        QByteArray old_str[8], new_str[8];

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
        old_str[2] = tkg::strf("%+.1f", node[i].pos.x).c_str();
        old_str[3] = tkg::strf("%+.1f", node[i].pos.y).c_str();
        old_str[4] = tkg::strf("%+.2f", node[i].right).c_str();
        old_str[5] = tkg::strf("%+.2f", node[i].left ).c_str();
        old_str[6] = tkg::strf("%+.2f", node[i].ex   ).c_str();
        old_str[7] = tkg::strf("%+.2f", node[i].spd  ).c_str();

        node[i].flag  = table->item(i,1)->text().toAscii().at(0);
        node[i].pos.x = table->item(i,2)->text().toAscii().toDouble();
        node[i].pos.y = table->item(i,3)->text().toAscii().toDouble();
        node[i].right = table->item(i,4)->text().toAscii().toDouble();
        node[i].left  = table->item(i,5)->text().toAscii().toDouble();
        node[i].ex    = table->item(i,6)->text().toAscii().toDouble();
        node[i].spd   = table->item(i,7)->text().toAscii().toDouble();

        new_str[1] = tkg::strf("%c",    node[i].flag ).c_str();
        new_str[2] = tkg::strf("%+.1f", node[i].pos.x).c_str();
        new_str[3] = tkg::strf("%+.1f", node[i].pos.y).c_str();
        new_str[4] = tkg::strf("%+.2f", node[i].right).c_str();
        new_str[5] = tkg::strf("%+.2f", node[i].left ).c_str();
        new_str[6] = tkg::strf("%+.2f", node[i].ex   ).c_str();
        new_str[7] = tkg::strf("%+.2f", node[i].spd  ).c_str();

        bool changed = false;
        for(int j=1; j<8; j++)
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

    for(uint i=0; i+1<node.size(); i++)
    {
        if( node[i].flag != 'C') continue;

        tkg::Point3 v,w;
        v = node[i+1].pos - node[i].pos;
        v = v.unit();
        w = v.rotateZ(tkg::pi/2);

        std::vector<tkg::Point3> outer = outer_box(i);
        glColor4dv(outer_color.rgba);
        glBegin(GL_LINE_LOOP);
        for(int j=0; j<4; j++) tkg::glVertex(outer[j]);
        glEnd();

        std::vector<tkg::Point3> inner = inner_box(i);
        glColor4dv(inner_color.rgba);
        glBegin(GL_LINE_LOOP);
        for(int j=0; j<4; j++) tkg::glVertex(inner[j]);
        glEnd();
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
    return (node_select != -1);
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
    node_select = -1;
    edge_select = -1;

    if(tmplog.type != NONE)
    {
        log.push_back(tmplog);
        tmplog.type = NONE;
    }
}

void RouteEdit::set(double x, double y, double r)
{
    node_select = -1;
    edge_select = -1;

    tkg::Point3 pos(x,y,0);
    double mindist = tkg::inf;

    for(uint i=0; i<node.size(); i++)
    {
        double dist = (node[i].pos - pos).abs();
        if( dist > r) continue;
        if( dist < mindist )
        {
            node_select = i;
            mindist     = dist;
        }
    }

    if(0 <= node_select && node_select < node.size())
    {
        tmplog.type   = MOVE;
        tmplog.select = node_select;
        tmplog.pos    = node[node_select];
        return;
    }

    for(uint i=0; i+1<node.size(); i++)
    {
        std::vector<tkg::Point3> box = inner_box(i);
        for(int j=0; j<3; j++)
        {
            tkg::Point3 u = box[j+1] - box[j];
            tkg::Point3 v =      pos - box[j];

            double dot = ( u.x * v.x + u.y * v.y ) / (u.abs() * u.abs());
            if(dot < 0.0 || 1.0 < dot) continue;

            double dist = abs( u.x * v.y - u.y * v.x ) / u.abs();
            if( dist > r) continue;
            if( dist < mindist )
            {
                edge_select = i * 10 + j;
                mindist     = dist;
            }
        }
    }

    if(0 <= (edge_select/10) && (edge_select/10)+1 < node.size())
    {
        tmplog.type   = MOVE;
        tmplog.select = edge_select/10;
        tmplog.pos    = node[edge_select/10];
        return;
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
        w.flag  = node[target-1].flag;
        w.pos   = pos;
        w.right = node[target-1].right;
        w.left  = node[target-1].left;
        w.ex    = node[target-1].ex;
        w.spd   = node[target-1].spd;
        node.insert(node.begin() + target, w);

        update = true; return;
    }

    // push mode -------------------------------

    tmplog.type = PUSH;
    log.push_back( tmplog );
    tmplog.type = NONE;

    WayPoint w;
    w.flag  = 'A';
    w.pos   = pos;
    w.right = 2.0;
    w.left  = 2.0;
    w.ex    = 0.0;
    w.spd   = 0.7;

    node.push_back( w );
    update = true; return;

    // -----------------------------------------
}

void RouteEdit::move(double x, double y)
{
    if(0 <= node_select && node_select < node.size())
    {
        node[node_select].pos = tkg::Point3(x,y,0);
        update = true;
    }

    if(0 <= (edge_select/10) && (edge_select/10)+1 < node.size())
    {
        int i = edge_select / 10;

        tkg::Point3 pos(x,y,0);
        tkg::Point3 u = node[i].pos - node[i+1].pos;
        tkg::Point3 v =         pos - node[i+1].pos;

        if(edge_select % 10 == 1) u = u.rotateZ(tkg::pi/2);

        double dist = (u.x * v.y - u.y * v.x) / u.abs();
        switch(edge_select % 10)
        {
            case 0: node[i].left  = std::max(0.0, -dist); break;
            case 1: node[i].ex    = std::max(0.0,  dist); break;
            case 2: node[i].right = std::max(0.0,  dist); break;
        }
        update = true;
    }
}

void RouteEdit::change(double s)
{
    if(node_select<0 || node.size()<=node_select) return;

    node[node_select].spd += s;
    if(node[node_select].spd < 0) node[node_select].spd = 0;

    update = true;
}
