#ifndef DISTANCE_CHECKER_OPT_HPP_
#define DISTANCE_CHECKER_OPT_HPP_

#include<getopt.h>
#include<iostream>
#include<string>

//================================
// 固定値の定義
//================================
namespace distance_checker
{

    const char ShortOpt[] = "hn:i:l:";
    const struct option LongOpt[] =
    {
        { "help" , no_argument , 0 , 'h' },
        { "ssm-name" , required_argument , 0 , 'n' },
        { "ssm-id" , required_argument , 0 , 'i' },
        { "log-name" , required_argument , 0 , 'l' },
        { 0 , 0 , 0 , 0 }
    };

}

//================================
// オプションで決定する変数の定義
//================================
namespace distance_checker
{

    int ssm_id = 0;
    std::string ssm_name = "spur_odometry";
    std::string log_name = "";  //ログ出力のファイル名（空で出力しない）
}


//================================
// オプション読み込み関数
//================================
namespace distance_checker
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

            //ログ出力設定
            case 'l':
            {
                log_name = optarg;
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
                          << "\t\t set odometry data ssm-id( default = 1 ) / off\n"
                          << "\n"
                          << "\t\033[1m -n \033[0m ( \033[1m --ssm-name \033[0m ) <ssm-name>\n"
                          << "\t\t set odometry data ssm-name( default = \"spur_odometry\" )\n"
                          << "\n"
                          << "\t\033[1m -l \033[0m ( \033[1m  --log-name \033[0m ) <log-file-name>\n"
                          << "\t\t set distance logging mode and output file name/ off\n"
                          << "\n";
                return 1;
            }


            }

        }

        return 0;
    }

}

#endif // LS_ACCUMULATOR_OPT_HPP_




