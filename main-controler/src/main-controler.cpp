//-------------------------------------------------
//main-controler.cpp
//YSD
//since: 2013-10-10
//-------------------------------------------------

//kore ha noroi no puroguramu desu.

//c, c++
#include <stdio.h>
#include <iostream>
//#include<csignal>
#include<ctime>
#include<vector>
//#include<cmath>


//roboken lib
#include <ssm.hpp>
#include <ypspur.h>

//ssmtype
#include<ssmtype/spur-odometry.h>
#include"ssm-laser.hpp"

//ysdlib
//#include "ysd-color-detector.hpp"
#include "ysd-time.hpp"
#include "ysd-signal.hpp"

//gndlib
#include "gnd-timer.hpp"

//headers
#include "main-controler-config.hpp"
#include "main-controler-opt.hpp"
#include "main-controler.hpp"

using namespace std;

//-----------------------
//configuration parameter
//-----------------------
main_controler::proc_configuration conf;

//-----------------------
//route
//-----------------------
vector<main_controler::waypoint> keiro;

//-----------------------
//SSM data
//-----------------------
bool ssm_isopen = false;
SSMApi<Spur_Odometry> ssm_odom; //odometry
SSMSOKUIKIData3D sokuiki_fs;

inline void finalize_ssm()
{
    std::cerr << "\n";

    if( ssm_odom.isOpen() )
    {
        ssm_odom.close();
        std::cerr << "ssm-data " << ssm_odom.getStreamName() << " closed.\n";
    }

    if( sokuiki_fs.isOpen() )
    {
        sokuiki_fs.close();
        std::cerr << "ssm-data " << sokuiki_fs.getStreamName() << "closed.\n";
    }

    if( ssm_isopen )
    {
        endSSM();
        ssm_isopen = false;
        std::cerr << "ssm closed.\n";
    }
}

namespace main_controler
{   // ---> namespace main_controler

class pos
{
public:
    double x;
    double y;
    double th;

};

class robot_status
    {
    public:
        int dest_wp_num;                    //次のwp#  上層部が書き込む

        vector<main_controler::pos> dest_pos;
        int dest_pos_num;

        gnd::timer::stopwatch waiting_stop_watch;   //pre-crash用のstop watch
        bool is_waiting_timer_started;

        double x_over_line;
        double y_over_line;
        double over_line_mergin;

        bool is_searching;          // <-write:master / read:master | true: 探索エリア / false: 通常走行
        bool is_avoiding;           // <-write:master / read:master | true: 回避動作中 / false: 通常
        bool is_foward_target;      // <-write:master / read:master | true: 探索対象に向かってる / false: 通常

        bool enable_avoiding;       // <-write:master / read:master |
        bool enable_seemless;       // <-write:master / read:master |

        bool is_arrival;            // <-write:slave, (master) / read:master | true: pos_arrayの示す位置姿勢に到達した / false: まだだよ
        bool is_precrash_stopping;  // <-write:slave, (master) / read:master | true: 衝突回避停止中 / false: 通常

        //slaveにたいする命令
    private:
        bool _cue_spin;           // <-write:master, slave / read:slave | true: Spur_spin中 / false: Spur_line中
    public:
        bool cue_spin(void){return _cue_spin;}
        bool cue_line(void){return !_cue_spin;}
        void set_cue_spin(void){_cue_spin = true;}
        void set_cue_line(void){_cue_spin = false;}
        bool cue_stop;           // <-write:master, slave / read:slave | true: Spur_stop中 / false: 通常
//        bool cue_free;               // <-write:master, slave / read:slave | true: Spur_free中 / false: 通常

        int avoiding_pahse;         // <-write:slave, (master) / read:master | 回避動作のどの段階にるか
        int foward_target_phase;    // <-write:slave, (master) / read:master | 探索対象に近づく動作のどの段階にいるか

        void print_dest_wp(void)
        {
            fprintf(stdout, "[%3d]: ( %lf, %lf )\n", this->dest_wp_num, keiro[this->dest_wp_num].x, keiro[this->dest_wp_num].y);
        }

        void print_dest_pos(int i)
        {
            fprintf(stdout, "[%2d]: ( %lf, %lf, %lf )\n", i, this->dest_pos[i].x, this->dest_pos[i].y, this->dest_pos[i].th);
        }

