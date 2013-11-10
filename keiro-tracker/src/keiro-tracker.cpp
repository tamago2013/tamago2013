/*

  keiro-tracker
  spur-adjustを監視してそこに沿ったkeiro.datを吐く。

  2013-11-10
  MTM

*/


//================================
// include
//================================
//c,c++
#include<csignal>
#include<iostream>
#include<fstream>
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
#include"keiro-tracker-opt.hpp"            //オプション読み取り関係の定義

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
    gnd::inttimer timer_console;        //コンソール表示用GNDタイマ
    std::ofstream ofs_keiro;            //keiro吐き出し用のFileStream
    unsigned long keiro_counter = 0;    //keiroを何行吐き出したか？
    timer_console.begin( CLOCK_REALTIME , 1.0 , -1.0 );

    //--------------------------------------
    // シグナルハンドラを登録
    //--------------------------------------
    signal( SIGINT , ctrl_c );

    //--------------------------------------
    // オプション解析
    //--------------------------------------
    if( keiro_tracker::read_option( argc , argv ) != 0 )return -1;

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

        std::cerr << "initializing ssm-data " <<  keiro_tracker::ssm_name << "... ";
        if( !ssm_odometry.openWait( keiro_tracker::ssm_name.c_str() , keiro_tracker::ssm_id  , 0.0 , SSM_READ ) )
        {
            std::cerr << "[\033[1m\033[31mERROR\033[30m\033[0m]:fail to open ssm-data " <<  keiro_tracker::ssm_name << "\n";
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
    // keiro吐き出し用ファイルストリームの初期化
    //--------------------------------------
    std::cerr << "open " << keiro_tracker::output_name << " ... ";
    ofs_keiro.open( keiro_tracker::output_name.c_str() );

    if( !ofs_keiro )
    {
        std::cerr << "[\033[1m\033[31mERROR\033[30m\033[0m]:fail to open " << keiro_tracker::output_name << "\n";
        return 1;
    }
    else
    {
        std::cerr << "OK.\n";
    }

    //--------------------------------------
    // メインループ
    //--------------------------------------
    std::cout << "\n"
              << "\n\n\n\n\n\n\n\n\n";
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
            std::cout << "\033[9A"
                      << "\033[2K --\033[1m\033[33m" << "keiro-tracker" << "\033[39m\033[0m --\n"
                      << "\033[2K odometry ssm name : " << keiro_tracker::ssm_name << "\n"
                      << "\033[2K odometry ssm id   : " << keiro_tracker::ssm_id << "\n"
                      << "\033[2K odometry x        : " << ssm_odometry.data.x << " [m]\n"
                      << "\033[2K odometry y        : " << ssm_odometry.data.y << " [m]\n"
                      << "\033[2K odometry theta    : " << ssm_odometry.data.theta << " [rad]\n"
                      << "\033[2K output file       : " << keiro_tracker::output_name << "\n"
                      << "\033[2K output waypoint   : " << keiro_counter << " pts\n";
            std::cout << "\033[2K \n";
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
        //走行距離・角度の計算
        double run_distance = ( ssm_odometry.data.x - past_odometry.x ) * ( ssm_odometry.data.x - past_odometry.x ) + ( ssm_odometry.data.y - past_odometry.y ) * ( ssm_odometry.data.y - past_odometry.y );
        double run_angle = fabs( ssm_odometry.data.theta - past_odometry.theta );
        run_distance = sqrt( run_distance );

        //----------------------------------
        //走行距離あるいは回転角が既定値以上だったら経路吐き出し
        if( run_distance >= keiro_tracker::track_interval || run_angle >= keiro_tracker::angle_interval )
        {
            past_odometry = ssm_odometry.data;
            ofs_keiro << "A " << ssm_odometry.data.x << " " << ssm_odometry.data.y << " 1.0 0.7" << std::endl;
            keiro_counter++;
        }

    }

    //--------------------------------------
    // 終了処理
    //--------------------------------------
    finalize_ssm();
    ofs_keiro.close();

    std::cout << keiro_tracker::output_name << " closed ( total " << keiro_counter << " waypoints )\n";

    return 0;
}


