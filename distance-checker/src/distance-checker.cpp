/*

  distance-checker
  オドメトリを監視してひたすら走行距離を出力するだけ。

  2013-10-19
  MTM

*/


//================================
// include
//================================
//c,c++
#include<csignal>
#include<iostream>
#include<ctime>
#include<cmath>

//roboken
#include<ssm.hpp>
#include<ssmtype/spur-odometry.h>

//ssmtype
#include<ssm-laser.hpp>

//gndlib
#include"gnd-timer.hpp"

//headers
#include"distance-checker-opt.hpp"            //オプション読み取り関係の定義

//================================
// ssm data / finalize functions
//================================
bool ssm_isopen = false;
SSMApi< Spur_Odometry > ssm_odometry;       //オドメトリ

void finalize_ssm()
{
    std::cerr << "\n";

    if( ssm_odometry.isOpen() )
    {
        ssm_odometry.close();
        std::cerr << "ssm-data " << ssm_odometry.getStreamName() << " closed.\n";
    }

    if( ssm_isopen )
    {
        endSSM();
        ssm_isopen = false;
        std::cerr << "ssm closed.\n";
    }
}

//================================
// signal
//================================
bool is_shutting_down = false;

void ctrl_c( int a_status )
{
    is_shutting_down = true;
    finalize_ssm();
    signal( SIGINT , NULL );
}

//================================
// main
//================================
int main ( int argc , char **argv )
{
    //--------------------------------------
    // 値の定義
    //--------------------------------------
    Spur_Odometry past_odometry;        //1回前に覚えたオドメトリ
    double total_distance = 0.0;
    gnd::inttimer timer_console;
    timer_console.begin( CLOCK_REALTIME , 1.0 , -1.0 );

    //--------------------------------------
    // シグナルハンドラを登録
    //--------------------------------------
    signal( SIGINT , ctrl_c );

    //--------------------------------------
    // オプション解析
    //--------------------------------------
    if( distance_checker::read_option( argc , argv ) != 0 )return -1;

    //--------------------------------------
    // SSM初期化
    //--------------------------------------
    {
        std::cerr << "initializing ssm ... ";
        if( !initSSM() )
        {
            std::cerr << "[\033[1m\033[31mERROR\033[30m\033[0m]:fail to initialize ssm.\n";
            return -1;
        }
        else
        {
            std::cerr << "OK.\n";
            ssm_isopen = true;
        }

        std::cerr << "initializing ssm-data " <<  distance_checker::ssm_name << "... ";
        if( !ssm_odometry.openWait( distance_checker::ssm_name.c_str() , distance_checker::ssm_id  , 0.0 , SSM_READ ) )
        {
            std::cerr << "[\033[1m\033[31mERROR\033[30m\033[0m]:fail to open ssm-data " <<  distance_checker::ssm_name << "\n";
            return -1;
        }
        else
        {
            ssm_odometry.readLast();
            past_odometry = ssm_odometry.data;

            std::cerr << "OK.\n";
        }

    }

    //--------------------------------------
    // メインループ
    //--------------------------------------
    std::cout << "\n"
              << "\n\n\n\n\n\n";
    bool first_read = true;
    while( !is_shutting_down )
    {
        //----------------------------------
        //sleep
        usleepSSM(1000);

        //----------------------------------
        //コンソール描画
        if( timer_console.clock() )
        {
            std::cout.flush();
            std::cout << "\033[6A"
                      << "\033[2K ------------- \033[1m\033[33m" << "distance-checker" << " \033[39m\033[0m -------------\n"
                      << "\033[2K total distance             : \033[1m" << total_distance << "\033[0m [m]\n"
                      << "\033[2K odometry ssm name          : " << distance_checker::ssm_name << "\n"
                      << "\033[2K odometry ssm id            : " << distance_checker::ssm_id << "\n"
                      << "\033[2K odometry (x,y,theta)       : " << ssm_odometry.data.x << " , " << ssm_odometry.data.y << " , " << ssm_odometry.data.theta << "\n"
                      << "\033[2K \n";
        }

        //----------------------------------
        //オドメトリの読み込み
        if( !ssm_odometry.readNew() )
        {
            continue;
        }

        if( first_read ) //更新された最初の1回目はまだオドメトリを記憶するだけにしておく
        {
            past_odometry = ssm_odometry.data;
            first_read = false;
            continue;
        }

        //----------------------------------
        //走行距離の計算
        double run_distance = ( ssm_odometry.data.x - past_odometry.x ) * ( ssm_odometry.data.x - past_odometry.x ) + ( ssm_odometry.data.y - past_odometry.y ) * ( ssm_odometry.data.y - past_odometry.y );
        run_distance = sqrt( run_distance );
        past_odometry = ssm_odometry.data;
        total_distance += run_distance;
    }

    //--------------------------------------
    // 終了処理
    //--------------------------------------
    finalize_ssm();

    std::cout << "*******************************\n"
              << "total distance : \033[1m" << total_distance << "\033[0m [m]\n"
              << "*******************************\n"
              << "\n";

    return 0;
}


