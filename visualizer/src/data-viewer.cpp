#include "data-viewer.hpp"
#include "window.hpp"
#include "ssm-laser.hpp"
#include "ssm-particles.hpp"
#include "ssm-cluster.hpp"
#include "ssm-ptz.hpp"
#include "ssmtype/spur-odometry.h"
#include "menu-handler.hpp"
#include "map-viewer.hpp"
#include "ssm-message.hpp"
#include "tkg-debug.hpp"




FieldViewer::FieldViewer(Window *window, tkg::ConfigGroup &conf)
{
    opsm = new MapViewer(conf["file"]);
    menu = new ToggleMenuHandler;
    menu->title = conf["title"];
    menu->value = conf["view-state"] == "true";
    window->addMenuView(menu);
}

FieldViewer::~FieldViewer()
{
    delete opsm;
    delete menu;
}

std::string FieldViewer::load()
{
    opsm->load();
    return ""; //opsm->message();
}

void FieldViewer::draw(const Robot &robot)
{
    if(opsm->good() && menu->value)
    {
        return opsm->draw();
    }

    double s = -30.0, g = 30.0;

    glColor3d(0.2,0.2,0.2);
    glLineWidth(1);
    glBegin(GL_LINES);
    for(double d=s; d<=g; d+=1.0)
    {
        tkg::Point3 sx(d, s, 0.0);
        tkg::Point3 gx(d, g, 0.0);
        tkg::Point3 sy(s, d, 0.0);
        tkg::Point3 gy(g, d, 0.0);

        tkg::glVertex(robot.pos + sx.rotateZ(robot.ang));
        tkg::glVertex(robot.pos + gx.rotateZ(robot.ang));
        tkg::glVertex(robot.pos + sy.rotateZ(robot.ang));
        tkg::glVertex(robot.pos + gy.rotateZ(robot.ang));
    }
    glEnd();
}




RouteViewer::RouteViewer(Window *window, tkg::ConfigGroup &conf)
{
    menu = new SelectMenuHandler;
    menu->title = conf["title"];
    menu->value = 1;
    menu->list.push_back( SelectMenuElement("non-display",     0) );
    menu->list.push_back( SelectMenuElement("route only"     , 1) );
    menu->list.push_back( SelectMenuElement("toute + number" , 3) );
    window->addMenuView(menu);

    file        = conf["file"];
    detect      = tkg::parseInt(conf["detect-length"]) / 1000.0;
    text_color  = tkg::Color4(conf["text-color"]);
    inner_color = tkg::Color4(conf["inner-color"]);
    outer_color = tkg::Color4(conf["outer-color"]);

    for(char i=0; i<26; i++)
    {
        node_color[i] = tkg::Color4(conf[std::string("node-color-") + (char)(i+'A')]);
        edge_color[i] = tkg::Color4(conf[std::string("edge-color-") + (char)(i+'A')]);
    }
}

RouteViewer::~RouteViewer()
{
    delete menu;
}

std::string RouteViewer::load()
{
    std::string   str;
    std::ifstream fin(file.c_str());

    if(!fin)
    {
        str += "ルートの読込に失敗しました。[" + file + "]\n";
        return str;
    }

    WayPoint w;
    while(fin >> w.flag, fin.good())
    {

        fin >> w.pos.x >> w.pos.y >> w.right >> w.left >> w.ex >> w.spd;
        node.push_back( w );
    }

    str += "ルートの読込に成功しました。\n";
    return str;
}

