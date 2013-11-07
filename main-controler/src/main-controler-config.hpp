
#ifndef MAIN_CONTROLER_CONF_HPP_
#define MAIN_CONTROLER_CONF_HPP_

#include<string>
#include<ssmtype/spur-odometry.h>
#include"ssm-laser.hpp"
#include"gnd-config-file.hpp"
#include"gnd-lib-error.h"

//==============================
// コンフィグ初期化関係の構造体・関数のプロトタイプ宣言
//==============================
namespace main_controler
{
    struct proc_configuration;

    int proc_conf_initialize( proc_configuration* c );
    int proc_conf_get( gnd::conf::configuration* src  , proc_configuration* dest );
    int proc_conf_set( gnd::conf::configuration* dest , proc_configuration* src  );
    int proc_conf_write( std::string f , proc_configuration* dest );
    int proc_conf_read( std::string f , proc_configuration* src );
}

//==============================
// 固定値の定義
//==============================
namespace main_controler
{

    static const char proc_name[] = "main-controler";

    //YPspur max velocity
    static const gnd::conf::parameter<double> ConfIni_YPspurMaxVel =
    {
        "YPspur-max-velocity",
        0.7,
        "YPspur max velocity[m/sec]"
    };

    //YPspur max velocity
    static const gnd::conf::parameter<double> ConfIni_YPspurMaxAccel =
    {
        "YPspur-max-acceleration",
        1.5,
        "YPspur max acceleration[m/sec^2]"
    };

    //YPspur max angle velocity
    static const gnd::conf::parameter<double> ConfIni_YPspurMaxAngVel =
    {
        "YPspur-max-angle-velocity",
        3.14,
        "YPspur max angle velocity[rad/sec]"
    };

    //YPspur max angle velocity
    static const gnd::conf::parameter<double> ConfIni_YPspurMaxAngAccel =
    {
        "YPspur-max-angle-angle-accel",
        2.0,
        "YPspur max angle accelration[rad/sec^2]"
    };


    //YPspur near ang detection rabge
    static const gnd::conf::parameter<double> ConfIni_YPspurNearAngRange =
    {
        "YPspur-near-ang-detection-range",
        3.0,
        "YPspur near angle detection range [+/- deg]"
    };

    //オドメトリのSSM名
    static const gnd::conf::parameter_array< char , 512 > ConfIni_OdometryName =
    {
        "odometry-ssm-name",
        SNAME_ODOMETRY,
        "odometry data ssm name"
    };

    //オドメトリのSSMID
    static const gnd::conf::parameter< int > ConfIni_OdometryID =
    {
        "odometry-ssm-id",
        0,
        "odometry data ssm id"
    };

    //水平URGのSSM名
    static const gnd::conf::parameter_array< char, 512 > ConfIni_SokuikiFSName =
    {
        "sokuiki-fs-ssm-name",
        SSM_NAME_SOKUIKI_3D_FS,
        "sokuiki fs ssm name"
    };

    //水平URGのSSMID
    static const gnd::conf::parameter<int> ConfIni_SokuikiFSID =
    {
        "sokuiki-fs-ssm-id",
        0,
        "sokuiki fs ssm id"
    };

    //そくど変更するときの速度 <double>
    static const gnd::conf::parameter<double> ConfIni_SlowSpeedVelocity =
    {
        "slow-speed-velocity",
        0.4,
        "slow speed velocity [m/s]"
    };

    //経路データの在り処 <char*>
    static const gnd::conf::parameter_array< char, 512 > ConfIni_RouteDataPath =
    {
        "route-data-path",
        "../data/working/keiro.dat",
        "route data path"
    };

    //seemless-modeにするかいなか <bool>
    static const gnd::conf::parameter<bool> ConfIni_SeemLessMode =
    {
        "seemless-mode",
        false,
        "seemless mode"
    };

    //走行onky-mode <bool>    <-optでやれ<-でもここにあるといいかもおよ


    //スタート地点wp(とちゅうじっこう)
    static const gnd::conf::parameter<int> ConfIni_StartWayPoint =
    {
        "start-waypoint",
        0,
        "start waypoint"
    };

    //pre-crash safety mergin x
    static const gnd::conf::parameter<double> ConfIni_PreCrashSafetyMerginX =
    {
        "pre-crash-safety-mergin-x",
        0.7,
        "pre-crash safety mergin x [m]"
    };

    //pre-crash safety mergin y
    static const gnd::conf::parameter<double> ConfIni_PreCrashSafetyMerginY =
    {
        "pre-crash-safety-mergin-y",
        0.3,
        "pre-crash safety mergin y [m]"
    };

