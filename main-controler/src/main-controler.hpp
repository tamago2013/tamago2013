//-------------------------------------------------
//main-controler.hpp
//YSD
//since: 2013-10-11
//-------------------------------------------------


#include<stdio.h>
#include <iostream>
#include<vector>

#include"gnd-cui.hpp"


using namespace std;

namespace  main_controler
{ // ---> namespace main_controler

    static const unsigned char bitflag_way = 0x01;
    static const unsigned char bitflag_search = 0x02;
    static const unsigned char bitflag_goal = 0x04;
    static const unsigned char bitflag_adhoc_navigation = 0x08;
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

//        double detection_radius;
        double run_area_right_width;
        double run_area_left_width;
        double run_area_extention_length;

        double velocity;

//    public:
        waypoint(){}
        ~waypoint(){}
        waypoint(const waypoint &obj)
        {
            this->x = obj.x;
            this->y = obj.y;
            this->wp_type = obj.wp_type;

//            this->detection_radius = obj.detection_radius;
            this->run_area_right_width = obj.run_area_right_width;
            this->run_area_left_width = obj.run_area_left_width;
            this->run_area_extention_length = obj.run_area_extention_length;

            this->velocity = obj.velocity;
        }

        waypoint(double x, double y, unsigned char wp_type, /*double detection_radius*/
                 double run_area_right_width, double run_area_left_width, double run_area_extention_length,
                 double velocity)
        {
            this->x = x;
            this->y = y;
            this->wp_type = wp_type;
//            this->detection_radius = detection_radius;
            this->run_area_right_width = run_area_right_width;
            this->run_area_left_width = run_area_left_width;
            this->run_area_extention_length = run_area_extention_length;
            this->velocity = velocity;
        }

        void print()
        {
            fprintf(stdout, "( %+10.3f, %+10.3f ) way: %d, search: %d, goal: %d, detectin area:( %.3lf, %.3lf, %.3lf ), vel: %.3lf",
                    this->x, this->y, isWayPoint(), isSearchPoint(), isGoalPoint(),
//                    this->detection_radius,
                    this->run_area_right_width,
                    this->run_area_left_width,
                    this->run_area_extention_length,
                    this->velocity);
        }

        bool isWayPoint(){        return (wp_type & main_controler::bitflag_way); }
        bool isSearchPoint(){     return (wp_type & main_controler::bitflag_search); }
        bool isGoalPoint(){       return (wp_type & main_controler::bitflag_goal); }
        bool isAdhocNavigation(){ return (wp_type & main_controler::bitflag_adhoc_navigation); }

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
                ret = fscanf(fp, "%c %lf %lf %lf %lf %lf %lf\n",
                             &buff.wp_type, &buff.x, &buff.y,
//                             &buff.detection_radius,
                             &buff.run_area_right_width,
                             &buff.run_area_left_width,
                             &buff.run_area_extention_length,
                             &buff.velocity);

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


namespace main_controler
{   // ---> namespace main_controler

class pos
{
public:
    double x;
    double y;
    double th;

};
}


inline double deg2rad(double deg)
{
    return M_PI * deg / 180.0;
}

inline double rad2deg(double rad)
{
    return 180.0 * rad / M_PI;
}


inline double line2angle(double x1, double y1, double x2, double y2)
{
    return atan2( y2 - y1, x2 - x1 );
}


double th_b2ls(double x1, double y1, double x2, double y2, double x3, double y3)
{
    double ax = x2-x1;
    double ay = y2-y1;
    double bx = x3-x2;
    double by = y3-y2;
    double ax2 = ax*ax;
    double ay2 = ay*ay;
    double bx2 = bx*bx;
    double by2 = by*by;

    return acos( (ax*bx + ay*by) / sqrt(ax2*bx2 + ax2*by2 + ay2*bx2 + ay2*by2) );
}


void pos_GL2camera(const ysd::_rect *cluster_GL, ysd::_rect *cluster_camera, const Spur_Odometry *robot_pos)
{
    double sin_th = sin(robot_pos->theta);
    double cos_th = cos(robot_pos->theta);
    double x_offset = -0.10;    //カメラ位置

    cluster_camera->x1 = cos_th*(cluster_GL->x1 - robot_pos->x) + sin_th*(cluster_GL->y1 - robot_pos->y) + x_offset;
    cluster_camera->y1 = -1.0*sin_th*(cluster_GL->x1 - robot_pos->x) + cos_th*(cluster_GL->y1 - robot_pos->y);

    cluster_camera->x2 = cos_th*(cluster_GL->x2 - robot_pos->x) + sin_th*(cluster_GL->y2 - robot_pos->y) + x_offset;
    cluster_camera->y2 = -1.0*sin_th*(cluster_GL->x2 - robot_pos->x) + cos_th*(cluster_GL->y2 - robot_pos->y);

    cluster_camera->x_g = cos_th*(cluster_GL->x_g - robot_pos->x) + sin_th*(cluster_GL->y_g - robot_pos->y) + x_offset;
    cluster_camera->y_g = -1.0*sin_th*(cluster_GL->x_g - robot_pos->x) + cos_th*(cluster_GL->y_g - robot_pos->y);
}


void calc_subgoal_pos(double start_x, double start_y,
                      double finish_x, double finish_y,
                      double target_x, double target_y,
                      double *X, double *Y)
{
    //直線の傾きを求める
    double theta = atan2( finish_y - start_y , finish_x - start_x );

    //直線の方程式（ax+by+c = 0）の係数a,b,cを求める
    double a = +sin( theta );
    double b = -cos( theta );
    double c = - start_y * b - start_x * a;

    //ターゲットから直線におろした垂線と直線の交点を求める
    *X = target_x - 1/(a*a + b*b)*(a*a*target_x + a*b*target_y + a*c);
    *Y = target_y - 1/(a*a + b*b)*(a*b*target_x + b*b*target_y + b*c);
}


inline bool compare_cluster_pos(const ysd::_rect& left, const ysd::_rect& right)
{
    return left.x_g < right.x_g;
}


void wait_restart_key(void)
{
    printf("1.安全が確認された場合、Gキーを押してください\n");
    while(1){
        string key;
        //	qin>>key;
        cin>>key;
        cout << key << endl;
        if( key == "g" || key == "G" )
        {	//goサインが出た場合
            printf("安全が確認されたので動作を再開します\n");
            break;
        }
        else
        {
            printf("2.安全が確認された場合、Gキーを押してください\n");
        }
        usleep(5000);
    }
}

void my_stop(void){	//ロボットがそのばその姿勢でほぼ完全に停止する関数
    double v, w;

    Spur_stop();
    Spur_get_vel(&v, &w);
    while(fabs(v) > 0.01 || fabs(w) > 0.01 ){
        usleepSSM(5000);
        Spur_get_vel(&v, &w);
    }
}

//cui command list
const gnd::cui_command cui_cmd[] =
{
    {"Quit",        'Q',    "main-controler shut-off"},
    {"help",        'h',    "show help"},
    {"Free",        'F',    "go to free mode"},
    {"Restart", 	'S',    "Restart"},
    {"show-status", 's',    "show status"},
    {"", '\0'}
    //        {"show",		's',	"state show mode"},
    //        {"stand-by",	'B',	"operation stop and wait cui-command"},
    //        {"start",		'o',	"start operation"},
    //        {"wide",		'w',	"change wide sampling mode on/off"},
    //        {"debug-log",	'd',	"change debug mode on/off"},
};


