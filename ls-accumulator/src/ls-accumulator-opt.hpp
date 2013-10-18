#ifndef LS_ACCUMULATOR_OPT_HPP_
#define LS_ACCUMULATOR_OPT_HPP_

#include<getopt.h>
#include<iostream>
#include"ls-accumulator-conf.hpp"

//================================
// 固定値の定義
//================================
namespace ls_accumulator
{

    const char ConfFile[] = "ls-accumulator.conf";
    const char ShortOpt[] = "hg:G:s:o:i:m:r:c:";
    const struct option LongOpt[] =
    {
        { "help" , no_argument , 0 , 'h' },
        { "config" , required_argument , 0 , 'g' },
        { "write-config" , optional_argument , 0 , 'G' },
        { ConfIni_SokuikiRawID.item , required_argument , 0 , 's' },
        { ConfIni_OdometryID.item , required_argument , 0 , 'o' },
        { ConfIni_DataAccumulatingIntervalRotation.item , required_argument , 0 , 'r' },
        { ConfIni_DataAccumulatingIntervalDistance.item , required_argument , 0 , 'i' },
        { ConfIni_DataAccumulatingMaxDistance.item , required_argument , 0 , 'm' },
        { ConfIni_FilledDataClearMode.item , required_argument , 0 , 'c' },
        { 0 , 0 , 0 , 0 }
    };

}

//================================
// オプション読み込み関数
//================================
namespace ls_accumulator
{

    bool read_option( int argc , char **argv , proc_configuration* conf )
    {
        gnd_error( !conf , -1 , "invalid member" );

        while( 1 )
        {
            int opt;
            optarg = 0;
            opt = getopt_long( argc , argv , ShortOpt , LongOpt , 0 );

            if( opt < 0 )break;

            switch( opt )
            {

            //コンフィグ読み取り
            case 'g':
            {
                if( proc_conf_read( optarg , conf ) < 0 )
                {
                    std::cerr << "[\033[1m\033[31mERROR\033[30m\033[0m]: -g option , configure file syntax error" << std::endl;
                    return -1;
                }
                break;
            }

            //コンフィグ書き出し
            case 'G':
            {

                if( optarg )
                {
                    proc_conf_write( optarg , conf );
                    std::cerr << "output configuration file \"\033[4m" << optarg << "\033[0m\"" << std::endl;
                }
                else
                {
                    proc_conf_write( ConfFile , conf );
                    std::cerr << "output configuration file \"\033[4m" << ConfFile << "\033[0m\"" << std::endl;
                }

                return 2;
            }

            //測域生データIDのセット
            case 's':
            {
                conf->sokuiki_raw_id.value = atoi( optarg );
                break;
            }

            //オドメトリのIDセット
            case 'o':
            {
                conf->odometry_id.value = atoi( optarg );
                break;
            }

            //蓄積の距離間隔のセット
            case 'i':
            {
                conf->data_accumulating_interval_distance.value = atof( optarg );
                break;
            }

            //蓄積の角度間隔のセット
            case 'r':
            {
                conf->data_accumulating_interval_rotation.value = atof( optarg );
                break;
            }

            //蓄積の最大距離のセット
            case 'm':
            {
                conf->data_accumulating_max_distance.value = atoi( optarg );
                break;
            }

            //データクリアモードのセット
            case 'c':
            {
                conf->filled_data_clear_mode.value = atoi( optarg );
                break;
            }

            //ヘルプの表示
            case 'h':
            {
                std::cerr << "\033[1mNAME\033[0m\n"
                          << "\t\033[1m" << proc_name << "\033[0m - accumulating laser scanner data\n"
                          << "\n"
                          << "\033[1mSYNAPSIS\033[0m\n"
                          << "\t\033[1m" << proc_name << "\033[0m [ -options ]\n"
                          << "\n"
                          << "\033[1mOPTIONS\033[0m\n"
                          << "\t\033[1m -h \033[0m ( \033[1m --help \033[0m )\n"
                          << "\t\t print help\n"
                          << "\n"
                          << "\t\033[1m -g \033[0m ( \033[1m --config \033[0m ) <file name>\n"
                          << "\t\t read configure file\n"
                          << "\n"
                          << "\t\033[1m -G \033[0m ( \033[1m --write-configure \033[0m ) [file name]\n"
                          << "\t\t write configure file\n"
                          << "\n"
                          << "\t\033[1m -s \033[0m ( \033[1m "<< ConfIni_SokuikiRawID.item << " \033[0m ) <ID>\n"
                          << "\t\t set sokuiki raw ssm id\n"
                          << "\n"
                          << "\t\033[1m -o \033[0m ( \033[1m "<< ConfIni_OdometryID.item << " \033[0m ) <ID>\n"
                          << "\t\t set odometry ssm id\n"
                          << "\n"
                          << "\t\033[1m -i \033[0m ( \033[1m "<< ConfIni_DataAccumulatingIntervalDistance.item << " \033[0m ) <distance[m]>\n"
                          << "\t\t set data accumulating interval distance\n"
                          << "\n"
                          << "\t\033[1m -r \033[0m ( \033[1m "<< ConfIni_DataAccumulatingIntervalRotation.item << " \033[0m ) <rotation[deg]>\n"
                          << "\t\t set data accumulating interval rotation\n"
                          << "\n"
                          << "\t\033[1m -c \033[0m ( \033[1m "<< ConfIni_FilledDataClearMode.item << " \033[0m ) <0 or 1>\n"
                          << "\t\t set filled data clear mode on / off\n"
                          << "\n"
                          << "\t\033[1m -m \033[0m ( \033[1m "<< ConfIni_DataAccumulatingMaxDistance.item << " \033[0m ) <distance[m]>\n"
                          << "\t\t set data accumulating max distance\n"
                          << "\n";
                return 1;
            }


            }

        }

        return 0;
    }

}

#endif // LS_ACCUMULATOR_OPT_HPP_




