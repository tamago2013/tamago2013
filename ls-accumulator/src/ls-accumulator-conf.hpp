/*

    ls-accumulatorのコンフィグパラメータ

    2013-10-01
    MTM

*/

#ifndef LS_ACCUMULATOR_CONF_HPP_
#define LS_ACCUMULATOR_CONF_HPP_

#include<string>
#include<ssmtype/spur-odometry.h>
#include"ssm-laser.hpp"
#include"gnd-config-file.hpp"
#include"gnd-lib-error.h"

//==============================
// コンフィグ初期化関係の構造体・関数のプロトタイプ宣言
//==============================
namespace ls_accumulator
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
namespace ls_accumulator
{

    static const char proc_name[] = "ls-accumulator";

    //測域センサ生データのSSM名
    static const gnd::conf::parameter_array< char , 512 > ConfIni_SokuikiRawName =
    {
        "sokuiki-raw-ssm-name",
        SSM_NAME_SCAN_POINT_2D,
        "sokuiki raw data ssm name"
    };

    //測域センサ生データのSSMID
    static const gnd::conf::parameter< int > ConfIni_SokuikiRawID =
    {
        "sokuiki-raw-ssm-id",
        0,
        "sokuiki raw data ssm id"
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

    //出力する蓄積データのSSM名
    static const gnd::conf::parameter_array< char , 512 > ConfIni_SokuikiAccumulatedName =
    {
        "sokuiki-accumulated-ssm-name",
        "sokuiki_ac",
        "sokuiki accumulated data ssm name"
    };

    //データを蓄積するロボットの走行距離間隔
    static const gnd::conf::parameter< float > ConfIni_DataAccumulatingIntervalDistance =
    {
        "accumulating-interval-distance",
        0.5,
        "accumulating interval distance[m]"
    };

    //データを蓄積するロボットの最大走行距離
    static const gnd::conf::parameter< float > ConfIni_DataAccumulatingMaxDistance =
    {
        "accumulating-max-distance",
        10.0,
        "accumulating maximum distance[m]"
    };
}

//==============================
// 構造体の定義
//==============================
namespace ls_accumulator
{

    struct proc_configuration
    {
        //ssmのコンフィグ
        gnd::conf::parameter_array< char , 512 > sokuiki_raw_name; //測域生データのSSM名
        gnd::conf::parameter< int > sokuiki_raw_id; //測域生データのSSMID
        gnd::conf::parameter_array< char , 512 > odometry_name; //オドメトリのSSM名
        gnd::conf::parameter< int > odometry_id; //オドメトリのSSMID
        gnd::conf::parameter_array< char , 512 > sokuiki_accumulated_name; //出力するデータのSSM名

        //プログラムの動作に関するコンフィグ
        gnd::conf::parameter< float > data_accumulating_interval_distance; //データを蓄積する、ロボットの走行距離
        gnd::conf::parameter< float > data_accumulating_max_distance; //データを蓄積する最大のロボットの走行距離

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
namespace ls_accumulator
{

    //コンフィグパラメータの初期化
    inline int proc_conf_initialize( proc_configuration* conf )
    {
        gnd_assert( !conf , -1 , "invalid null pointer" );

        memcpy( &conf->sokuiki_raw_name , &ConfIni_SokuikiRawName , sizeof( ConfIni_SokuikiRawName ) );
        memcpy( &conf->sokuiki_raw_id , &ConfIni_SokuikiRawID , sizeof( ConfIni_SokuikiRawID ) );
        memcpy( &conf->odometry_name , &ConfIni_OdometryName , sizeof( ConfIni_OdometryName ) );
        memcpy( &conf->odometry_id , &ConfIni_OdometryID , sizeof( ConfIni_OdometryID ) );
        memcpy( &conf->sokuiki_accumulated_name , &ConfIni_SokuikiAccumulatedName , sizeof( ConfIni_SokuikiAccumulatedName ) );
        memcpy( &conf->data_accumulating_interval_distance , &ConfIni_DataAccumulatingIntervalDistance , sizeof( ConfIni_DataAccumulatingIntervalDistance ) );
        memcpy( &conf->data_accumulating_max_distance , &ConfIni_DataAccumulatingMaxDistance , sizeof( ConfIni_DataAccumulatingMaxDistance ) );

        return 0;
    }

    //コンフィグパラメータを取得
    inline int proc_conf_get( gnd::conf::configuration* src , proc_configuration* dest )
    {
        gnd_assert( !src , -1 , "invalid null pointer" );
        gnd_assert( !dest , -1 , "invalid null pointer" );

        gnd::conf::get_parameter( src , &dest->sokuiki_raw_name );
        gnd::conf::get_parameter( src , &dest->sokuiki_raw_id );
        gnd::conf::get_parameter( src , &dest->odometry_name );
        gnd::conf::get_parameter( src , &dest->odometry_id );
        gnd::conf::get_parameter( src , &dest->sokuiki_accumulated_name );
        gnd::conf::get_parameter( src , &dest->data_accumulating_interval_distance );
        gnd::conf::get_parameter( src , &dest->data_accumulating_max_distance );

        return 0;
    }

    //コンフィグパラメータをコンフィグクラスに登録
    inline int proc_conf_set( gnd::conf::configuration* dest , proc_configuration* src )
    {
        gnd_assert( !src , -1 , "invalid null pointer" );
        gnd_assert( !dest , -1 , "invalid null pointer" );

        gnd::conf::set_parameter( dest , &src->sokuiki_raw_name );
        gnd::conf::set_parameter( dest , &src->sokuiki_raw_id );
        gnd::conf::set_parameter( dest , &src->odometry_name );
        gnd::conf::set_parameter( dest , &src->odometry_id );
        gnd::conf::set_parameter( dest , &src->sokuiki_accumulated_name );
        gnd::conf::set_parameter( dest , &src->data_accumulating_interval_distance );
        gnd::conf::set_parameter( dest , &src->data_accumulating_max_distance );

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




#endif // LS_ACCUMULATOR_CONF_HPP_
