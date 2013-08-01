#include "stream.hpp"
#include "gl-geometry.hpp"
using namespace tkg;

StreamManager::StreamManager()
{
    ssm = initSSM();
    if(ssm)
    {
        glpos.open("spur_odometry", 0);

        laser[0].open("sokuiki_fs", 0);
        //laser[0].getProperty();
    }
}

StreamManager::~StreamManager()
{
    if(ssm)
    {
        glpos.close();
        laser[0].close();
        endSSM();
    }
}

void StreamManager::draw()
{
    if(!ssm) return;

    // exec
    glpos.readNew();

    point3 robot_pos(glpos.data.x, glpos.data.y, 0);
    // ----exec

    glColor3d(1,0,0);
    glLineWidth(3);
    ggDrawCross(robot_pos, glpos.data.theta, 1);
    glLineWidth(1);


    // StreamBaseでreadNewが成功しているか判定する変数を用意して
    laser[0].readNew();
    glpos.readTime(laser[0].time);
    double robot_theta = glpos.data.theta;
    robot_pos = point3(glpos.data.x, glpos.data.y, 0);

    glColor3d(0,1,0);
    glPointSize(3);
    glBegin(GL_POINTS);
    //for(int i=0; i<laser[0].data.numPoints(); i++)
    for(int i=0; i<laser[0].data.numPoints(); i++)
    {
        if(laser[0].data[i].isWarning()) continue;

        point3 ref(laser[0].data[i].reflect.vec);
        ref.rotZ(robot_theta);
        ggVertex( robot_pos + ref );

    }
    glEnd();
    glPointSize(1);
}
