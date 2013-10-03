#include "structure.hpp"
#include "ssm-laser.hpp"
#include "ssm-particles.hpp"
#include "ssm-cluster.hpp"
#include "ssm-ptz.hpp"
#include "ssmtype/spur-odometry.h"
#include "menu-handler.hpp"
#include "map-viewer.hpp"
#include "ssm-message.hpp"


#include "window.hpp"


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

void FieldViewer::draw()
{
    if(opsm->good() && menu->value)
    {
        return opsm->draw();
    }

    /*
    double s = -30.0, g = 30.0;

    glColor3d(0.2,0.2,0.2);
    glBegin(GL_LINES);
    for(double d=s; d<=g; d+=1.0)
    {
        tkg::Point3 sx(d, s, 0.0);
        tkg::Point3 gx(d, g, 0.0);
        tkg::Point3 sy(s, d, 0.0);
        tkg::Point3 gy(g, d, 0.0);

        glVertex3dv((position.robot.pos + sx.rotateZ(position.robot.ang)).vec);
        glVertex3dv((position.robot.pos + gx.rotateZ(position.robot.ang)).vec);
        glVertex3dv((position.robot.pos + sy.rotateZ(position.robot.ang)).vec);
        glVertex3dv((position.robot.pos + gy.rotateZ(position.robot.ang)).vec);
    }
    glEnd();
    */
}




RouteViewer::RouteViewer(Window *window, tkg::ConfigGroup &conf)
{
    menu = new ToggleMenuHandler;
    menu->title = conf["title"];
    menu->value = conf["view-state"] == "true";
    window->addMenuView(menu);

    file       = conf["file"];
    node_color = tkg::Color4(conf["node-color"]);
    edge_color = tkg::Color4(conf["edge-color"]);
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

    while(!fin.eof())
    {
        char c; double x,y;
        fin >> c >> x >> y;
        node.push_back( tkg::Point3(x,y,0) );
    }

    for(uint i=1; i<node.size(); i++)
    {
        edge.push_back( std::make_pair(i-1, i) );
    }

    str += "ルートの読込に成功しました。\n";
    return str;
}

void RouteViewer::draw()
{
    if( !menu->value ) return;

    glColor4dv(node_color.rgba);
    glPointSize(5);
    glBegin(GL_POINTS);
    for(uint i=0; i<node.size(); i++)
    {
        tkg::glVertex(node[i]);
    }
    glEnd();
    glPointSize(1);

    glColor4dv(edge_color.rgba);
    glLineWidth(1);
    glBegin(GL_LINES);
    for(uint i=0; i<edge.size(); i++)
    {
        tkg::glVertex(node[edge[i].first ]);
        tkg::glVertex(node[edge[i].second]);
    }
    glEnd();

    /*
    glColor3d(1.0, 1.0, 1.0);
    for(int i=0; i<route.size(); i++)
    {
        myString(route[i]+tkg::Point3(0,0,0.2), strprintf("%d",i).c_str());
    }
    */
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

    color = tkg::Color4(conf["color"]);
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

    glColor3d(1.0, 0.5, 0);
    glArrow(robot.pos, robot.ang, 0.5);

    /*
    glColor4d(1.0, 0.5, 0, 0.5);
    for(uint i=0; i<robot_log.size(); i++)
    {
        glArrow(robot_log[i].pos, robot_log[i].ang, 0.5);
    }
    */
}






PTZCameraViewer::PTZCameraViewer(Window *window, tkg::ConfigGroup &conf) : StreamViewer(conf)
{
    ssm  = new SSMApi<ysd::PTZ> (tkg::parseStr(conf["ssm-name"]), tkg::parseInt(conf["ssm-id"]));
    menu = new ToggleMenuHandler;
    menu->title = conf["title"];
    menu->value = conf["view-state"] == "true";
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
        double v = (-data.tilt + 90) * tkg::pi / 180;
        double h = (-data.pan  - 90) * tkg::pi / 180;
        tkg::Point3 ori = tkg::Point3(0.0, 0.0, height);
        tkg::Point3 dir = tkg::Point3::polar(30.0, v, h);

        glLineWidth(3);
        glBegin(GL_LINES);
        glVertex(robot.pos + ori);
        glVertex(robot.pos + ori + dir.rotateZ(robot.ang));
        glEnd();
    }
}






ClusterViewer::ClusterViewer(Window *window, tkg::ConfigGroup &conf) : StreamViewer(conf)
{
    ssm  = new SSMApi<ysd::cluster> (tkg::parseStr(conf["ssm-name"]), tkg::parseInt(conf["ssm-id"]));
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
        glBegin(GL_LINES);
        for(int i=0; i<data.num_clusters(); i++)
        {
            glVertex3d(data.rect[i].x1, data.rect[i].y1, height);
            glVertex3d(data.rect[i].x1, data.rect[i].y2, height);
            glVertex3d(data.rect[i].x2, data.rect[i].y2, height);
            glVertex3d(data.rect[i].x2, data.rect[i].y1, height);
        }
        glEnd();
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
