#include <cstring>
#include "ssmviewer.hpp"
#include "tkg-geometry-gl.hpp"
using namespace tkg;


void Stream::init(const char* name, int id)
{
    ssm_id = id;
    strncpy(ssm_name, name, 64);
}

bool Stream::bind(SSMApiBase *a)
{
    api = a;
}


SSMViewer::SSMViewer()
{
    ssm = initSSM();
    if(ssm)
    {
        glpos.open("spur_odometry", 0);

        laser[0].open("sokuiki_fs", 3);
        laser[1].open("sokuiki_fs", 4);
    }


}

SSMViewer::~SSMViewer()
{
    if(ssm)
    {
        glpos.close();
        laser[0].close();
        laser[1].close();
        endSSM();
    }
}

void SSMViewer::draw()
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


    for(int s=0; s<2; s++)
    {
        // StreamBaseでreadNewが成功しているか判定する変数を用意する
        laser[s].readNew();
        glpos.readTime(laser[s].time);
        double robot_theta = glpos.data.theta;
        robot_pos = point3(glpos.data.x, glpos.data.y, 0);

        if(s) glColor3d(0,1,0); else glColor3d(0,1,1);
        glPointSize(3);
        glBegin(GL_POINTS);
        for(int i=0; i<laser[s].data.numPoints(); i++)
        {
            if(laser[s].data[i].isWarning()) continue;

            point3 ref(laser[s].data[i].reflect.vec);
            ref.rotZ(robot_theta);
            ggVertex( robot_pos + ref );
        }
        glEnd();
        glPointSize(1);

        if(s) glColor3d(0,0.5,0); else glColor3d(0,0.5,0.5);
        glLineWidth(1);
        glBegin(GL_LINES);
        for(int i=0; i<laser[s].data.numPoints(); i++)
        {
            if(laser[s].data[i].isWarning()) continue;

            point3 ori(laser[s].data[i].origin.vec);
            point3 ref(laser[s].data[i].reflect.vec);
            ori.rotZ(robot_theta);
            ref.rotZ(robot_theta);
            ggVertex( robot_pos + ori );
            ggVertex( robot_pos + ref );
        }
        glEnd();
        glPointSize(1);
    }
}
