#ifndef KEIRO_TRACKER_OPT_HPP_
#define KEIRO_TRACKER_OPT_HPP_

#include<getopt.h>
#include<iostream>
#include<string>
#include<cstdlib>
#include<cmath>

//================================
// 固定値の定義
//================================
namespace keiro_tracker
{

    const char ShortOpt[] = "hn:i:t:a:";
    const struct option LongOpt[] =
    {
        { "help" , no_argument , 0 , 'h' },
        { "ssm-name" , required_argument , 0 , 'n' },
        { "ssm-id" , required_argument , 0 , 'i' },
        { "tracking-interval" , required_argument , 0 , 't' },
        { "tracking-angular-interval" , required_argument , 0 , 'a' },
        { 0 , 0 , 0 , 0 }
    };

}

//================================
// オプションで決定する変数の定義
//================================
namespace keiro_tracker
{

    int ssm_id = 0;
    std::string ssm_name = "spur_adjust";
    double track_interval = 3.0;
    double angle_interval = 60 * M_PI / 180.0;
}


//================================
// オプション読み込み関数
//================================
namespace keiro_tracker
{

    bool read_option( int argc , char **argv )
    {

        while( 1 )
        {
            int opt;
            optarg = 0;
            opt = getopt_long( argc , argv , ShortOpt , LongOpt , 0 );

            if( opt < 0 )break;

            switch( opt )
            {

            //SSM-IDの指定
            case 'i':
            {
                ssm_id = atoi( optarg );
                break;
            }

            //SSM-NAMEの指定
            case 'n':
            {
                ssm_name = optarg;
                break;
            }

            //経路のトラッキング間隔を設定
            case 't':
            {
                track_interval = atof( optarg );
                break;
            }

            //経路の回転間隔を設定
            case 'a':
            {
                track_interval = atof( optarg ) * M_PI / 180.0;
                break;
            }

            //ヘルプの表示
            case 'h':
            {
                std::cerr << "\033[1mNAME\033[0m\n"
                          << "\t\033[1m distance checker \033[0m - check running total distance\n"
                          << "\n"
                          << "\033[1mSYNAPSIS\033[0m\n"
                          << "\t\033[1m " << argv[0] << " \033[0m[ -options ]\n"
                          << "\n"
                          << "\033[1mOPTIONS\033[0m\n"
                          << "\t\033[1m -h \033[0m ( \033[1m --help \033[0m )\n"
                          << "\t\t print help\n"
                          << "\n"
                          << "\t\033[1m -i \033[0m ( \033[1m  --ssm-id \033[0m ) <ssm-id>\n"
                          << "\t\t set odometry data ssm-id( default = 1 )\n"
                          << "\n"
                          << "\t\033[1m -n \033[0m ( \033[1m --ssm-name \033[0m ) <ssm-name>\n"
                          << "\t\t set odometry data ssm-name( default = \"spur_odometry\" )\n"
                          << "\n"
                          << "\t\033[1m -t \033[0m ( \033[1m  --tracking-interval \033[0m ) <tracking interval[m]>\n"
                          << "\t\t set keiro tracking interval[m](default = 3.0[m])\n"
                          << "\n"
                          << "\t\033[1m -a \033[0m ( \033[1m  --angular-interval \033[0m ) <angular interval[deg]>\n"
                          << "\t\t set keiro tracking angular interval[m](default = 60.0[deg])\n"
                          << "\n";
                return 1;
            }


            }

        }

        return 0;
    }

}

#endif // KEIRO_TRACKER_OPT_HPP_




