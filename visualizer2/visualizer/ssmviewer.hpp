#ifndef SSMVIEWER_HPP
#define SSMVIEWER_HPP

#include "ssm-laser.hpp"
#include <ssmtype/spur-odometry.h>

/*
 ssmApi以外の変数をこちらにいこうして同一視できるようにする。
 →メニューのコールバックに引数が指定できないため無理か？
 */

class Stream
{
    public:

        bool view_state;
        int  ssm_id;
        char ssm_name[64];
        SSMApiBase *api;

        void init(const char* name, int id);
        bool bind(SSMApiBase *a);
};


class SSMViewer
{
    static const int MAX_LASER_STREAM = 4;

    public:

         SSMViewer();
        ~SSMViewer();

         void draw();

    private:

        // SSM
        int ssm;
        Stream test;

        SSMApi<Spur_Odometry> glpos;
        SSMSOKUIKIData3D      laser[MAX_LASER_STREAM];
};


#endif // SSMVIEWER_HPP
