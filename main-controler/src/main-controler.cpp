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
#include"ssm-cluster.hpp"
#include"ssm-ptz.hpp"
#include"ssm-sound.hpp"

//ysdlib
//#include "ysd-color-detector.hpp"
#include "ysd-time.hpp"
#include "ysd-signal.hpp"

//gndlib
#include "gnd-timer.hpp"
#include"gnd-cui.hpp"

//headers
#include "main-controler-config.hpp"
#include "main-controler-opt.hpp"
#include "main-controler.hpp"
#include "target-recognizer.hpp"
#include "adhoc-navigation.hpp" // 2013-11-14 MTM追加

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
SSMApi<Spur_Odometry> ssm_odom_bs; //odometry(adjustされないやつ) 2013-11-14 MTM追加
SSMSOKUIKIData3D sokuiki_fs;
SSMApi<ysd::cluster> cluster;
SSMApi<ysd::PTZ> ptz;
SSMApi<SoundType> ssm_sound; //サウンドプレイヤに渡すやつ 2013-11-14 MTM追加

SSMApi<double> human_detector_start_cue;    //dataはdetectin radius
SSMApi<bool> ptz_move_start_cue;
SSMApi<Spur_Odometry>	ssm_initpos;	// start poisition


inline void finalize_ssm()
{
    std::cerr << "\n";

    if( ssm_odom_bs.isOpen() )
    {
        ssm_odom_bs.close();
        std::cerr << "ssm-data " << ssm_odom_bs.getStreamName() << " closed.\n";
    }

    if( ssm_sound.isOpen() )
    {
        ssm_sound.release();
        std::cerr << "ssm-data " << ssm_sound.release() << " released.\n";
    }

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

    if( cluster.isOpen() )
    {
        cluster.close();
        std::cerr << "ssm-data " << cluster.getStreamName() << "closed.\n";
    }

    if( ptz.isOpen() )
    {
        ptz.close();
        std::cerr << "ssm-data " << ptz.getStreamName() << "closed.\n";
    }

    human_detector_start_cue.release();
    ptz_move_start_cue.release();
    ssm_initpos.release();


    if( ssm_isopen )
    {
        endSSM();
        ssm_isopen = false;
        std::cerr << "ssm closed.\n";
    }
}


// precrash : false
// no problem : true
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
            return false;
        }
    }
    return true;
}

namespace main_controler
{   // ---> namespace main_controler

class robot_status
{
public:
    int dest_wp_num;                    //次のwp#  上層部が書き込む

    vector<main_controler::pos> dest_pos;
    int dest_pos_num;

    gnd::timer::stopwatch waiting_stop_watch;   //pre-crash用のstop watch
    gnd::inttimer waiting_stop_timer;
    bool is_waiting_timer_started;

    double x_over_line;
    double y_over_line;
    double over_line_mergin;

    bool is_searching;          // <-write:master / read:master | true: 探索エリア / false: 通常走行
    bool is_avoiding;           // <-write:master / read:master | true: 回避動作中 / false: 通常
    bool is_foward_target;      // <-write:master / read:master | true: 探索対象に向かってる / false: 通常