void RouteViewer::draw(double rotv, double roth)
{
    if( menu->value & 1 )
    {
        glPointSize(5);
        glBegin(GL_POINTS);
        for(uint i=0; i<node.size(); i++)
        {
            glColor4dv(node_color[node[i].flag-'A'].rgba);
            tkg::glVertex(node[i].pos);
        }
        glEnd();
        glPointSize(1);

        glLineWidth(1);
        glBegin(GL_LINES);
        for(uint i=1; i<node.size(); i++)
        {
            glColor4dv(edge_color[node[i-1].flag-'A'].rgba);
            tkg::glVertex(node[i-1].pos);
            tkg::glVertex(node[i  ].pos);
        }
        glEnd();

        glLineWidth(1);
        for(uint i=0; i<node.size(); i++)
        {
            if(node[i].flag != 'C') continue;

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
    }

    if( menu->value & 2 )
    {
        const tkg::Point3 height(0.0, 0.0, 1.0);

        glColor4dv(text_color.rgba);
        for(uint i=0; i<node.size(); i++)
        {
            tkg::glString(tkg::strf("%d",i), node[i].pos+height, 0.5, rotv, roth, true);
        }
    }
}

std::vector<tkg::Point3> RouteViewer::inner_box(int i)
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

std::vector<tkg::Point3> RouteViewer::outer_box(int i)
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



StreamViewer::StreamViewer(tkg::ConfigGroup &conf)
{
    ssm  = NULL;
    sync = conf["synchronize"] == "true";
}

StreamViewer::~StreamViewer()
{
    if(smState(ssm))
    {
        ssm->close();
    }
    delete ssm;
}





PositionViewer::PositionViewer(Window *window, tkg::ConfigGroup &conf) : StreamViewer(conf)
{
    ssm  = new SSMApi<Spur_Odometry> (tkg::parseStr(conf["ssm-name"]), tkg::parseInt(conf["ssm-id"]));
    menu = new ToggleMenuHandler;
    menu->title = conf["title"];
    menu->value = conf["view-state"] == "true";
    window->addMenuView(menu);

    now_color = tkg::Color4(conf["now-color"]);
    log_color = tkg::Color4(conf["log-color"]);
}

PositionViewer::~PositionViewer()
{
    delete menu;
}

void PositionViewer::draw()
{
    if( !smState(ssm) ) return;
    Spur_Odometry &data = *(Spur_Odometry*)ssm->data();

    // exec (not draw)
    robot = Robot( tkg::Point3(data.x, data.y), data.theta );

    if( menu->value )
    {
        glColor4dv(now_color.rgba);
        glArrow(robot.pos, robot.ang, 0.5);

        glColor4dv(log_color.rgba);
        for(uint i=0; i<history.size(); i++)
        {
            glArrow(history[i].pos, history[i].ang, 0.5);
        }
    }
}






PTZCameraViewer::PTZCameraViewer(Window *window, tkg::ConfigGroup &conf) : StreamViewer(conf)
{
    ssm  = new SSMApi<ysd::PTZ> (tkg::parseStr(conf["ssm-name"]), tkg::parseInt(conf["ssm-id"]));
    menu = new SelectMenuHandler;
    menu->title = conf["title"];
    menu->value = conf["view-state"] == "true";
    menu->list.push_back( SelectMenuElement("non-display",      0) );
    menu->list.push_back( SelectMenuElement("centerline"      , 1) );
    menu->list.push_back( SelectMenuElement("range of vision" , 2) );
    window->addMenuView(menu);

    color  = tkg::Color4(conf["color"]);
    height = tkg::parseInt(conf["height"]) / 1000.0;
}

PTZCameraViewer::~PTZCameraViewer()
{
    delete menu;
}

void PTZCameraViewer::draw()
{
    if( !smState(ssm) ) return;
    ysd::PTZ &data = *(ysd::PTZ*)ssm->data();

    if( menu->value )
    {
        const int dy[] = {+1, +1, -1, -1};
        const int dz[] = {+1, -1, -1, +1};
        double v = (-data.tilt     ) * tkg::pi / 180;
        double h = (-data.pan  - 90) * tkg::pi / 180;
        tkg::Point3 ori = tkg::Point3(0.0, 0.0, height);

        if( menu->value == 1 )
        {
            glColor4dv(color.rgba);
            glLineWidth(3);
            glBegin(GL_LINES);
            glVertex(robot.pos + ori);
            glVertex(robot.pos + ori + tkg::Point3(20.0, 0.0, 0.0).rotateY(v).rotateZ(robot.ang + h));
            glEnd();
        }

        if( menu->value == 2 )
        {
            glColor4dv(color.rgba);
            glLineWidth(1);
            glBegin(GL_LINES);
            for(int d=1; d<=4; d++)
            {
                double tx = 5.0 * d;
                double ty = tx / 2 / (data.zoom + 1000.0) * 1000.0;
                double tz = tx / 3 / (data.zoom + 1000.0) * 1000.0;

                tkg::Point3 pt[4];
                for(int i=0; i<4; i++)
                {
                    pt[i] = tkg::Point3(tx, dy[i]*ty, dz[i]*tz).rotateY(v).rotateZ(robot.ang + h);
                }

                for(int i=0; i<4; i++)
                {
                    tkg::glVertex( robot.pos + ori );
                    tkg::glVertex( robot.pos + ori + pt[i]);
                    tkg::glVertex( robot.pos + ori + pt[i]);
                    tkg::glVertex( robot.pos + ori + pt[(i+1)%4]);
                }
            }
            glEnd();
        }
    }
}






ClusterViewer::ClusterViewer(Window *window, tkg::ConfigGroup &conf) : StreamViewer(conf)
{
    ssm  = new SSMApi<ysd::cluster>(tkg::parseStr(conf["ssm-name"]), tkg::parseInt(conf["ssm-id"]));
    menu = new ToggleMenuHandler;
    menu->title = conf["title"];
    menu->value = conf["view-state"] == "true";
    window->addMenuView(menu);

    color  = tkg::Color4(conf["color"]);
    height = tkg::parseInt(conf["height"]) / 1000.0;
}

ClusterViewer::~ClusterViewer()
{
    delete menu;
}

void ClusterViewer::draw()
{
    if( !smState(ssm) ) return;
    ysd::cluster &data = *(ysd::cluster*)ssm->data();

    if( menu->value )
    {
        glLineWidth(3);
        glColor4dv(color.rgba);
//        glBegin(GL_LINES);
        for(int i=0; i<data.num_clusters(); i++)
        {
        glBegin(GL_LINE_LOOP);		//YSD
            glVertex3d(data.rect[i].x1, data.rect[i].y1, height);
            glVertex3d(data.rect[i].x1, data.rect[i].y2, height);
            glVertex3d(data.rect[i].x2, data.rect[i].y2, height);
            glVertex3d(data.rect[i].x2, data.rect[i].y1, height);
        glEnd();					//YSD
        }
//        glEnd();
    }
}





ParticleViewer::ParticleViewer(Window *window, tkg::ConfigGroup &conf) : StreamViewer(conf)
{
    ssm  = new SSMParticles(tkg::parseStr(conf["ssm-name"]), tkg::parseInt(conf["ssm-id"]));
    menu = new ToggleMenuHandler;
    menu->title = conf["title"];
    menu->value = conf["view-state"] == "true";
    window->addMenuView(menu);

    color = tkg::Color4(conf["color"]);
}

ParticleViewer::~ParticleViewer()
{
    delete menu;
}

void ParticleViewer::draw()
{
    if( !smState(ssm) ) return;
    particle_set_c &data = *(particle_set_c*)ssm->data();

    if( menu->value )
    {
        glColor4dv(color.rgba);
        for(uint i=0; i<data.size(); i++)
        {
            tkg::Point3 pos(data[i][0][PARTICLE_X], data[i][0][PARTICLE_Y], 0.0 );
            glArrow(pos, data[i][0][PARTICLE_THETA], 0.2);
        }
    }
}





LaserViewer::LaserViewer(Window *window, tkg::ConfigGroup &conf) : StreamViewer(conf)
{
    ssm  = new SSMSOKUIKIData3D(tkg::parseStr(conf["ssm-name"]), tkg::parseInt(conf["ssm-id"]));
    menu = new SelectMenuHandler;
    menu->title = conf["title"];
    menu->value  = 0;
    menu->value |= (conf["view-point"]=="true" ? 1 : 0);
    menu->value |= (conf["view-laser"]=="true" ? 2 : 0);
    menu->list.push_back( SelectMenuElement("non-display", 0) );
    menu->list.push_back( SelectMenuElement("point"      , 1) );
    menu->list.push_back( SelectMenuElement("laser"      , 2) );
    menu->list.push_back( SelectMenuElement("point+laser", 3) );
    window->addMenuView(menu);

    point_color = tkg::Color4(conf["point-color"]);
    laser_color = tkg::Color4(conf["laser-color"]);
}

LaserViewer::~LaserViewer()
{
    delete menu;
}

void LaserViewer::draw()
{
    if( !smState(ssm) ) return;
    ssm::SOKUIKIData3D &data = *(ssm::SOKUIKIData3D*)ssm->data();

    if(menu->value & 1)
    {
        glColor4dv(point_color.rgba);
        glPointSize(3);
        glBegin(GL_POINTS);

        for(uint i=0; i<data.numPoints(); i++)
        {
            if(data[i].isWarning()) continue;

            tkg::Point3 ref(data[i].reflect.vec);
            glVertex(robot.pos + ref.rotateZ(robot.ang));
        }
        glEnd();
        glPointSize(1);
    }

    if(menu->value & 2)
    {
        glColor4dv(laser_color.rgba);
        glLineWidth(1);
        glBegin(GL_LINES);
        for(uint i=0; i<data.numPoints(); i++)
        {
            if(data[i].isWarning()) continue;

            tkg::Point3 ori(data[i].origin.vec);
            tkg::Point3 ref(data[i].reflect.vec);
            glVertex(robot.pos + ori);
            glVertex(robot.pos + ref.rotateZ(robot.ang));
        }
        glEnd();
    }
}