    //Spur over line mergin distance
    static const gnd::conf::parameter<double> ConfIni_SpurStopLineMergin =
    {
        "spur-stop-line-mergin",
        0.2,
        "spur_stop_line mergin distance [m]"
    };

    //waiting time
    static const gnd::conf::parameter<double> ConfIni_StopWaitingTime =
    {
        "stop-waiting-time",
        20.0,
        "stop waiting time till avoiding [sec]"
    };




}

//==============================
// 構造体の定義
//==============================
namespace main_controler
{

    struct proc_configuration
    {
        //ssmのコンフィグ

        gnd::conf::parameter<double> ypspur_max_vel;
        gnd::conf::parameter<double> ypspur_max_accel;
        gnd::conf::parameter<double> ypspur_max_ang_vel;
        gnd::conf::parameter<double> ypspur_max_ang_accel;
        gnd::conf::parameter<double> Ypspur_near_ang_range;
        gnd::conf::parameter_array< char , 512 > odometry_name; //オドメトリのSSM名
        gnd::conf::parameter< int > odometry_id; //オドメトリのSSMID
        gnd::conf::parameter_array<char, 512> sokuiki_fs_name;
        gnd::conf::parameter<int> sokuiki_fs_id;
        gnd::conf::parameter<double> slow_speed_velocity;
        gnd::conf::parameter_array<char, 512> route_data_path;
        gnd::conf::parameter<bool> seemless_mode;
        gnd::conf::parameter<int> start_waypoint;
        gnd::conf::parameter<double> pre_crash_safety_megin_x;
        gnd::conf::parameter<double> pre_crash_safety_megin_y;
        gnd::conf::parameter<double> spur_over_line_mergin;
        gnd::conf::parameter<double> stop_waiting_time;

        //初期化関数
        proc_configuration();
    };

    //初期化関数の実体
    inline proc_configuration::proc_configuration()
    {
        proc_conf_initialize( this );
    }

}

//==============================
// 関数の定義
//==============================
namespace main_controler
{

    //コンフィグパラメータの初期化
    inline int proc_conf_initialize( proc_configuration* conf )
    {
        gnd_assert( !conf , -1 , "invalid null pointer" );

        memcpy( &conf->ypspur_max_vel, &ConfIni_YPspurMaxVel, sizeof(ConfIni_YPspurMaxVel) );
        memcpy( &conf->ypspur_max_accel, &ConfIni_YPspurMaxAccel, sizeof(ConfIni_YPspurMaxAccel) );
        memcpy( &conf->ypspur_max_ang_vel, &ConfIni_YPspurMaxAngVel, sizeof(ConfIni_YPspurMaxAngVel) );
        memcpy( &conf->ypspur_max_ang_accel, &ConfIni_YPspurMaxAngAccel, sizeof(ConfIni_YPspurMaxAngAccel) );
        memcpy( &conf->Ypspur_near_ang_range, &ConfIni_YPspurNearAngRange, sizeof(ConfIni_YPspurNearAngRange) );
        memcpy( &conf->odometry_name , &ConfIni_OdometryName , sizeof( ConfIni_OdometryName ) );
        memcpy( &conf->odometry_id , &ConfIni_OdometryID , sizeof( ConfIni_OdometryID ) );
        memcpy( &conf->sokuiki_fs_name, &ConfIni_SokuikiFSName, sizeof(ConfIni_SokuikiFSName) );
        memcpy( &conf->sokuiki_fs_id, &ConfIni_SokuikiFSID, sizeof(ConfIni_SokuikiFSID) );
        memcpy( &conf->slow_speed_velocity, &ConfIni_SlowSpeedVelocity, sizeof(ConfIni_SlowSpeedVelocity) );
        memcpy( &conf->route_data_path, &ConfIni_RouteDataPath, sizeof(ConfIni_RouteDataPath) );
        memcpy( &conf->seemless_mode, &ConfIni_SeemLessMode, sizeof(ConfIni_SeemLessMode) );
        memcpy( &conf->start_waypoint, &ConfIni_StartWayPoint, sizeof(ConfIni_StartWayPoint) );
        memcpy( &conf->pre_crash_safety_megin_x, &ConfIni_PreCrashSafetyMerginX, sizeof(ConfIni_PreCrashSafetyMerginX) );
        memcpy( &conf->pre_crash_safety_megin_y, &ConfIni_PreCrashSafetyMerginY, sizeof(ConfIni_PreCrashSafetyMerginY) );
        memcpy( &conf->spur_over_line_mergin, &ConfIni_SpurStopLineMergin, sizeof(ConfIni_SpurStopLineMergin) );
        memcpy( &conf->stop_waiting_time, &ConfIni_StopWaitingTime, sizeof(ConfIni_StopWaitingTime) );


        return 0;
    }

