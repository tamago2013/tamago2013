/*----------------------------------------------------------

  つくばチャレンジ2013最後の難関、パイロンロードを攻めるプログラム
  その2：前方の開いている方向に向かう
  main-controllerから関数として呼び出される

  2013-11-13
  MTM

 -----------------------------------------------------------*/

#ifndef ADHOCNAVIGATION_2_HPP
#define ADHOCNAVIGATION_2_HPP

#include<vector>
#include<limits>
#include<fstream>

#include<yp-spur.h>
#include<ssmtype/spur-odometry.h>

#include"ssm-sound.hpp"
#include"ssm-laser.hpp"
#include"gnd-timer.hpp"

//#define WITHOUT_SPUR

namespace adhoc_navigation
{

    //----------------------------------
    //固定値パラメータ(ここを変えてコンパイルすることで値を変更する。コンフィグファイル？なにそれおいしいの？)

    //ロボットの動作
    const float stop_time = 3.0; //走行エリアに入ったときに、何秒間止まるか？
    const float normal_vel = 0.3; //通常走行時の速度
    const float slow_vel = 0.15;  //低速走行時の速度
    const float running_distance = 33.5; //走行する距離[m]
    const float estimate_distance_interval = 0.1; //方向推定をする距離間隔[m]
    const float slow_distance_threshold = 6.0; //最も優秀な直線がこれより短ければ、速度を遅くする
    //直線の評価
    const float search_distance = 10.0; //直線評価に用いる探索半径[m]
    const float direction_width = 10.0 * M_PI / 180.0; //評価直線をつくる角度幅[rad]
    const float direction_step  = 1.0 * M_PI / 180.0;  //評価直線の角度間隔[rad]
    //const float direction_evaluate_width = 1.7; //直線の幅[m]
    const float direction_evaluate_width = 0.3; //直線の幅[m] ※屋内用

    //-----------------------------------
    // データ
    typedef struct
    {
        float x;
        float y;
    }
    Point2D;

    typedef struct
    {
        Point2D start;
        Point2D finish;
    }
    Line2D;

    typedef struct
    {
        float theta;
        float length;
    }
    LengthDirection;

    typedef SSMApi< Spur_Odometry > SSMOdometry;
    typedef SSMApi< SoundType > SSMSound;

    bool initialized = false;                   //初期化されたかどうか（最初に関数をコールすると初期化される）
    std::vector< LengthDirection > directions;  //評価
    size_t best_direction_index;                //最優秀の評価の番号
    Spur_Odometry first_position_bs;
    Spur_Odometry past_position_bs;
    Spur_Odometry line_command_bs;

    //-----------------------------------
    // シュプール関数
    void robotStop()
    {
#ifndef WITHOUT_SPUR
        Spur_stop();

        double v , w;
        do
        {
            Spur_get_vel( &v , &w );
        }
        while( fabs(v) > 0.01 || fabs(w) > 0.01 );
#endif
    }

    //-----------------------------------
    // 幾何計算関数
    //　点と点の距離を計算
    double getSquaredPointDistance( Point2D& point1 , Point2D& point2 )
    {
        double diff_x = point1.x - point2.x;
        double diff_y = point1.y - point2.y;

        return diff_x * diff_x + diff_y * diff_y;
    }

    double getPointDistance( Point2D& point1 , Point2D& point2 )
    {
        return sqrt( getSquaredPointDistance( point1 , point2 ) );
    }

    // オドメトリとオドメトリの距離を計算
    double getSquaredPointDistance( Spur_Odometry& odom1 , Spur_Odometry& odom2 )
    {
        double diff_x = odom1.x - odom2.x;
        double diff_y = odom1.y - odom2.y;

        return diff_x * diff_x + diff_y * diff_y;
    }

    double getPointDistance( Spur_Odometry& odom1 , Spur_Odometry& odom2 )
    {
        return sqrt( getSquaredPointDistance( odom1 , odom2 ) );
    }

    // radとdegの変換
    inline float rad2deg( float rad )
    {
        return rad * 180.0 / M_PI;
    }

    inline float deg2rad( float deg )
    {
        return deg * M_PI / 180.0;
    }

