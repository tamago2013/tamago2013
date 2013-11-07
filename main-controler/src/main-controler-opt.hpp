//main-cntroler-opt.hpp

#include<getopt.h>
#include<iostream>

#include"main-controler-config.hpp"

//================================
// 固定値の定義
//================================
namespace main_controler
{

    const char ConfFile[] = "main-controler.conf";
    const char ShortOpt[] = "hg:G:s:o:f:t:";
    const struct option LongOpt[] =
    {
        { "help" , no_argument , 0 , 'h' },
        { "config" , required_argument , 0 , 'g' },
        { "write-config" , optional_argument , 0 , 'G' },
        { ConfIni_OdometryID.item , required_argument , 0 , 'o' },
    };

}

//================================
// オプション読み込み関数
//================================
namespace main_controler
{

    bool read_option( int argc , char **argv , proc_configuration* conf )
    {
        gnd_error( !conf , -1 , "invalid member" );

        while( 1 )
        {
            int opt;
            optarg = 0;
            opt = getopt_long( argc , argv , ShortOpt , LongOpt , 0 );

            if( opt < 0 ){ break; }

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

            //オドメトリのIDセット
            case 'o':
            {
                conf->odometry_id.value = atoi( optarg );
                break;
            }

            case 'f':
            {
//                std::cerr << optarg << "\n";
//                std::string a = optarg;
//                char b[512];
//                sprintf(b, "%s", a.c_str());
//                sprintf(conf->route_data_path.value, "%s", a.c_str());
                sprintf(conf->route_data_path.value, "%s", optarg);
//                std::cerr << "! " << conf->route_data_path.value << "\n";
                break;
            }

            case 't':
            {
                conf->start_waypoint.value = atoi( optarg );
                break;
            }

            //ヘルプの表示
            case 'h':
            {
                std::cerr << "\033[1mNAME\033[0m\n"
                          << "\t\033[1m" << proc_name << "\033[0m - planning and controle robot\n"
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
                          << "\t\033[1m -o \033[0m ( \033[1m "<< ConfIni_OdometryID.item << " \033[0m ) <ID>\n"
                          << "\t\t set odometry ssm id\n"
                          << "\n"
                          << "\t\033[1m -f \033[0m ( \033[1m "<< ConfIni_RouteDataPath.item << " \033[0m ) <file path>\n"
                          << "\t\t set route file\n"
                          << "\n"
                          << "\t\033[1m -t \033[0m ( \033[1m "<< ConfIni_StartWayPoint.item << " \033[0m ) <waypoint number>\n"
                          << "\t\t set start waypoint\n"
                          << "\n";
                return 1;
            }


            }

        }

        return 0;
    }

}