    //コンフィグパラメータを取得
    inline int proc_conf_get( gnd::conf::configuration* src , proc_configuration* dest )
    {
        gnd_assert( !src , -1 , "invalid null pointer" );
        gnd_assert( !dest , -1 , "invalid null pointer" );

        gnd::conf::get_parameter(src, &dest->ypspur_max_vel);
        gnd::conf::get_parameter(src, &dest->ypspur_max_accel);
        gnd::conf::get_parameter(src, &dest->ypspur_max_ang_vel);
        gnd::conf::get_parameter(src, &dest->ypspur_max_ang_accel);
        gnd::conf::get_parameter(src, &dest->Ypspur_near_ang_range);
        gnd::conf::get_parameter( src , &dest->odometry_name );
        gnd::conf::get_parameter( src , &dest->odometry_id );
        gnd::conf::get_parameter(src, &dest->sokuiki_fs_name);
        gnd::conf::get_parameter(src, &dest->sokuiki_fs_id);
        gnd::conf::get_parameter(src, &dest->slow_speed_velocity);
        gnd::conf::get_parameter(src, &dest->route_data_path);
        gnd::conf::get_parameter(src, &dest->seemless_mode);
        gnd::conf::get_parameter(src, &dest->start_waypoint);
        gnd::conf::get_parameter(src, &dest->pre_crash_safety_megin_x);
        gnd::conf::get_parameter(src, &dest->pre_crash_safety_megin_y);
        gnd::conf::get_parameter(src, &dest->spur_over_line_mergin);
        gnd::conf::get_parameter(src, &dest->stop_waiting_time);

        return 0;
    }

    //コンフィグパラメータをコンフィグクラスに登録
    inline int proc_conf_set( gnd::conf::configuration* dest , proc_configuration* src )
    {
        gnd_assert( !src , -1 , "invalid null pointer" );
        gnd_assert( !dest , -1 , "invalid null pointer" );

        gnd::conf::set_parameter(dest, &src->ypspur_max_vel);
        gnd::conf::set_parameter(dest, &src->ypspur_max_accel);
        gnd::conf::set_parameter(dest, &src->ypspur_max_ang_vel);
        gnd::conf::set_parameter(dest, &src->ypspur_max_ang_accel);
        gnd::conf::set_parameter(dest, &src->Ypspur_near_ang_range);
        gnd::conf::set_parameter(dest , &src->odometry_name );
        gnd::conf::set_parameter(dest , &src->odometry_id );
        gnd::conf::set_parameter(dest, &src->sokuiki_fs_name);
        gnd::conf::set_parameter(dest, &src->sokuiki_fs_id);
        gnd::conf::set_parameter(dest, &src->slow_speed_velocity);
        gnd::conf::set_parameter(dest, &src->route_data_path);
        gnd::conf::set_parameter(dest, &src->seemless_mode);
        gnd::conf::set_parameter(dest, &src->start_waypoint);
        gnd::conf::set_parameter(dest, &src->pre_crash_safety_megin_x);
        gnd::conf::set_parameter(dest, &src->pre_crash_safety_megin_y);
        gnd::conf::set_parameter(dest, &src->spur_over_line_mergin);
        gnd::conf::set_parameter(dest, &src->stop_waiting_time);

        return 0;
    }

    //コンフィグの読み取り
    inline int proc_conf_read( std::string f , proc_configuration* dest )
    {
        gnd_assert( !dest , -1 , "invalid null pointer" );

        {
            int ret;
            gnd::conf::file_stream fs;
            if( ( ret = fs.read( f.c_str() ) ) < 0 )
            {
                return ret;
            }
            else
            {
                return proc_conf_get( &fs , dest );
            }
        }
    }

    //コンフィグの書き込み
    inline int proc_conf_write( std::string f , proc_configuration* src )
    {
        gnd_assert( !src , -1 , "invalid null pointer" );

        {
            int ret;
            gnd::conf::file_stream fs;
            if( ( ret = proc_conf_set( &fs , src ) ) < 0 )
            {
                return ret;
            }
            else
            {
                return fs.write( f.c_str() );
            }
        }
    }
}




#endif //MAIN_CONTROLER_CONF_HPP_
