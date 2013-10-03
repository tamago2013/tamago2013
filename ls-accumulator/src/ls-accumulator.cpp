/*

  ls-accumulator
  urg-proxyの吐き出したscan-data2dをオドメトリに貼りつけて蓄積して、sokuiki-acとして流す。

  2013-10-01
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
#include"gnd-matrix-base.hpp"
#include"gnd-vector-base.hpp"
#include"gnd-matrix-coordinate.hpp"
#include"gnd-coord-tree.hpp"
#include"gnd-timer.hpp"

//headers
#include"ls-accumulator-ring-buffer.hpp"    //リングバッファクラスの定義
#include"ls-accumulator-conf.hpp"           //コンフィグ関係の定義
#include"ls-accumulator-opt.hpp"            //オプション読み取り関係の定義

//================================
// ssm data / finalize functions
//================================
bool ssm_isopen = false;
SSMApi< Spur_Odometry > ssm_odometry;       //オドメトリ
SSMScanPoint2D          ssm_sokuiki_raw;    //測域の生データ
SSMSOKUIKIData3D        ssm_sokuiki_ac;     //蓄積した測域データ

void finalize_ssm()
{
    std::cerr << "\n";

    if( ssm_odometry.isOpen() )
    {
        ssm_odometry.close();
        std::cerr << "ssm-data " << ssm_odometry.getStreamName() << " closed.\n";
    }

    if( ssm_sokuiki_raw.isOpen() )
    {
        ssm_sokuiki_raw.close();
        std::cerr << "ssm-data " << ssm_sokuiki_raw.getStreamName() << " closed.\n";
    }

    if( ssm_sokuiki_ac.isOpen() )
    {
        ssm_sokuiki_ac.close();
        std::cerr << "ssm-data " << ssm_sokuiki_ac.getStreamName() << " released.\n";
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
    unsigned int data_accumulate_scannum = 1;   //何スキャン分データを溜めるか

    ls_accumulator::proc_configuration conf;                //コンフィグパラメータの集合の構造体
    ls_accumulator::ring_buffer< ssm::Beam3D > ring_buf;    //リングバッファ

    gnd::matrix::coord_tree coordtree;          //座標系の関係を表す木
    int coordid_sn = -1;
    int coordid_fs     = -1;
    int coordid_bs     = -1;

    Spur_Odometry past_odometry;        //1回前に覚えたオドメトリ
    unsigned long write_count = 0;      //書き込んだ回数のカウント
    gnd::inttimer timer_console;
    timer_console.begin( CLOCK_REALTIME , 1.0 , -1.0 );

    //--------------------------------------
    // シグナルハンドラを登録
    //--------------------------------------
    signal( SIGINT , ctrl_c );

    //--------------------------------------
    // オプション解析
    //--------------------------------------
    ls_accumulator::proc_conf_initialize( & conf );     //コンフィグパラメータの初期化
    if( ls_accumulator::read_option( argc , argv , &conf ) != 0 )return -1;
    data_accumulate_scannum = (unsigned int)( conf.data_accumulating_max_distance.value / conf.data_accumulating_interval_distance.value ) + 1;

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

        std::cerr << "initializing ssm-data " << conf.sokuiki_raw_name.value << "... ";
        if( !ssm_sokuiki_raw.openWait( conf.sokuiki_raw_name.value , conf.sokuiki_raw_id.value , 0.0 , SSM_READ ) )
        {
            std::cerr << "[\033[1m\033[31mERROR\033[30m\033[0m]:fail to open ssm-data " << conf.sokuiki_raw_name.value << "\n";
            return -1;
        }
        else if( !ssm_sokuiki_raw.getProperty() )
        {
            std::cerr << "[\033[1m\033[31mERROR\033[30m\033[0m]:fail to get property of ssm-data " << conf.sokuiki_raw_name.value << "\n";
            return -1;
        }
        else
        {
            std::cerr << "OK.\n";
        }

        std::cerr << "initializing ssm-data " << conf.odometry_name.value << "... ";
        if( !ssm_odometry.openWait( conf.odometry_name.value , conf.odometry_id.value , 0.0 , SSM_READ ) )
        {
            std::cerr << "[\033[1m\033[31mERROR\033[30m\033[0m]:fail to open ssm-data " << conf.odometry_name.value << "\n";
            return -1;
        }
        else
        {
            //出力用データのアロケート
            ssm_sokuiki_ac.data.alloc( ssm_sokuiki_raw.property.numPoints * data_accumulate_scannum );
            ssm_sokuiki_raw.readLast();
            ssm_sokuiki_raw.setBlocking( true );
            ssm_sokuiki_ac.property = ssm_sokuiki_raw.property;
            ssm_sokuiki_ac.property.numPoints = ssm_sokuiki_raw.property.numPoints * data_accumulate_scannum;

            //リングバッファの最大値のセット
            ring_buf.set_max_size( ssm_sokuiki_raw.property.numPoints * data_accumulate_scannum );

            std::cerr << "OK.\n";
        }

        std::cerr << "initializing ssm-data " << conf.sokuiki_accumulated_name.value << "... ";
        if( !ssm_sokuiki_ac.create( conf.sokuiki_accumulated_name.value , conf.sokuiki_raw_id.value , 0.4 , 0.04 ) )
        {
            std::cerr << "[\033[1m\033[31mERROR\033[30m\033[0m]:fail to create ssm-data " << conf.sokuiki_accumulated_name.value  << "\n";
            return -1;
        }
        else if( !ssm_sokuiki_ac.setProperty() )
        {
            std::cerr << "[\033[1m\033[31mERROR\033[30m\033[0m]:fail to set property of ssm-data " << conf.sokuiki_accumulated_name.value  << "\n";
            return -1;
        }
        else
        {
            for( size_t i = 0 ; i < ssm_sokuiki_ac.property.numPoints ; ++i )
            {
                ssm_sokuiki_ac.data[i].intensity = 0.0;
                ssm_sokuiki_ac.data[i].status = ssm::laser::STATUS_ERROR;
                ssm_sokuiki_ac.data[i].reflect.x = 0.0;
                ssm_sokuiki_ac.data[i].reflect.y = 0.0;
                ssm_sokuiki_ac.data[i].reflect.z = 0.0;
            }

            ssm_odometry.readLast();
            past_odometry = ssm_odometry.data;

            ssm_sokuiki_ac.write( ssm_odometry.time );
            std::cerr << "OK.\n";
        }
    }

    //--------------------------------------
    // Coordinate Treeの作成
    //--------------------------------------
    {
        gnd::coord_matrix cm;
        gnd::matrix::set_unit( &cm ); //単位行列のセット

        coordid_bs  = coordtree.add( "odometry" , "root" , &cm );    //オドメトリ(BS)座標系の作成(関係：単位行列＝つまり同じ座標系)
        coordid_fs  = coordtree.add( "robot" , "odometry" , &cm );   //ロボット(FS)座標系の作成(関係は本当は単位行列ではないが、初めは単位行列で初期化)
        coordid_sn = coordtree.add( "sensor" , "robot" , &ssm_sokuiki_raw.property.coordm ); //センサ座標系の作成(関係：scan_data2dに入っている変換行列)

    }

    //--------------------------------------
    // メインループ
    //--------------------------------------
    std::cout << "\n"
              << "\n\n\n\n\n\n\n\n";
    while( !is_shutting_down )
    {
        //----------------------------------
        //sleep
        usleepSSM(50000);

        //----------------------------------
        //コンソール描画
        if( timer_console.clock() )
        {
            std::cout.flush();
            std::cout << "\033[9A"
                      << "\033[2K ------------- \033[1m\033[35m" << ls_accumulator::proc_name << " \033[39m\033[0m -------------\n"
                      << "\033[2K accumulate dist            : " << conf.data_accumulating_interval_distance.value << " [m]\n"
                      << "\033[2K accumulate max dist        : " << conf.data_accumulating_max_distance.value << " [m]\n"
                      << "\033[2K # of points of single scan : " << ssm_sokuiki_raw.property.numPoints << "\n"
                      << "\033[2K # of accumulating scan     : " << data_accumulate_scannum << "\n"
                      << "\033[2K ring buffer total size     : " << ssm_sokuiki_raw.property.numPoints * data_accumulate_scannum << "\n"
                      << "\033[2K odometry (x,y,theta)       : " << ssm_odometry.data.x << " , " << ssm_odometry.data.y << " , " << ssm_odometry.data.theta << "\n"
                      << "\033[2K writing count              : " << write_count << "\n"
                      << "\033[2K \n";
        }

        //----------------------------------
        //sokuiki_rawの読み込み
        if( !ssm_sokuiki_raw.readNew() )
        {
            continue;
        }

        //----------------------------------
        //オドメトリの読み込み
        ssm_odometry.readTime( ssm_sokuiki_raw.time );

        //----------------------------------
        //走行距離の計算
        double run_distance = ( ssm_odometry.data.x - past_odometry.x ) * ( ssm_odometry.data.x - past_odometry.x ) + ( ssm_odometry.data.y - past_odometry.y ) * ( ssm_odometry.data.y - past_odometry.y );
        run_distance = sqrt( run_distance );

        //----------------------------------
        //走行距離が閾値以上か？
        if( run_distance < conf.data_accumulating_interval_distance.value )
        {
            continue;
        }
        else
        {
            past_odometry = ssm_odometry.data;
        }

        //----------------------------------
        //FS座標系のcoordtreeをodometryにしたがって更新
        {
            gnd::coord_matrix cm;
            gnd::matrix::coordinate_converter //spur_odometryを基準にしてcoord_matrixを取得
            (
                &cm ,
                ssm_odometry.data.x , ssm_odometry.data.y , 0.0 ,
                cos( ssm_odometry.data.theta ) , sin( ssm_odometry.data.theta ) , 0.0 ,
                0.0 , 0.0 , 1.0
            );
            coordtree.set_coordinate( coordid_fs , &cm ); //coord_matrixをfs座標系にセット
        }

        //----------------------------------
        //Sensor→BS変換行列、BS→FS変換行列、Sensor→FS変換行列の取得
        gnd::coord_matrix cm_sn2bs;
        gnd::coord_matrix cm_bs2fs;
        gnd::coord_matrix cm_sn2fs;
        coordtree.get_convert_matrix( coordid_sn , coordid_bs , &cm_sn2bs );
        coordtree.get_convert_matrix( coordid_bs , coordid_fs , &cm_bs2fs );
        coordtree.get_convert_matrix( coordid_sn , coordid_fs , &cm_sn2fs );

        //----------------------------------
        //測域生データをBSに座標変換しつつリングバッファに追加
        {
            gnd::vector::fixed_column< 4 > point_sn;
            gnd::vector::fixed_column< 4 > point_bs;
            gnd::vector::fixed_column< 4 > sensor_origin_sn;
            gnd::vector::fixed_column< 4 > sensor_origin_bs;

            //sensor originの計算(全点共通なので先にやっておく)
            sensor_origin_sn[0] = ssm_sokuiki_raw.property.coordm[0][3];
            sensor_origin_sn[1] = ssm_sokuiki_raw.property.coordm[1][3];
            sensor_origin_sn[2] = ssm_sokuiki_raw.property.coordm[0][3];
            sensor_origin_sn[3] = 1.0;
            gnd::matrix::prod( &cm_sn2bs , &sensor_origin_sn , &sensor_origin_bs );

            for( size_t i = 0 ; i < ssm_sokuiki_raw.property.numPoints; ++i )
            {
                //リングバッファに入れるBeam3D型
                ssm::Beam3D bs_beam3d;

                //センサ座標系の点の計算
                point_sn[0] = ssm_sokuiki_raw.data[i].r * cos( ssm_sokuiki_raw.data[i].th );
                point_sn[1] = ssm_sokuiki_raw.data[i].r * sin( ssm_sokuiki_raw.data[i].th );
                point_sn[2] = 0.0;
                point_sn[3] = 1.0;
                gnd::matrix::prod( &cm_sn2bs , &point_sn , &point_bs );

                //BSのBeam3Dの作成
                bs_beam3d.intensity = ssm_sokuiki_raw.data[i].intensity;
                bs_beam3d.status    = ssm_sokuiki_raw.data[i].status;
                bs_beam3d.reflect.x = point_bs[0];
                bs_beam3d.reflect.y = point_bs[1];
                bs_beam3d.reflect.z = point_bs[2];
                bs_beam3d.origin.x  = sensor_origin_bs[0];
                bs_beam3d.origin.y  = sensor_origin_bs[1];
                bs_beam3d.origin.z  = sensor_origin_bs[2];

                ring_buf.push_back( bs_beam3d );
            }

        }

        //----------------------------------
        //リングバッファからFSに変換
        {
            gnd::vector::fixed_column< 4 > point_bs;
            gnd::vector::fixed_column< 4 > point_fs;
            gnd::vector::fixed_column< 4 > sensor_origin_sn;
            gnd::vector::fixed_column< 4 > sensor_origin_fs;

            //sensor originの計算(全点共通なので先にやっておく)
            sensor_origin_sn[0] = ssm_sokuiki_raw.property.coordm[0][3];
            sensor_origin_sn[1] = ssm_sokuiki_raw.property.coordm[1][3];
            sensor_origin_sn[2] = ssm_sokuiki_raw.property.coordm[0][3];
            sensor_origin_sn[3] = 1.0;
            gnd::matrix::prod( &cm_sn2fs , &sensor_origin_sn , &sensor_origin_fs );

            for( size_t i = 0 ; i < ssm_sokuiki_ac.property.numPoints ; ++i )
            {

                if( i >= ring_buf.size() ) //リングバッファがまたすべて埋まっていない場合→エラーのレーザー値で足りない部分を埋める
                {
                    ssm_sokuiki_ac.data[i].intensity = 0.0;
                    ssm_sokuiki_ac.data[i].status = ssm::laser::STATUS_ERROR;
                    ssm_sokuiki_ac.data[i].reflect.x = sensor_origin_fs[0];
                    ssm_sokuiki_ac.data[i].reflect.y = sensor_origin_fs[1];
                    ssm_sokuiki_ac.data[i].reflect.z = sensor_origin_fs[2];
                }
                else //それ以外→リングバッファをFSに座標変換して埋める
                {
                    point_bs[0] = ring_buf[i].reflect.x;
                    point_bs[1] = ring_buf[i].reflect.y;
                    point_bs[2] = ring_buf[i].reflect.z;
                    point_bs[3] = 1.0;
                    gnd::matrix::prod( &cm_bs2fs , &point_bs , &point_fs );

                    ssm_sokuiki_ac.data[i].intensity = ring_buf[i].intensity;
                    ssm_sokuiki_ac.data[i].status    = ring_buf[i].status;
                    ssm_sokuiki_ac.data[i].reflect.x = point_fs[0];
                    ssm_sokuiki_ac.data[i].reflect.y = point_fs[1];
                    ssm_sokuiki_ac.data[i].reflect.z = point_fs[2];
                }

                ssm_sokuiki_ac.data[i].origin.x = sensor_origin_fs[0];
                ssm_sokuiki_ac.data[i].origin.y = sensor_origin_fs[1];
                ssm_sokuiki_ac.data[i].origin.z = sensor_origin_fs[2];
            }
        }

        //----------------------------------
        //SSM出力
        ssm_sokuiki_ac.write( ssm_sokuiki_raw.time );
        write_count++;

    }

    //--------------------------------------
    // 終了処理
    //--------------------------------------
    finalize_ssm();

    return 0;
}