        void set_over_line_pos(int i)
        {
            this->x_over_line = this->dest_pos[i].x - this->over_line_mergin*cos(this->dest_pos[i].th);
            this->y_over_line = this->dest_pos[i].y - this->over_line_mergin*sin(this->dest_pos[i].th);
        }

};

}   // <--- namespace main_controler

bool is_safety(void)
{
    sokuiki_fs.readLast();
    for(int i=0; i < (int)sokuiki_fs.property.numPoints; i++)
    {
        if(sokuiki_fs.data[i].status == ssm::laser::STATUS_NO_REFLECTION) continue ;
        if(sokuiki_fs.data[i].isWarning() == false
                && sokuiki_fs.data[i].reflect.x > 0.24
                && sokuiki_fs.data[i].reflect.x < conf.pre_crash_safety_megin_x.value
                && fabs(sokuiki_fs.data[i].reflect.y) < conf.pre_crash_safety_megin_y.value)
        {
            return true;
        }
    }
    return false;
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

inline void wait_restart_key(void)
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




int main( int argc, char* argv[] )
{
    //=================================================
    // ---> declaration

    //-----------------------
    //robot status
    //-----------------------
    main_controler::robot_status robot_status;

    // <--- declaration






    //=================================================
    // ---> initialize
    printf("Initialize Start...\n");
    //-----------------------
    //configuration parameter initialize
    //-----------------------
    main_controler::proc_conf_initialize(&conf);

    //-----------------------
    //read option and set configuration parameter
    //-----------------------
    if( main_controler::read_option( argc , argv , &conf ) != 0 ){ return -1; }

    //-----------------------
    //read route data file
    //-----------------------
    if( main_controler::read_route(conf.route_data_path.value, &keiro) )
    {
        fprintf(stderr, "ERROR : cannot read route file : %s\n", conf.route_data_path.value);
        return -1;
    }

    cerr << keiro.size() << " waypoints" << "\n";
    for(unsigned int i = 0; i < keiro.size(); ++i)
    {
        fprintf(stdout, "[%3d] ", i);
        keiro[i].print();
        fprintf(stdout, "\n");
    }


    //-----------------------
    //SSM initialize
    //-----------------------
    std::cerr << "initializing ssm ... ";
    if( !initSSM() )
    {
        fprintf(stderr, "ERROR : initSSM()\n");
        return -1;
    }
    else
    {
        std::cerr << "OK.\n";
        ssm_isopen = true;
    }

    //-----------------------
    //SSM open data
    //-----------------------
    std::cerr << "initializing ssm-data " << conf.odometry_name.value << "... ";
    if( !ssm_odom.openWait(conf.odometry_name.value, conf.odometry_id.value, 0.0, SSM_READ) )
    {
        fprintf(stderr, "ERROR : SSM open odometry : %s : ID %d\n", conf.odometry_name.value, conf.odometry_id.value);
        return -1;
    }
    else
    {
        std::cerr << "OK.\n";
    }

    std::cerr << "initializing ssm-data " << conf.sokuiki_fs_name.value << "... ";
    if( !sokuiki_fs.openWait(conf.sokuiki_fs_name.value, conf.sokuiki_fs_id.value, 0.0, SSM_READ) )
    {
        fprintf(stderr, "ERROR : SSM open sokuiki fs : %s : ID %d\n", conf.sokuiki_fs_name.value, conf.sokuiki_fs_id.value);
        return -1;
    }
    else if( !sokuiki_fs.getProperty() )
    {
        fprintf(stderr, "ERROR : SSM get property sokuiki fs : %s : ID %d\n", conf.sokuiki_fs_name.value, conf.sokuiki_fs_id.value);
        return -1;
    }
    else
    {
        std::cerr << "OK.\n";
    }

    //-----------------------
    //spur initialize
    //-----------------------
    cerr << "initializing yp-spur...";
    if( !Spur_init() )
    {
        fprintf(stderr, "ERROR : Spur_init()\n");
        return -1;
    }
    else
    {
        std::cerr << "OK.\n";
    }

    //-----------------------
    //set spur parameter
    //-----------------------
    Spur_set_vel(conf.ypspur_max_vel.value);
    Spur_set_accel(conf.ypspur_max_accel.value);
    Spur_set_angvel(conf.ypspur_max_ang_vel.value);
    Spur_set_angaccel(conf.ypspur_max_ang_accel.value);


    //-----------------------
    //set interval timer console
    //-----------------------
    gnd::timer::interval_timer timer_operation;
    gnd::timer::interval_timer timer_console;


    //-----------------------
    //set Signal handler
    //-----------------------
    ysd::setSigInt();


    //-----------------------
    //robot_status の更新（！現在地を通常のwpとする！）
    //-----------------------
    robot_status.dest_wp_num = conf.start_waypoint.value;
    if(robot_status.dest_wp_num > 0)    //途中実行の場合、initil_posをssmに流す
    {

    }

    robot_status.dest_wp_num++;

    robot_status.is_searching = false;
    robot_status.is_avoiding = false;
    robot_status.is_foward_target = false;

    robot_status.enable_avoiding = true;
    if(keiro[conf.start_waypoint.value].isSearchPoint() == true)
    {
        robot_status.enable_seemless = false;
    }
    else
    {
        robot_status.enable_seemless = conf.seemless_mode.value;
    }

    robot_status.is_arrival = false;
    robot_status.is_precrash_stopping = false;

    robot_status.set_cue_spin();    //spin->line->spin->line...の順番になる
    robot_status.cue_stop = false;

    robot_status.avoiding_pahse = 0;
    robot_status.foward_target_phase = 0;

    //次のwpをdest_pos_arrayに登録
    robot_status.dest_pos.clear();  //いままでの経由地点を消去
    main_controler::pos buff;

    buff.x = keiro[robot_status.dest_wp_num].x;
    buff.y = keiro[robot_status.dest_wp_num].y;
    buff.th = line2angle( keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                          keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num-1].y );
    robot_status.dest_pos.push_back(buff);

    //-----------------------
    //robot_status.dest_posをdest_pos_array[0]に更新
    robot_status.dest_pos_num = 0;

//    robot_status.over_line_mergin = 0.2;    //あかん
    if(robot_status.enable_seemless == true)
    {
        robot_status.over_line_mergin =   keiro[robot_status.dest_wp_num-1].velocity
                / ( conf.ypspur_max_ang_vel.value * tan(0.5 * (M_PI - th_b2ls(keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                                                                      keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y,
                                                                      keiro[robot_status.dest_wp_num+1].x, keiro[robot_status.dest_wp_num+1].y) ) ) );
        cerr << "mergin " << robot_status.over_line_mergin << "\n";
        cerr << "th_bl2 "
                << rad2deg(M_PI - th_b2ls(keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y,
                keiro[robot_status.dest_wp_num+1].x, keiro[robot_status.dest_wp_num+1].y) )
                << "\n";
    }
    else
    {
        robot_status.over_line_mergin = conf.spur_over_line_mergin.value;
    }
    robot_status.set_over_line_pos(robot_status.dest_pos_num);

    cerr << "dest_wp\n";
    robot_status.print_dest_wp();
    cerr << "dest_pos\n";
    robot_status.print_dest_pos(robot_status.dest_pos_num);
    cerr << "robot_status.cue_line()" << robot_status.cue_line() << endl;
    cerr << "robot_status.cue_spin()" << robot_status.cue_spin() << endl;
    fprintf(stderr, "over_line (%lf, %lf)\n", robot_status.x_over_line, robot_status.y_over_line);

    //ypspur_vel 更新
    Spur_set_vel(keiro[robot_status.dest_wp_num-1].velocity);

    printf("Initialize Finished.\n");
    // <--- initialize



    bool verbose = true;


    //=================================================
    // ---> operation
    timer_console.begin( CLOCK_REALTIME , 0.5 , -1.0 );
    timer_operation.begin( CLOCK_REALTIME , 0.005 , -1.0 );
    printf("Operation Start...\n");
    Spur_stop();
//    Spur_free();
    wait_restart_key();
    cout << "\n\n";
    while( !ysd::gShutOff )
    {   // ---> main loop
//        ysd::time::interval_timerSSM(1.0);

        //-----------------------
        //interval timer
        if( !timer_operation.clock() )
        {
//            cerr << "wait\n";
            timer_operation.wait();
        }

        sokuiki_fs.readLast();
        ssm_odom.readLast();

        //-----------------------
        //write console
        if( timer_console.clock() )
        {
            if( !verbose )
            {
                fprintf(stdout, "\033[2A");
                fprintf(stdout, "\033[2K -------- \033[33m\033[1m%s\033[39m\033[0m --------\n", main_controler::proc_name);
            }

//            fprintf(stdout, "\033[2K time : %lf\n", ysd::time::gettimeofday_sec());

        }


        //-----------------------
        //状態取得
        //slave。動作実行とリンクしてる
        if(robot_status.cue_line() == true)   //line中
        {
            if(is_safety() == true)   //pre_crash
            {
                Spur_stop();
                if(robot_status.is_waiting_timer_started == false)
                {
                    robot_status.waiting_stop_watch.begin(CLOCK_REALTIME);
                    robot_status.is_waiting_timer_started = true;
                }
                robot_status.is_precrash_stopping = true;
            }
            else
            {
                if(Spur_over_line_GL(robot_status.x_over_line, robot_status.y_over_line, robot_status.dest_pos[robot_status.dest_pos_num].th) == true) //とうたつ
                {
                    //Spur_stop?
                    cerr << "over line\n";  //debug
                    //dest_pos_arrayをみて最後なら上層部に到達を報告
                    if( robot_status.dest_pos_num+1 == (int)robot_status.dest_pos.size() )
                    {
                        cerr << "arrival at over line\n";   //debug
                        robot_status.is_arrival = true;
                    }
                    else    //dest_pos_arrayをみてまだ先があるならspinを実行
                    {
                        cerr << "go to spin at over line\n";    //debug
                        robot_status.dest_pos_num++;

                        robot_status.set_over_line_pos(robot_status.dest_pos_num);

                        if(robot_status.enable_seemless == true)
                        {   //ここでこの場合になることはない
                            robot_status.set_cue_line();   //そのままlineコマンドを発行
                        }
                        else
                        {
                            robot_status.set_cue_spin();
                        }

                    }


                }
            }
        }
//        cerr << "!\n";
        if(robot_status.cue_spin() == true)    //spin中
        {
            if( Spur_near_ang_GL( robot_status.dest_pos[robot_status.dest_pos_num].th, deg2rad(conf.Ypspur_near_ang_range.value) ) == true )
            {
                //dest_pos_arrayはそのままで、直進する
                robot_status.set_cue_line();
            }
        }





        //-----------------------
        //行動決定・動作計画
        //master
        if(robot_status.is_precrash_stopping == true)   //ぷりクラッシュで停止してる場合
        {
            robot_status.cue_stop = true;

            cerr << "pre_crash!!!!!!!!!!!!!!!!!!!!!!!\n";   //debug
            //is_safety()を絶えずチェック
            if(is_safety() == false)    //安全が確認さえｒた場合
            {
                robot_status.cue_stop = false;
                robot_status.is_precrash_stopping = false;
                cerr << "saikai\n"; //debug

                if(robot_status.enable_avoiding == true)    //回避を許可している場合
                {
                    robot_status.waiting_stop_watch.end();
                    robot_status.is_waiting_timer_started = false;
                }
            }
            else
            {   //まだなんかいる場合
                if(robot_status.enable_avoiding == true)    //回避を許可している場合
                {
                    double waiting_time,lap;
                    robot_status.waiting_stop_watch.get(&waiting_time, &lap);
                    if(waiting_time > conf.stop_waiting_time.value)   //ストップウォッチ計測->一定時間経過していたら衝突回避スタート
                    {

                    }
                }
                else
                {
                }
            }
        }
        else if(robot_status.is_arrival == true)
        {
            cerr << "arriaval\n";   //debug

            if(    keiro[robot_status.dest_wp_num].isGoalPoint() == true
                || robot_status.dest_wp_num+1 == (int)keiro.size() )
            {
                fprintf(stdout, "GOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOAL!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11\n");
                break;  //勝利のbreak
            }

            if(robot_status.is_avoiding == true)
            {
                //衝突回避中
                //avoiding_phaseにしたがってしょりをかえる

            }
            //else ifでもよい?
            if(robot_status.is_foward_target == true)
            {
                //探索対象に向かってる
                //foward_target_phaseにしたがってしょりをかえる

            }
            else
            {
                //wp到着
                //wp更新
                ++robot_status.dest_wp_num;
                robot_status.is_searching = keiro[robot_status.dest_wp_num-1].isSearchPoint();

                if(robot_status.is_searching == true)
                {
                    //探索開始
                    //human-detectorとtarget-recognizerを操作




                    //探索対象に向かう経路を計画







                }
                else
                {
                    //通常走行中
                    //次のwpに向かう                    

                    //-----------------------
                    //次のwpをdest_pos_arrayに登録
                    robot_status.dest_pos.clear();  //いままでの経由地点を消去
//                    dest_pos_array.clear(); //いままでの経由地点を消去

                    main_controler::pos buff;

                    cerr << "robot_status.dest_wp_num " << robot_status.dest_wp_num << "\n";    //debug

                    buff.x = keiro[robot_status.dest_wp_num].x;
                    buff.y = keiro[robot_status.dest_wp_num].y;
                    buff.th = line2angle( keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                                          keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y );
                    robot_status.dest_pos.push_back(buff);

                    robot_status.enable_seemless = conf.seemless_mode.value;

                    //-----------------------
                    //robot_status.dest_posをdest_pos_array[0]に更新
                    robot_status.dest_pos_num = 0;
                    if(robot_status.enable_seemless == true)
                    {
                        robot_status.over_line_mergin =   keiro[robot_status.dest_wp_num-1].velocity
                                / ( conf.ypspur_max_ang_vel.value * tan(0.5 * (M_PI - th_b2ls(keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                                                                                      keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y,
                                                                                      keiro[robot_status.dest_wp_num+1].x, keiro[robot_status.dest_wp_num+1].y) ) ) );
                        cerr << "mergin " << robot_status.over_line_mergin << "\n";
                        cerr << "th_bl2 "
                                << rad2deg(M_PI - th_b2ls(keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                                keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y,
                                keiro[robot_status.dest_wp_num+1].x, keiro[robot_status.dest_wp_num+1].y) )
                                << "\n";
                    }
                    else
                    {
                        robot_status.over_line_mergin = conf.spur_over_line_mergin.value;
                    }
                    robot_status.set_over_line_pos(robot_status.dest_pos_num);

                    //-----------------------
                    //robot_statusの各フラグを更新
                    robot_status.is_avoiding = false;
                    robot_status.is_foward_target = false;

                    robot_status.is_arrival = false;
                    robot_status.is_precrash_stopping = false;

                    if(robot_status.enable_seemless == true)
                    {
                        robot_status.set_cue_line();   //そのままlineコマンドを発行
                    }
                    else
                    {
                        robot_status.set_cue_spin();
                    }
                    robot_status.cue_stop = false;

                    robot_status.avoiding_pahse = 0;
                    robot_status.foward_target_phase = 0;

                    cerr << "dest_wp\n";
                    robot_status.print_dest_wp();
                    cerr << "dest_pos\n";
                    robot_status.print_dest_pos(robot_status.dest_pos_num);
                    cerr << "robot_status.cue_line()" << robot_status.cue_line() << endl;
                    cerr << "robot_status.cue_spin()" << robot_status.cue_spin() << endl;
                    fprintf(stderr, "over_line (%lf, %lf)\n", robot_status.x_over_line, robot_status.y_over_line);

                    //ypspur_vel 更新
                    Spur_set_vel(keiro[robot_status.dest_wp_num-1].velocity);

                }
            }
        }






        //-----------------------
        //動作実行
        //slave。状態取得とリンクしてる
        if(robot_status.cue_stop == true || robot_status.is_precrash_stopping == true)   //
        {
            Spur_stop();
        }
        else
        {
            if(robot_status.cue_line() == true)   //line
            {
//                cerr << "line\n"; //debug
                Spur_stop_line_GL(robot_status.dest_pos[robot_status.dest_pos_num].x, robot_status.dest_pos[robot_status.dest_pos_num].y, robot_status.dest_pos[robot_status.dest_pos_num].th);
            }
            else if(robot_status.cue_spin() == true)    //spin
            {
//                cerr << "spin\n"; //debug
                Spur_spin_GL(robot_status.dest_pos[robot_status.dest_pos_num].th);
            }
        }








    }// <--- main loop
    printf("Operation Finished.\n");
    // <--- operation






    //=================================================
    // ---> finalize
    printf("Finalize Start...\n");
    Spur_stop();

    finalize_ssm();

    Spur_free();

    printf("Finalize Finished.\n");
    // <--- finalize

    cout << "End Successfully" << endl;
    return 0;

}
// koko ha puroguramu no owari desu .