    bool is_adhoc;


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
    bool cue_free;               // <-write:master, slave / read:slave | true: Spur_free中 / false: 通常

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

bool is_blank(double max_x, double max_y, double min_x, double min_y)
{
    for(unsigned int i = 0; i < sokuiki_fs.property.numPoints; ++i)
    {
        if( sokuiki_fs.data[i].isWarning() == true) continue;
        if(   min_x < sokuiki_fs.data[i].reflect.x || sokuiki_fs.data[i].reflect.x < max_x
           || min_y < sokuiki_fs.data[i].reflect.y || sokuiki_fs.data[i].reflect.y < max_y )
        {
            return false;
        }
    }
    return true;
}



int main( int argc, char* argv[] )
{
    //=================================================
    // ---> declaration

    //-----------------------
    //robot status
    //-----------------------
    main_controler::robot_status robot_status;

    gnd::cui_reader gcui;
    double cuito = 0;
    bool show_st = true;
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

    if(conf.run_only_mode.value == true)
    {
        for(unsigned int i = 0; i < keiro.size(); ++i)
        {
            keiro[i].wp_type = 'A';
        }
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
    //SSM create data
    //-----------------------
    human_detector_start_cue.create("human_detector_start_cue", 0, 10.0, 1.0);
    ptz_move_start_cue.create("ptz_move_start_cue", 0, 10.0, 1.0);
    ssm_initpos.create("init-pos", 0, 1, 0.05);

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

    std::cerr << "initializing ssm-data " << "spur_odometry" << "... "; //2013-11-14 MTM追加
    if( !ssm_odom_bs.openWait("spur_odometry", 0, 0.0, SSM_READ) )
    {
        fprintf(stderr, "ERROR : SSM open odometry : %s : ID %d\n", "spur_odometry", 0);
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

    //    std::cerr << "initializing ssm-data " << conf.odometry_name.value << "... ";
    cerr << "initializing ssm-data cluster...";
    if( !cluster.openWait(SNAME_CLUSTER, 0, 0.0, SSM_READ) )
    {
        //        fprintf(stderr, "ERROR : SSM cluster : %s : ID %d\n", conf.odometry_name.value, conf.odometry_id.value);
        fprintf(stderr, "ERROR : SSM cluster \n");
        return -1;
    }
    else
    {
        std::cerr << "OK.\n";
    }

    //    std::cerr << "initializing ssm-data " << conf.odometry_name.value << "... ";
    cerr << "initializing ssm-data ptz...";
    if( !ptz.openWait(SNAME_PTZ, 0, 0.0, SSM_READ) )
    {
        //        fprintf(stderr, "ERROR : SSM cluster : %s : ID %d\n", conf.odometry_name.value, conf.odometry_id.value);
        fprintf(stderr, "ERROR : SSM ptz \n");
        return -1;
    }
    else
    {
        std::cerr << "OK.\n";
    }

    std::cerr << "initializing ssm-data " << SNAME_SOUND << "... "; //2013-11-14 MTM追加
    if( !ssm_sound.create( SNAME_SOUND, SID_SOUND, 1.0 , 0.2 ) )
    {
        fprintf(stderr, "ERROR : SSM create sound : %s : ID %d\n", SNAME_SOUND, SID_SOUND);
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
//    robot_status.waiting_stop_watch.begin(CLOCK_REALTIME);
//    robot_status.waiting_stop_watch.end();

    robot_status.waiting_stop_timer.begin(CLOCK_REALTIME, conf.stop_waiting_time.value, -0.0);
    sleep(1.0);
    robot_status.waiting_stop_timer.end();

    //-----------------------
    //set cui
    //-----------------------
    gcui.set_command(cui_cmd, sizeof(cui_cmd) / sizeof(cui_cmd[0]));


    //-----------------------
    //set Signal handler
    //-----------------------
    ysd::setSigInt();


    //-----------------------
    //robot_status の更新（！現在地を通常のwpとする！）
    //-----------------------
    robot_status.dest_wp_num = conf.start_waypoint.value;

    robot_status.dest_wp_num++;

    robot_status.is_searching = false;
    robot_status.is_avoiding = false;
    robot_status.is_foward_target = false;

    robot_status.is_waiting_timer_started = false;

    robot_status.enable_avoiding = false;
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
    robot_status.cue_free = false;
    robot_status.is_adhoc = false;


    robot_status.avoiding_pahse = 0;
    robot_status.foward_target_phase = 0;

    //次のwpをdest_pos_arrayに登録
    robot_status.dest_pos.clear();  //いままでの経由地点を消去
    main_controler::pos buff;

    buff.x = keiro[robot_status.dest_wp_num].x;
    buff.y = keiro[robot_status.dest_wp_num].y;
    buff.th = line2angle( keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
            keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y );
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
        // ---> debug
        cerr << "mergin " << robot_status.over_line_mergin << "\n";
        cerr << "th_bl2 "
             << rad2deg(M_PI - th_b2ls(keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y,
                keiro[robot_status.dest_wp_num+1].x, keiro[robot_status.dest_wp_num+1].y) )
                << "\n";
        // <--- debug
    }
    else
    {
        robot_status.over_line_mergin = conf.spur_over_line_mergin.value;
    }
    robot_status.set_over_line_pos(robot_status.dest_pos_num);

    // ---> debug
    cerr << "dest_wp\n";
    robot_status.print_dest_wp();
    cerr << "dest_pos\n";
    robot_status.print_dest_pos(robot_status.dest_pos_num);
    cerr << "robot_status.cue_line()" << robot_status.cue_line() << endl;
    cerr << "robot_status.cue_spin()" << robot_status.cue_spin() << endl;
    fprintf(stderr, "over_line (%lf, %lf)\n", robot_status.x_over_line, robot_status.y_over_line);
    // <--- debug

    //ypspur_vel 更新
    Spur_set_vel(keiro[robot_status.dest_wp_num-1].velocity);

    //init_pos initialize
    ssm_initpos.data.x = keiro[robot_status.dest_wp_num-1].x;
    ssm_initpos.data.y = keiro[robot_status.dest_wp_num-1].y;
    ssm_initpos.data.theta = robot_status.dest_pos[robot_status.dest_pos_num].th;
    ssm_initpos.data.v = 0;
    ssm_initpos.data.w = 0;
    ssm_initpos.write();

    fprintf(stderr, "initpos: (%lf, %lf, %lf pi)\n", ssm_initpos.data.x, ssm_initpos.data.y, ssm_initpos.data.theta / M_PI);


    printf("Initialize Finished.\n");
    // <--- initialize



    bool verbose = true;


    //=================================================
    // ---> operation
    timer_console.begin( CLOCK_REALTIME , 0.5 , -1.0 );
    timer_operation.begin( CLOCK_REALTIME , 0.005 , -1.0 );
    printf("Operation Start...\n");

    Spur_stop();
    Spur_free();

    ssm_sound.data = sound_system_ready;
    ssm_sound.write();

    wait_restart_key();

    sleepSSM(5.0);

    cout << "\n\n";
    while( !ysd::gShutOff )
    {   // ---> main loop

        //-----------------------
        //interval timer
        if( !timer_operation.clock() )
        {
            //            cerr << "wait\n";
            timer_operation.wait();
        }


        //-----------------------
        // ---> cui
        int cuival = 0;
        char cuiarg[512];

        ::memset(cuiarg, 0, sizeof(cuiarg));
        if( gcui.poll(&cuival, cuiarg, sizeof(cuiarg), cuito) > 0 )
        {
            if( show_st )
            {
                // if show status mode, quit show status mode
                show_st = false;
                ::fprintf(stderr, "-------------------- cui mode --------------------\n");
                cerr << "> ";
            }
            else
            {
                switch(cuival)
                {
                case 'F':   //go to free
                    cerr << "pressed 'F'\n";
                    //free フラグを立てる
                    cerr << "Free Mode\n";
                    Spur_free();
                    robot_status.cue_free = true;
                    break;

                case 'S':   //run restart
                    cerr << "pressed 'S'\n";
                    cerr << "Restart\n";
                    //restartフラグをたてる
                    robot_status.cue_free = false;
                    show_st = true;
                    break;

                case 's':   //show status
                    cerr << "pressed 's'\n";
                    cerr << "show status\n";
                    show_st = true;
                    break;

                case 'h':
                    cerr << "pressed 'h'\n";
                    cerr << "help\n";
                    gcui.show(stderr, "   ");
                    break;

                case 'Q':
                    cerr << "pressed 'Q'\n";
                    cerr << "QUit\n";
                    ysd::gShutOff = true;
                    //終了フラグをたてる
                    break;

                }
                cerr << "> ";
            }
        }
        // <--- cui




        //-----------------------
        // ---> write console
        if( timer_console.clock() && show_st == true)
        {
            //            if( !verbose )
            //            {
            //                fprintf(stdout, "\033[2A");
            //                fprintf(stdout, "\033[2K -------- \033[33m\033[1m%s\033[39m\033[0m --------\n", main_controler::proc_name);
            //            }
            //
//            fprintf(stdout, "\033[2K time : %lf\n", ysd::time::gettimeofday_sec());
        }
        // <--- write console





        //-----------------------
        //状態取得
        //slave。動作実行とリンクしてる
        sokuiki_fs.readLast();
        ssm_odom.readLast();
        ssm_odom_bs.readTime( sokuiki_fs.time );

        if(robot_status.is_adhoc == true)
        {

            if( adhoc_navigation::run( ssm_odom_bs , ssm_odom , sokuiki_fs , ssm_sound , keiro[robot_status.dest_wp_num-1].x , keiro[robot_status.dest_wp_num-1].y , keiro[robot_status.dest_wp_num].x , keiro[robot_status.dest_wp_num].y ) )
            {
                robot_status.is_adhoc = false;
                robot_status.is_arrival = true;
            }

            continue;
        }


        if(robot_status.cue_line() == true)   //line中
        {
            if(is_safety() == false)   //pre_crash
            {
                Spur_stop();

                if(robot_status.is_waiting_timer_started == false)
                {
//                    robot_status.waiting_stop_watch.begin(CLOCK_REALTIME);
                    robot_status.waiting_stop_timer.begin(CLOCK_REALTIME, conf.stop_waiting_time.value, -0.0);
                    robot_status.is_waiting_timer_started = true;
                    ssm_sound.data = sound_EM_stop;
                    ssm_sound.write();
//                    cerr << "\033[33m\033[1mstopwatch start @ slave\033[39m\033[0m\n";
                }
                else
                {
//                    cerr << "stopwatch already started @ slave\n";
                }
                robot_status.is_precrash_stopping = true;
            }
            else
            {
                if(Spur_over_line_GL( robot_status.x_over_line,
                                      robot_status.y_over_line,
                                      robot_status.dest_pos[robot_status.dest_pos_num].th) == true) //とうたつ
                {
                    //Spur_stop?
                    cerr << "over line\n";  //debug
                    ssm_sound.data = sound_wp;  //play sound
                    ssm_sound.write();

                    //dest_pos_arrayをみて最後なら上層部に到達を報告
                    if( robot_status.dest_pos_num+1 == (int)robot_status.dest_pos.size() )
                    {
                        cerr << "arrival at over line\n";   //debug
                        ssm_odom.readLast();
                        fprintf(stderr, "ssm_odom(%lf, %lf, %lf)\n", ssm_odom.data.x, ssm_odom.data.y, ssm_odom.data.theta);
                        fprintf(stderr, "dest_pos(%lf, %lf, %lf)\n", robot_status.dest_pos[robot_status.dest_pos_num].x, robot_status.dest_pos[robot_status.dest_pos_num].y, robot_status.dest_pos[robot_status.dest_pos_num].th);
                        robot_status.is_arrival = true;
                    }
                    else    //dest_pos_arrayをみてまだ先があるならspinを実行
                    {
                        cerr << "go to spin at over line\n";    //debug

                        fprintf(stderr, "ssm_odom(%lf, %lf, %lf)\n", ssm_odom.data.x, ssm_odom.data.y, ssm_odom.data.theta);
                        fprintf(stderr, "dest_pos(%lf, %lf, %lf)\n", robot_status.dest_pos[robot_status.dest_pos_num].x, robot_status.dest_pos[robot_status.dest_pos_num].y, robot_status.dest_pos[robot_status.dest_pos_num].th);

                        if(robot_status.is_foward_target == true)   //targetに近づいたらとまる
                        {
                            if(robot_status.dest_pos_num % 3 == 1)
                            {
                                //音鳴らして〜
                                my_stop();
                                ssm_sound.data = sound_arrival_target;
                                ssm_sound.write();

                                sleepSSM(5.0);
                            }
                        }
                        else
                        {
                            Spur_stop();
                            sleepSSM(1.0);
                        }

                        robot_status.dest_pos_num++;
                        robot_status.set_over_line_pos(robot_status.dest_pos_num);

                        //速度設定: targetに向かうときだけおそい
                        if(robot_status.is_foward_target == true)
                        {
                            if(robot_status.dest_pos_num % 3 == 1)
                            {
                                Spur_set_vel(0.4);
                                fprintf(stderr, "vel: %lf\n", 0.4);
                            }
                            else
                            {
                                Spur_set_vel(keiro[robot_status.dest_wp_num-1].velocity);
                                fprintf(stderr, "vel: %lf\n", keiro[robot_status.dest_wp_num-1].velocity);
                            }
                        }


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
        if(robot_status.is_precrash_stopping == true)   //ぷりクラッシュで停止gしてる場合
        {
            robot_status.cue_stop = true;
//            if(timer_console.clock())
//            {
//                cerr << "pre_crash!!!!!!!!!!!!!!!!!!!!!!!\n";   //debug
//            }
            //is_safety()を絶えずチェック
            if(is_safety() == true)    //安全が確認さえｒた場合
            {
                robot_status.cue_stop = false;
                robot_status.is_precrash_stopping = false;
                cerr << "saikai\n"; //debug
                ssm_sound.data = sound_restart;
                ssm_sound.write();


//                robot_status.waiting_stop_watch.end();
                robot_status.is_waiting_timer_started = false;

                if(robot_status.enable_avoiding == true)    //回避を許可している場合   いみふ」
                {
                }
            }
            else
            {   //まだなんかいる場合

                double waiting_time,lap;
//                robot_status.waiting_stop_watch.get(&waiting_time, &lap);
//                fprintf(stderr, "waiting time: %lf\n", waiting_time);
//                if(waiting_time > conf.stop_waiting_time.value)   //ストップウォッチ計測->一定時間経過していたら衝突回避スタート
                if(robot_status.waiting_stop_timer.clock())
                {
//                    robot_status.waiting_stop_watch.end();
                    robot_status.is_waiting_timer_started = false;

                    fprintf(stderr, "over waiting time!!\n");

                    if(robot_status.is_foward_target == true)
                    {
                        if( robot_status.dest_pos_num % 3 == 1 )    //targetに向かう途中に障害物に当たった場合
                        {
                            //とりあえず止まってみつけたことにする?
//                            sleepSSM(5.0);
                            ssm_sound.data = sound_arrival_target;
                            ssm_sound.write();

                            //つぎのwaypointに向かう ===> 経路上のsubgogalに還る
                            cerr << "つぎのwaypointに向かう ===> 経路上のsubgogalに還る\n";
                            robot_status.dest_pos_num++;
                            robot_status.set_over_line_pos(robot_status.dest_pos_num);
                            robot_status.set_cue_spin();

                            robot_status.is_precrash_stopping = false;
                            robot_status.cue_stop = false;
                        }
                    }
                    if(robot_status.enable_avoiding == true)    //回避を許可している場合
                    {
                        cerr << "avoiding しちゃます!!!!!!!!!!!!!!!!!!\n";
                        //avoidingのフラグを立てて障害物回避をする

                        //1. 左右を見ていける方向を決める
//                        sokuiki_fs.readLast();
//                        左をたしかめる
//                        if(is_blank() == true)
                        //左を確かめる

                        //どちらに行くかきメル


                        //2. まよこに行くようにdest_posを設定ぢて走行
                        //vel下げる





                        robot_status.is_avoiding = true;
                    }
                }
            }

        }
        else if(robot_status.is_arrival == true)
        {   // ---> waypoint arrival
            cerr << "arriaval\n";   //debug

            if(    keiro[robot_status.dest_wp_num].isGoalPoint() == true
                   || robot_status.dest_wp_num+1 == (int)keiro.size() )
            {
                fprintf(stdout, "GOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOAL!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11\n");
                ssm_sound.data = sound_goal;
                ssm_sound.write();
                break;  //勝利のbreak
            }

            if(robot_status.is_avoiding == true)
            {
                //衝突回避中
                //avoiding_phaseにしたがってしょりをかえる
                //途中でpre-crashで止まったらどうする?


                //phase 1: 正面を向いていくらか進む


                //phase 2: wpがみえたら？lineコマンド通常走行に復帰する


            }
            //else ifでもよい?
            if(robot_status.is_foward_target == true)
            {
                cerr << "foward target\n";
                //探索対象に向かってる
                //foward_target_phaseにしたがってしょりをかえる

                robot_status.is_foward_target = false;
            }
            else
            {
                //wp到着
                cerr << "waypoint 到着\n";

                //wp更新 <---ここか？ここか
                ++robot_status.dest_wp_num;
                robot_status.is_searching = keiro[robot_status.dest_wp_num-1].isSearchPoint();

                if (keiro[robot_status.dest_wp_num-1].isAdhocNavigation() == true)
                {
                    robot_status.is_adhoc = true;

//                    フラグをいろいろ調整しておく

                }



                if(robot_status.is_searching == true)
                {
                    my_stop();  //完全に止まる
                    double spin_ang_ = line2angle(keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                                                  keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y );
                    // 次のwpを向くまで回てんする
                    //これはひどい
                    cerr << "apin_ang " << 180 * spin_ang_/M_PI << "\n";
                    Spur_spin_GL(spin_ang_);
                    while(!ysd::gShutOff)
                    {
                        if(Spur_near_ang_GL(spin_ang_ , deg2rad(conf.Ypspur_near_ang_range.value)) == 1)
                        {
                            break;
                        }
                        Spur_spin_GL(spin_ang_);
                        usleepSSM(1000 * 5);
                    }
//                    my_stop();
                    sleepSSM(1.0);
                    my_stop();

                    ssm_sound.data = sound_start_detection;
                    ssm_sound.write();

                    ssm_odom.readLast();
                    fprintf(stderr, "ssm_odom(%lf, %lf, %lf)\n", ssm_odom.data.x, ssm_odom.data.y, ssm_odom.data.theta);
                    cerr << "探索開始！\n";
                    //探索開始
                    //human-detectorに命令を送る
                    //けいろ番号を与える?
//                    human_detector_start_cue.data = keiro[robot_status.dest_wp_num-1].detection_radius;
                    human_detector_start_cue.write();

                    //ptz-handlerに命令を送る
                    ptz_move_start_cue.data = true;
                    ptz_move_start_cue.write();

                    //ptz-handlerから終わった報告があるまで待つ
                    //ssm-clusterが更新されたことを確認
                    sleep(conf.target_detection_stop_time.value);    // <-- config?


                    cluster.readLast();
                    if(cluster.data.num_clusters() > 0)    //クラスタがあった場合
//                    if(0)   //かくかくするけど近づかない
                    {
                        ssm_odom.readLast();    //ここでいいのか？
                        vector<int> target_num;
                        vector<ysd::_rect> cluster_from_camera;     //いらない?
                        vector<ysd::_rect> target_cluster_array_wp;

                        double area_length = sqrt(  (keiro[robot_status.dest_wp_num].x-keiro[robot_status.dest_wp_num-1].x)*(keiro[robot_status.dest_wp_num].x-keiro[robot_status.dest_wp_num-1].x)
                                                  + (keiro[robot_status.dest_wp_num].y-keiro[robot_status.dest_wp_num-1].y)*(keiro[robot_status.dest_wp_num].y-keiro[robot_status.dest_wp_num-1].y)
                                                 );

                        for(unsigned int i = 0; i < cluster.data.num_clusters(); ++i)
                        { // ---> for each cluster
                            fprintf(stderr, "cluster[%d] G(%lf, %lf)\n", i, cluster.data.rect[i].x_g, cluster.data.rect[i].y_g);

                            cerr << "i= " << i << "!\n";

                            //clusterそれぞれに、たいして、探索エリア内にいるかどうかの判定
                            //target_posをwp座標系に変換すべし
                            ysd::_rect target_cluster_pos_wp;
                            pos_GL2wp(&cluster.data.rect[i], &target_cluster_pos_wp,
                                      keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                                      line2angle(keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y, keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y)
                                    );

                            if(  target_cluster_pos_wp.y_g < (keiro[robot_status.dest_wp_num-1].run_area_left_width + conf.detect_area_y_mergin.value)
                                 && target_cluster_pos_wp.y_g > -1.0*(keiro[robot_status.dest_wp_num-1].run_area_right_width + conf.detect_area_y_mergin.value)
                                 && target_cluster_pos_wp.x_g > 0.0
                                 && target_cluster_pos_wp.x_g < area_length + keiro[robot_status.dest_wp_num].run_area_extention_length )
                            {   // ---> クラスタが探索エリアないにいる場合
                                // ---> (x_g, y_g)と角度が最も近い画像をもってくる

//                                cerr << "!!\n";

                                //clusterの位置をかカメラ座標系に変換
                                ysd::_rect target_cluster_pos_c;
                                pos_GL2camera(&cluster.data.rect[i], &target_cluster_pos_c, &ssm_odom.data);

                                //kakukaku_angs作成   //ホントはここじゃない
                                int split = 8;
                                double pan_min_fs = -82.0;
                                double pan_max_fs = 82.0;
                                vector<double> kakukaku_angs;   //deg
                                kakukaku_angs.resize(split+1);
                                kakukaku_angs[0] = pan_min_fs;
                                for(unsigned int i = 0; i < kakukaku_angs.size(); ++i)
                                {
                                    kakukaku_angs[i] = fabs( rad2deg( atan2(target_cluster_pos_c.y_g, target_cluster_pos_c.x_g))  - (pan_min_fs + (double)i*(pan_max_fs-pan_min_fs)/split) );
//                                    cerr << "pan: " << pan_min_fs + (double)i*(pan_max_fs-pan_min_fs)/split << "\n";
//                                    cerr << "atan2: " << atan2(target_cluster_pos_c.y_g, target_cluster_pos_c.x_g) << "\n";
//                                    cerr << i << " " << kakukaku_angs[i] << "\n";   //debug
                                }

                                int min_ang_num = 0;
                                for(unsigned int i = 0; i < kakukaku_angs.size(); i++)
                                {
                                    if(kakukaku_angs[min_ang_num] > kakukaku_angs[i] )
                                    {
                                        min_ang_num = i;
                                    }
                                }
                                fprintf(stderr, "[%d] pos_c( %lf, %lf ) diff_ang = %lf[deg]\n",
                                        min_ang_num, target_cluster_pos_c.x_g, target_cluster_pos_c.y_g, kakukaku_angs[min_ang_num] );   //debug
                                char img_name[512];
                                sprintf(img_name, "/dev/shm/test-%d.jpg", min_ang_num);

                                cv::Mat src, out;
                                src = cv::imread(img_name);
                                out = src.clone();
                                //                            cv::imshow(img_name, src);  //debug
                                //                            cv::waitKey(0);             //debug
                                // <--- (x_g, y_g)と角度が最も近い画像をもってくる



                                //target-recognize
                                if( target_recognizer::target_recognize(src,
                                                                        &out,
                                                                        deg2rad(pan_min_fs + (double)min_ang_num*(pan_max_fs-pan_min_fs)/split),
                                                                        &target_cluster_pos_c ) == true)
                                {
                                    target_num.push_back(i);
                                    cluster_from_camera.push_back(target_cluster_pos_c);    //いあらない?
                                    target_cluster_array_wp.push_back(target_cluster_pos_wp);
                                }
                            }   // <--- クラスタが探索エリアないにいる場合
                            else
                            {   //クラスタが探索エリア外の場合
                            }
                        }// <--- for each cluster

                        if(target_num.size() > 0)
                        {
                            ssm_sound.data = sound_found;
                            ssm_sound.write();

//                            //そーとする---バブルソート
//                            for(unsigned int i = 0; i < target_num.size()-1; ++i)
//                            {
//                                for(unsigned int j = target_num.size()-1; j > i; j--)
//                                {
//                                    if(cluster_from_camera[target_num[j-1]].x_g > cluster_from_camera[target_num[j]].x_g)
//                                    {
//                                        int temp = target_num[j-1];
//                                        target_num[j-1] = target_num[j];
//                                        target_num[j] = temp;
//                                    }
//                                }
//                            }

                            vector<ysd::_rect> target_cluster_array_gl;

                            std::sort( target_cluster_array_wp.begin(), target_cluster_array_wp.end(), compare_cluster_pos );
                            for(unsigned int i = 0; i < target_cluster_array_wp.size(); i++)
                            {
                                ysd::_rect _temp;

                                pos_wp2Gl(&target_cluster_array_wp[i], &_temp,
                                          keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                                          line2angle(keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y, keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y)
                                        );
                                target_cluster_array_gl.push_back(_temp);
                                fprintf(stderr, "[%d]: wp( %lf, %lf ) gl( %lf, %lf )\n", i,
                                        target_cluster_array_wp[i].x_g, target_cluster_array_wp[i].y_g,
                                        target_cluster_array_gl[i].x_g, target_cluster_array_gl[i].y_g
                                        );
                            }

                            //探索対象に向かう経路を計画
                            robot_status.dest_pos.clear();
                            main_controler::pos pos_;

                            for(unsigned int i = 0; i < target_cluster_array_gl.size(); ++i)
                            {
                                //                            unsigned int i = 0;     //最も近いターゲットのみ近づく

                                //サブゴールを計算
                                double sub_x, sub_y;
                                calc_subgoal_pos(keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                                                 keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y,
                                                 target_cluster_array_gl[i].x_g, target_cluster_array_gl[i].y_g,
                                                 &sub_x, &sub_y);

                                //サブゴールをdest_posに登録
                                pos_.x = sub_x;
                                pos_.y = sub_y;
                                pos_.th = line2angle(keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                                                     pos_.x, pos_.y);
                                robot_status.dest_pos.push_back(pos_);

                                //ターゲットの近くをdest_posに登録
                                double target_approach_mergin = conf.target_approach_mergin.value;
                                //ここで探索エリア・走行エリア判定
                                if(target_cluster_array_wp[i].y_g >= 0.0)
                                {   //run_area_left_width
                                    fprintf(stderr, "左にターゲット\n");
                                    if( target_cluster_array_wp[i].y_g > keiro[robot_status.dest_wp_num-1].run_area_left_width )
                                    {
                                        target_approach_mergin = fabs(target_cluster_array_wp[i].y_g - keiro[robot_status.dest_wp_num-1].run_area_left_width);
                                        cerr << "とりあえずいけるところまで行くわ。左に\n";
                                        fprintf(stderr, "target approach mergin: %lf\n", target_approach_mergin);
                                    }
                                }
                                else
                                {   //run_area_right_width
                                    fprintf(stderr, "右にターゲット\n");
                                    if( target_cluster_array_wp[i].y_g < -1.0*keiro[robot_status.dest_wp_num-1].run_area_right_width )
                                    {
                                        target_approach_mergin = fabs(target_cluster_array_wp[i].y_g + keiro[robot_status.dest_wp_num-1].run_area_right_width);
                                        cerr << "とりあえずいけるところまで行くわ。右に\n";
                                        fprintf(stderr, "target approach mergin: %lf\n", target_approach_mergin);
                                    }
                                }
                                pos_.th = line2angle(pos_.x, pos_.y, target_cluster_array_gl[i].x_g, target_cluster_array_gl[i].y_g);
                                pos_.x = target_cluster_array_gl[i].x_g - target_approach_mergin*cos(pos_.th);
                                pos_.y = target_cluster_array_gl[i].y_g - target_approach_mergin*sin(pos_.th);
                                robot_status.dest_pos.push_back(pos_);

                                //経路に還ってくるサブゴールを登録
                                pos_.x = sub_x;
                                pos_.y = sub_y;
                                pos_.th = line2angle(target_cluster_array_gl[i].x_g, target_cluster_array_gl[i].y_g, pos_.x, pos_.y);
                                robot_status.dest_pos.push_back(pos_);

                            }
                            //次のwpをdest_posの最後に登録
                            pos_.x = keiro[robot_status.dest_wp_num].x;
                            pos_.y = keiro[robot_status.dest_wp_num].y;
                            pos_.th = line2angle( keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                                    keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y );
                            robot_status.dest_pos.push_back(pos_);

                            //各種フラグをたてて。slaveに渡す
                            robot_status.dest_pos_num = 0;
                            robot_status.over_line_mergin = conf.spur_over_line_mergin.value;
                            robot_status.set_over_line_pos(robot_status.dest_pos_num);

                            robot_status.is_searching = true;
                            robot_status.is_avoiding = false;
                            robot_status.is_foward_target = true;

                            robot_status.enable_avoiding = false;
                            robot_status.enable_seemless = false;

                            robot_status.is_arrival = false;
                            robot_status.is_precrash_stopping = false;

                            robot_status.set_cue_spin();
                            robot_status.cue_stop = false;

                            robot_status.foward_target_phase = 0;
                            robot_status.avoiding_pahse = 0;

                            //ypspur_vel 更新
                            Spur_set_vel(keiro[robot_status.dest_wp_num-1].velocity);
                            fprintf(stderr, "vel: %lf\n", keiro[robot_status.dest_wp_num-1].velocity);
                            // ---> 表示
                            cerr << "dest_pos\n";
                            for (unsigned int i = robot_status.dest_pos_num; i < robot_status.dest_pos.size(); ++i)
                            {
                                fprintf(stderr, "[%d]: (%lf, %lf, %lf)\n", i,
                                        robot_status.dest_pos[i].x, robot_status.dest_pos[i].y, robot_status.dest_pos[i].th);
                            }
                            // <--- 表示

                            //                        wait_restart_key();
                        }
                        else
                        {
                            //フラグを変えて下へ回す?
                            ssm_sound.data = sound_not_found;
                            ssm_sound.write();
                            robot_status.is_searching = false;
                        }

                    }
                    else    //クラスタがない場合、次のｗｐへ向かう
                    {
                        //フラグを変えて下へ回す?
                        ssm_sound.data = sound_not_found;
                        ssm_sound.write();
                        robot_status.is_searching = false;
                    }


                }
                if(robot_status.is_searching == false)
//                else
                {
                    //通常走行中
                    //次のwpに向かう

                    //-----------------------
                    //次のwpをdest_pos_arrayに登録
                    robot_status.dest_pos.clear();  //いままでの経由地点を消去
                    //                    dest_pos_array.clear(); //いままでの経由地点を消去

                    main_controler::pos buff;

                    cerr << "robot_status.dest_wp_num " << robot_status.dest_wp_num << "\n";    //debug
                    ssm_odom.readLast();
                    fprintf(stderr, "ssm_odom(%lf, %lf, %lf)\n", ssm_odom.data.x, ssm_odom.data.y, ssm_odom.data.theta);


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
                        robot_status.over_line_mergin =   2.0 * keiro[robot_status.dest_wp_num-1].velocity
                                / ( conf.ypspur_max_ang_vel.value * tan(0.5 * (M_PI - th_b2ls(keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                                                                        keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y,
                                keiro[robot_status.dest_wp_num+1].x, keiro[robot_status.dest_wp_num+1].y) ) ) );
                        if( robot_status.over_line_mergin < 0.3 ) robot_status.over_line_mergin = 0.3;

                        // ---> debug
                        cerr << "mergin " << robot_status.over_line_mergin << "\n";
                        cerr << "th_bl2 "
                             << rad2deg(M_PI - th_b2ls(keiro[robot_status.dest_wp_num-1].x, keiro[robot_status.dest_wp_num-1].y,
                                keiro[robot_status.dest_wp_num].x, keiro[robot_status.dest_wp_num].y,
                                keiro[robot_status.dest_wp_num+1].x, keiro[robot_status.dest_wp_num+1].y) )
                             << "\n";
                        // <--- debug
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
                    robot_status.cue_free = false;

                    robot_status.avoiding_pahse = 0;
                    robot_status.foward_target_phase = 0;

                    // ---> debug
                    cerr << "dest_wp\n";
                    robot_status.print_dest_wp();
                    cerr << "dest_pos\n";
                    robot_status.print_dest_pos(robot_status.dest_pos_num);
                    cerr << "robot_status.cue_line()" << robot_status.cue_line() << endl;
                    cerr << "robot_status.cue_spin()" << robot_status.cue_spin() << endl;
                    fprintf(stderr, "over_line (%lf, %lf)\n", robot_status.x_over_line, robot_status.y_over_line);
                    // <--- debug

                    //ypspur_vel 更新
                    Spur_set_vel(keiro[robot_status.dest_wp_num-1].velocity);

                }
            }
        }   // <--- waypoint arrival




        //-----------------------
        //動作実行
        //slave。状態取得とリンクしてる
        if(robot_status.cue_free == true)
        {
//            cerr << "free\n";
            Spur_free();
            continue;
        }
        else if(robot_status.cue_stop == true || robot_status.is_precrash_stopping == true)   //
        {
            Spur_stop();
        }
        else
        {
            if(robot_status.cue_line() == true)   //line
            {

//                if( timer_console.clock() )
//                {
//                                        cerr << "line\n"; //debug
//                    fprintf(stderr, "line (%lf, %lf, %lf)\n", robot_status.dest_pos[robot_status.dest_pos_num].x,
//                            robot_status.dest_pos[robot_status.dest_pos_num].y,
//                            robot_status.dest_pos[robot_status.dest_pos_num].th );
//                }
                Spur_stop_line_GL( robot_status.dest_pos[robot_status.dest_pos_num].x,
                        robot_status.dest_pos[robot_status.dest_pos_num].y,
                        robot_status.dest_pos[robot_status.dest_pos_num].th );
            }
            else if(robot_status.cue_spin() == true)    //spin
            {
//                if( timer_console.clock() )
//                {
//                                        cerr << "spin\n"; //debug]
//                    fprintf(stderr, "spin (%lf)", robot_status.dest_pos[robot_status.dest_pos_num].th);
//                }

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
