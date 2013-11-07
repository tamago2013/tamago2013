//-------------------------------------------------
//main-controler.hpp
//YSD
//since: 2013-10-11
//-------------------------------------------------


#include<stdio.h>
#include <iostream>
#include<vector>

using namespace std;

namespace  main_controler
{ // ---> namespace main_controler

    static const unsigned char bitflag_way = 0x01;
    static const unsigned char bitflag_search = 0x02;
    static const unsigned char bitflag_goal = 0x04;
    static const unsigned char bitflag_change_speed = 0x08;
    //ただの通過地点 A
    //探索地点 C
    //ゴール E
    //速度変更 の通過地点 I <-やらん
    //速度変更 探索地点 K　<-やらん
} // <--- namespace main_controler



namespace  main_controler
{ // ---> namespace main_controler

/*
x y wp_type detection_radius
*/
    class waypoint
    {
    public:
//    private:
        double x;
        double y;
        unsigned char wp_type;
        double detection_radius;
        double velocity;

//    public:
        waypoint(){}
        ~waypoint(){}
        waypoint(const waypoint &obj)
        {
            this->x = obj.x;
            this->y = obj.y;
            this->wp_type = obj.wp_type;
            this->detection_radius = obj.detection_radius;
            this->velocity = obj.velocity;
        }

        waypoint(double x, double y, unsigned char wp_type, double detection_radius/*, double velocity*/)
        {
            this->x = x;
            this->y = y;
            this->wp_type = wp_type;
            this->detection_radius = detection_radius;
            this->velocity = velocity;
        }

        void print()
        {
            fprintf(stdout, "( %+10.3f, %+10.3f ) way: %d, search: %d, goal: %d, r: %.3lf, vel: %.3lf",
                    this->x, this->y, isWayPoint(), isSearchPoint(), isGoalPoint(), this->detection_radius, this->velocity);
        }

        bool isWayPoint(){          return (wp_type & main_controler::bitflag_way);}
        bool isSearchPoint(){       return (wp_type & main_controler::bitflag_search);}
        bool isGoalPoint(){         return (wp_type & main_controler::bitflag_goal);}
//        bool isChangeSpeedPoint(){  return (wp_type & main_controler::bitflag_change_speed);}

    };


}// <--- namespace main_controler

namespace  main_controler
{ // ---> namespace main_controler

    int read_route(std::string route_file_path, vector<main_controler::waypoint> *keiro)
    {
        main_controler::waypoint buff;
        FILE *fp;
        if((fp = fopen(route_file_path.c_str(), "r")) == NULL)
        {
            fprintf(stderr, "ERROR : cannot open %s\n", route_file_path.c_str());	//経路ファイルがオープンできないときは異常終了
            return -1;
        }
        else
        {
            printf("route file opened : %s\n", route_file_path.c_str());
            int ret;
            while(1)
            {
                ret = fscanf(fp, "%c %lf %lf %lf %lf\n", &buff.wp_type, &buff.x, &buff.y, &buff.detection_radius, &buff.velocity);

                if(ret == EOF)
                {		//次の通過点がない(ゴール)であるかの判定
                    break;
                }
                keiro->push_back(buff);
            }
            fclose(fp);
            return 0;
        }
    }

} // <--- namespace main_controler