    // coordinate-convert関数
    void convertCoordinateToFS( Point2D& input_point_coordinate_X , Point2D& output_point_coordinate_FS , Spur_Odometry& robot_pos_coordinate_X )
    {
        double diff_x = input_point_coordinate_X.x - robot_pos_coordinate_X.x;
        double diff_y = input_point_coordinate_X.y - robot_pos_coordinate_X.y;

        double r = sqrt( diff_x * diff_x + diff_y * diff_y );
        double t = atan2( diff_y , diff_x ) - robot_pos_coordinate_X.theta;

        output_point_coordinate_FS.x = r * cos( t );
        output_point_coordinate_FS.y = r * sin( t );
    }

     // coordinate-convert関数
    void convertCoordinateFromFS( Point2D& output_point_coordinate_X , Point2D& input_point_coordinate_FS , Spur_Odometry& robot_pos_coordinate_X )
    {
        double r = sqrt( input_point_coordinate_FS.x * input_point_coordinate_FS.x + input_point_coordinate_FS.y * input_point_coordinate_FS.y );
        double t = atan2( input_point_coordinate_FS.y , input_point_coordinate_FS.x ) + robot_pos_coordinate_X.theta;

        output_point_coordinate_X.x = r * cos( t ) + robot_pos_coordinate_X.x;
        output_point_coordinate_X.y = r * sin( t ) + robot_pos_coordinate_X.y;
    }

