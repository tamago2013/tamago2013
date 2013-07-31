#ifndef STREAM_H
#define STREAM_H

#include "ssm-laser.hpp"
#include <ssmtype/spur-odometry.h>

/*
 ssmApi以外の変数をこちらにいこうして同一視できるようにする。
 →メニューのコールバックに引数が指定できないため無理か？
class StreamBase
{
};
*/

class Stream : SSMApiBase
{
    public:

         Stream() { api = NULL; }
        ~Stream() { delete api; }


        SSMApiBase *api;
};

class StreamManager
{
    static const int MAX_LASER_STREAM = 4;

    public:

         StreamManager();
        ~StreamManager();

         void draw();

    private:

        // SSM
        int ssm;

        SSMApi<Spur_Odometry> glpos;
        SSMSOKUIKIData3D      laser[MAX_LASER_STREAM];
};


#endif // STREAM_H