    //------------------------------------
    // run関数
    // この関数がmain-controllerから呼び出される。実際に制御を行う関数。
    // 戻り値：移動距離が終了条件に達していればtrue、そうでなければfalse
    bool run( SSMOdometry& ssm_odometry_bs , SSMOdometry& ssm_odometry_gl , SSMSOKUIKIData3D& ssm_sokuiki , SSMSound& ssm_sound , float start_x , float start_y , float finish_x , float finish_y )
    {
        //==============
        // 初期処理
        //==============
        if( !initialized )
        {
            //開始
            std::cerr << "\033[33m[adhoc-navigation]\033[39m start\n";

            //初期位置の獲得
            first_position_bs = ssm_odometry_bs.data;
            past_position_bs  = ssm_odometry_bs.data;

            //現在地点で停止
            robotStop();

            //現在地点から目標方向へ向く
            double theta = atan2( finish_y - ssm_odometry_gl.data.y , finish_x - ssm_odometry_gl.data.x );
            std::cerr << "\033[33m[adhoc-navigation]\033[39m turn " << rad2deg(theta) << "[deg]finish=( " << finish_x << "," << finish_y << ")( now theta = " << deg2rad(ssm_odometry_gl.data.theta) << ")\n";
#ifndef WITHOUT_SPUR
            Spur_spin_GL( theta );
#endif
            while(
#ifndef WITHOUT_SPUR
                  Spur_near_ang_GL( theta , deg2rad( 3.0 ) ) != 1
#else
                  false
#endif
                  )
            {
                usleepSSM(5000);
            }
            robotStop();

            //指定された時間止まる
             std::cerr << "\033[33m[adhoc-navigation]\033[39m wait " << stop_time << "[s]\n";
            sleepSSM( stop_time );

            //それぞれの方向の評価用直線を初期化
            directions.clear();
            for( float t = 0.0; t <= direction_width / 2; t += direction_step )
            {
                LengthDirection ld;
                ld.theta  = ssm_odometry_bs.data.theta + t;
                ld.length = 0.0;

                directions.push_back( ld );
            }
            for( float t = -direction_step; t >= - direction_width / 2; t -= direction_step )
            {
                LengthDirection ld;
                ld.theta  = ssm_odometry_bs.data.theta + t;
                ld.length = 0.0;

                directions.push_back( ld );
            }

            initialized = true;
            std::cerr << "\033[33m[adhoc-navigation]\033[39m initialized\n";
        }
        else
        {
            //==========================
            // 終了条件・更新条件の調査、Spurコマンドの発行
            //==========================
            if( getPointDistance( ssm_odometry_bs.data , first_position_bs ) >= running_distance )
            {
                std::cerr << "\033[33m[adhoc-navigation]\033[39m wait " << stop_time << "[s]\n";
                return true;
            }

            if( best_direction_index < directions.size() && directions[ best_direction_index ].length >= slow_distance_threshold )
            {
#ifndef WITHOUT_SPUR
                Spur_set_vel( normal_vel );
#endif
            }
            else
            {
#ifndef WITHOUT_SPUR
                Spur_set_vel( slow_vel );
#endif
            }

#ifndef WITHOUT_SPUR
            Spur_line_BS( line_command_bs.x , line_command_bs.y , line_command_bs.theta );
#endif

            if( getPointDistance( ssm_odometry_bs.data , past_position_bs ) < estimate_distance_interval )
            {
                return false;
            }
            past_position_bs = ssm_odometry_bs.data;

            //==============
            // 最尤直線に合わせて次の直線を展開
            //==============
            directions.clear();
            for( float t = 0.0; t <= direction_width / 2; t += direction_step )
            {
                LengthDirection ld;
                ld.theta  = line_command_bs.theta + t;
                ld.length = 0.0;

                directions.push_back( ld );
            }

            for( float t = -direction_step; t >= - direction_width / 2; t -= direction_step )
            {
                LengthDirection ld;
                ld.theta  = line_command_bs.theta + t;
                ld.length = 0.0;

                directions.push_back( ld );
            }

        }

        //==============
        // 各方向を評価
        //==============
        for( size_t i=0; i<directions.size(); ++i )
        {
            //直線座標系の
            Spur_Odometry direction_coordinate_origin_fs;
            direction_coordinate_origin_fs.x = 0.0;
            direction_coordinate_origin_fs.y = 0.0;
            direction_coordinate_origin_fs.theta = directions[i].theta - ssm_odometry_bs.data.theta;

            //各線分において、各URGの点に対して垂線の脚を計算　→　その位置がFS座標系原点から最も近いものの、FS座標系原点との距離を求める。
            float min_dist = search_distance;
            for( size_t j=0; j<ssm_sokuiki.data.numPoints(); ++j )
            {
                if( ssm_sokuiki.data[j].isWarning() || ssm_sokuiki.data[j].status == ssm::laser::STATUS_NO_REFLECTION )
                {
                    continue;
                }

                Point2D scan_fs;
                scan_fs.x = ssm_sokuiki.data[j].reflect.x;
                scan_fs.y = ssm_sokuiki.data[j].reflect.y;

                Point2D scan_direction_coordinate;
                convertCoordinateToFS( scan_fs , scan_direction_coordinate , direction_coordinate_origin_fs );

                if( fabs( scan_direction_coordinate.y ) > direction_evaluate_width / 2 )
                {
                    continue;
                }

                if( scan_direction_coordinate.x < 0 || scan_direction_coordinate.x > search_distance )
                {
                    continue;
                }

                float dist = scan_direction_coordinate.x;
                if( min_dist > dist )
                {
                    min_dist = dist;
                }


            }
            directions[i].length = min_dist;
        }

        //==============
        // 最も距離の長い方向を探す(距離がほぼ等しい場合はオドメトリに近い方を優先)
        //==============
        best_direction_index = 0;
        float best_length = 0.0;
        float best_ang_diff = direction_width / 2;
        for( size_t i=0; i<directions.size(); ++i )
        {
            if( fabs( directions[i].length - best_length ) < 0.05 )
            {
                float ang_diff = fabs( directions[i].theta - ssm_odometry_bs.data.theta );
                if( ang_diff < best_ang_diff )
                {
                    best_ang_diff = ang_diff;
                    best_direction_index = i;
                }
            }

            if( directions[i].length > best_length )
            {
                best_length = directions[i].length;
                best_ang_diff = fabs( directions[i].theta - ssm_odometry_bs.data.theta );
                best_direction_index = i;
            }
        }

        line_command_bs.x = ssm_odometry_bs.data.x;
        line_command_bs.y = ssm_odometry_bs.data.y;
        line_command_bs.theta = directions[best_direction_index].theta;
        std::cerr << "[adhoc-navigation] best angle " << rad2deg( line_command_bs.theta ) << "[deg]\n";

        if( best_direction_index != 0 )
        {
            ssm_sound.data = 0;
            ssm_sound.write();
        }

        return false;
    }

}

#endif // ADHOCNAVIGATION_2_HPP



