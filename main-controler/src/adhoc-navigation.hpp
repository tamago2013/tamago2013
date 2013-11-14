/*----------------------------------------------------------

  つくばチャレンジ2013最後の難関、パイロンロードを攻めるプログラム
  main-controllerから関数として呼び出される

  2013-11-13
  MTM

 -----------------------------------------------------------*/

#ifndef ADHOCNAVIGATION_HPP
#define ADHOCNAVIGATION_HPP

#include<vector>
#include<set>

#include<yp-spur.h>
#include<ssmtype/spur-odometry.h>

#include"ssm-sound.hpp"
#include"ssm-laser.hpp"
#include"gnd-coord-tree.hpp"
#include"gnd-timer.hpp"

namespace adhoc_navigation
{

    //----------------------------------
    // パラメータ(ここを変えてコンパイルすることで値を変更する。コンフィグファイル？なにそれおいしいの？)
    //認識・走行領域
    const float pylon_estimation_area_width  = 4.0;        //パイロン推定の際に見る範囲のロボット左右方向の幅[m]
    const float pylon_estimation_area_offset = 0.0;        //パイロン推定の際に見る範囲のロボット前方方向のオフセット[m]
    const float pylon_estimation_area_length = 6.0;        //パイロン推定の際に見る範囲のロボット前方方向の長さ[m]
    const float pylon_estimation_distance_interval = -1.0; //パイロン推定を行う距離間隔[m](ゼロまたは負の値で常にパイロン推定)
    const float running_distance = 36.0;                   //アドホックで走行する距離[m] この距離を超えると終了とみなす。
    //パイロン推定
    const float pylon_radius = 0.07;                       //推定するパイロンの半径[m]
    const float pylon_penalty_radius = 0.23;                //パイロンの中心からこの半径[m]の間に点があったらペナルティ
    const unsigned int pylon_likelihood_threshold = 6;     //パイロンとみなす最低スコア　（スコア＝pylon_radius以内に入る点の数-pylon_penalty_radiusに入る点の数×ペナルティゲイン）
    const int pylon_penalty_gain = 5;                      //ペナルティエリアに入った点を減点する数
    const float max_pylon_distance = 6.0;                  //蓄積したパイロン位置がこの値[m]以上遠かったら捨てる
    //パイロンライン推定
    const float pylon_interval = 2.2;                      //パイロンの基本間隔[m]
    const float pylon_interval_error = 0.5;                //許容するパイロン間隔誤差[m]
    const float pylon_angle_error = 20.0 * M_PI / 180.0;   //許容するパイロンを結ぶ線分の角度誤差[rad]。オドメトリのthetaからプラマイこの角度分ずれていても許容する。
    //シュプールライン推定
    const float spur_line_max_error_angle = 30.0 * M_PI / 180.0; //進行方向をこの角度以上に修正するものは無視する

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

    bool initialized = false;                                         //初期化されたかどうか（最初に関数をコールすると初期化される）
    Spur_Odometry past_odometry;                                      //1個前のオドメトリ（蓄積の距離計算に用いる）
    Spur_Odometry first_odometry;                                     //最初のオドメトリ（終了条件判定に用いる）
    std::vector< Point2D > downsampled_scan;                          //ダウンサンプリングされたスキャンデータ（FS）
    std::vector< Point2D > pylon_bs;                                  //推定したパイロン候補のポジション（BS）
    std::vector< Line2D > pylon_line_bs;                              //推定したパイロン候補から導出されるパイロンのペアを結ぶ線分(BS)
    Point2D target_line_basepoint;                                    //YP-Spurで追従させる直線の基点
    double target_line_angle;                                         //YP-Spurで追従させる直線の傾き

    //-----------------------------------
    // 関数の宣言
    bool run( SSMApi< Spur_Odometry >& ssm_odometry , SSMSOKUIKIData3D& ssm_sokuiki , SSMApi< SoundType >& ssm_sound );
    void initialize( SSMApi< Spur_Odometry >& ssm_odometry );
    void updatePylon( SSMApi< Spur_Odometry >& ssm_odometry , SSMSOKUIKIData3D& ssm_sokuiki );
    void updatePylonLine( SSMApi< Spur_Odometry >& ssm_odometry );
    void updateTargetLine( SSMApi< Spur_Odometry >& ssm_odometry );

    //------------------------------------
    // 関数の定義 [ 関数本体 ]
    // この関数がmain-controllerから呼び出される。実際に制御を行う関数。
    // 戻り値：移動距離が終了条件に達していればtrue、そうでなければfalse
    bool run( SSMApi< Spur_Odometry >& ssm_odometry , SSMSOKUIKIData3D& ssm_sokuiki , SSMApi< SoundType >& ssm_sound )
    {
        //初期化されていなかったら、初期化
        if( !initialized )
        {
            initialize( ssm_odometry );
        }

        //走行距離が終了値以上だったら終了判定
        {
            float diff_x = first_odometry.x - ssm_odometry.data.x;
            float diff_y = first_odometry.y - ssm_odometry.data.y;

            if( diff_x * diff_x + diff_y * diff_y >= running_distance * running_distance )
            {
                return true;
            }
        }

        //パイロン推定距離に達したらパイロン推定
        {
            float diff_x = past_odometry.x - ssm_odometry.data.x;
            float diff_y = past_odometry.y - ssm_odometry.data.y;
            if( diff_x * diff_x + diff_y * diff_y >= pylon_estimation_distance_interval * pylon_estimation_distance_interval )
            {
                //パイロン認識
                updatePylon( ssm_odometry , ssm_sokuiki );

                //パイロンラインの認識
                updatePylonLine( ssm_odometry );
                past_odometry = ssm_odometry.data;

                //目標追従直線の更新
                updateTargetLine( ssm_odometry );

                //パイロンラインの数に合わせてサウンドを変えて送る
                if( pylon_line_bs.size() == 1 )
                {
                    ssm_sound.data = -1;
                }
                if( pylon_line_bs.size() == 2 )
                {
                    ssm_sound.data = -2;
                }
                if( pylon_line_bs.size() >= 3 )
                {
                    ssm_sound.data = -3;
                }
                ssm_sound.write();
            }
        }

        return false;
    }

    //-----------------------------------
    // 関数の定義 [ 初期化関数 ]
    // 開始時点でのオドメトリを記録しておく
    void initialize( SSMApi< Spur_Odometry >& ssm_odometry )
    {
        //すでに初期化されていたら何もしない
        if( initialized )
        {
            return;
        }

        //初期オドメトリを記憶
        first_odometry = ssm_odometry.data;
        past_odometry = ssm_odometry.data;

        //初期目標直線→今のオドメトリでまっすぐ
        target_line_basepoint.x = ssm_odometry.data.x;
        target_line_basepoint.y = ssm_odometry.data.y;
        target_line_angle = ssm_odometry.data.theta;

        //パイロン位置初期化
        pylon_bs.clear();
        pylon_line_bs.clear();

        initialized = true;
    }

    //-----------------------------------
    // 関数の定義 [ パイロン推定 ]
    void updatePylon( SSMApi< Spur_Odometry >& ssm_odometry , SSMSOKUIKIData3D& ssm_sokuiki )
    {
        //--------------------------------------------------------
        // 計算量削減のため、先にパイロンとペナルティーエリアの距離を２乗しておく
        float sq_pylon_radius = pylon_radius * pylon_radius;
        float sq_pylon_penalty_radius = pylon_penalty_radius * pylon_penalty_radius;

        //--------------------------------------------------------
        // scan_fsのうち、エリア内の非エラーの点だけBS座標系にして抽出
        std::vector< Point2D > scandata_inside;
        for( size_t i=0; i<ssm_sokuiki.data.numPoints(); ++i )
        {
            if( ssm_sokuiki.data[i].isWarning() || ssm_sokuiki.data[i].status == ssm::laser::STATUS_NO_REFLECTION )
            {
                continue;    void updateTargetLine( SSMApi< Spur_Odometry >& ssm_odometry );
            }
            if( ssm_sokuiki.data[i].reflect.x < pylon_estimation_area_offset || ssm_sokuiki.data[i].reflect.x > pylon_estimation_area_length + pylon_estimation_area_offset )
            {
                continue;
            }
            if( ssm_sokuiki.data[i].reflect.y < - pylon_estimation_area_width / 2 || ssm_sokuiki.data[i].reflect.y > + pylon_estimation_area_width / 2 )
            {
                continue;
            }

            float r = sqrt( ssm_sokuiki.data[i].reflect.x * ssm_sokuiki.data[i].reflect.x + ssm_sokuiki.data[i].reflect.y * ssm_sokuiki.data[i].reflect.y );
            float t = atan2( ssm_sokuiki.data[i].reflect.y , ssm_sokuiki.data[i].reflect.x ) + ssm_odometry.data.theta;

            Point2D p;
            p.x = ssm_odometry.data.x + r * cos( t );
            p.y = ssm_odometry.data.y + r * sin( t );

            bool neighbor_pylon_exist = false;
            for( size_t j=0; j<pylon_bs.size() ; ++j )
            {
                float diff_x = p.x - pylon_bs[j].x;
                float diff_y = p.y - pylon_bs[j].y;
                float sq_dist = diff_x * diff_x + diff_y * diff_y;

                if( sq_dist < sq_pylon_penalty_radius )
                {
                    neighbor_pylon_exist = true;
                    break;
                }
            }

            if( !neighbor_pylon_exist )
            {
                scandata_inside.push_back( p );
            }
        }

        //--------------------------------------------------------
        // 領域内の点からパイロン候補を推定
        std::vector< char > scandata_is_used( scandata_inside.size() , 0 );
        for( size_t i=0; i<scandata_inside.size(); ++i )
        {
            if( scandata_is_used[i] == 1 )
            {
                continue;
            }

            std::vector< unsigned int > neighbor_list;  //見つけた近傍のリスト
            unsigned int penalty_neighbor_count = 0; //ペナルティー距離に入った点の数
            for( size_t j=0; j<scandata_inside.size(); ++j )
            {
                if( i == j || scandata_is_used[j] )
                {
                    continue;
                }

                float diff_x = scandata_inside[i].x - scandata_inside[j].x;
                float diff_y = scandata_inside[i].y - scandata_inside[j].y;

                float sq_dist = diff_x * diff_x + diff_y * diff_y;

                if( sq_dist < sq_pylon_radius )
                {
                    neighbor_list.push_back( j );
                    continue;
                }
                if( sq_dist < sq_pylon_penalty_radius )
                {
                    penalty_neighbor_count++;
                    continue;
                }
            }

            int likelihood = (int)neighbor_list.size() - pylon_penalty_gain * (int)penalty_neighbor_count;
            if( likelihood >= (int)pylon_likelihood_threshold )
            {
                pylon_bs.push_back( scandata_inside[i] );

                scandata_is_used[i] = 1;
                for( size_t k=0; k<neighbor_list.size(); k++ )
                {
                    scandata_is_used[ neighbor_list[k] ] = 1;
                }
            }
        }

        //--------------------------------------------------------
        // 自己位置から閾値以上の距離にあるパイロン候補の削除
        for( std::vector< Point2D >::iterator i = pylon_bs.begin(); i!=pylon_bs.end(); ++i )
        {
            float diff_x = i->x - ssm_odometry.data.x;
            float diff_y = i->y - ssm_odometry.data.y;

            if( diff_x * diff_x + diff_y * diff_y > max_pylon_distance * max_pylon_distance )
            {
                pylon_bs.erase( i );
                --i;
            }
        }
    }

    //-----------------------------------
    // 関数の定義 [ パイロンを結ぶ線分推定 ]
    void updatePylonLine( SSMApi< Spur_Odometry >& ssm_odometry )
    {
        pylon_line_bs.clear();

        //すべての組み合わせのパイロン線分を計算する。
        float max_pylon_interval = pylon_interval + pylon_interval_error;
        float min_pylon_interval = pylon_interval - pylon_interval_error;
        for( size_t i=0; i<pylon_bs.size(); ++i )
        for( size_t j=0; j<pylon_bs.size(); ++j )
        {
            if( i == j )
            {
                continue;
            }

            //２つのパイロン間隔を測定
            float diff_x = pylon_bs[i].x - pylon_bs[j].x;
            float diff_y = pylon_bs[i].y - pylon_bs[j].y;
            float interval = sqrt( diff_x * diff_x + diff_y * diff_y );

            if( interval < min_pylon_interval || interval > max_pylon_interval )
            {
                continue;
            }

            //2つのパイロンの傾きとオドメトリの向きの差分を測定
            float angle = atan2( diff_y , diff_x );
            float diff_angle = fabs( angle - ( ssm_odometry.data.theta + M_PI / 2 ) );

            while( diff_angle > 2 * M_PI )
            {
                diff_angle -= 2 * M_PI;
            }
            if( diff_angle > M_PI )
            {
                diff_angle = 2 * M_PI - diff_angle;
            }

            if( diff_angle > pylon_angle_error )
            {
                continue;
            }

            //合致したものを登録
            Line2D line;
            line.start = pylon_bs[i];
            line.finish = pylon_bs[j];

            pylon_line_bs.push_back( line );
        }
    }

    //-----------------------------------
    //関数の定義[ YP-Spurで追従する直線方程式の計算 ]
    void updateTargetLine( SSMApi< Spur_Odometry >& ssm_odometry )
    {
        if( pylon_line_bs.size() < 1 )
        {
            //パイロンラインが1本もないとき：オドメトリでまっすぐ進む
            target_line_basepoint.x = ssm_odometry.data.x;
            target_line_basepoint.y = ssm_odometry.data.y;
            target_line_angle = ssm_odometry.data.theta;

            return;
        }
        else if( pylon_line_bs.size() == 1 )
        {
            //パイロンラインが１本のとき：その中点から、そのラインに垂直に伸びる線分
            target_line_basepoint.x = ( pylon_line_bs[0].start.x +  pylon_line_bs[0].finish.x ) / 2;
            target_line_basepoint.y = ( pylon_line_bs[0].start.y +  pylon_line_bs[0].finish.y ) / 2;
            target_line_angle = atan2( pylon_line_bs[0].finish.y - pylon_line_bs[0].start.y , pylon_line_bs[0].finish.x - pylon_line_bs[0].start.x ) + M_PI / 2;
        }
        else if( pylon_line_bs.size() == 2 )
        {
            //パイロンラインが2本のとき：2つの中点を結ぶ線分
            target_line_basepoint.x = ( pylon_line_bs[0].start.x +  pylon_line_bs[0].finish.x ) / 2;
            target_line_basepoint.y = ( pylon_line_bs[0].start.y +  pylon_line_bs[0].finish.y ) / 2;
            Point2D target_finish;
            target_finish.x = ( pylon_line_bs[1].start.x +  pylon_line_bs[1].finish.x ) / 2;
            target_finish.y = ( pylon_line_bs[1].start.y +  pylon_line_bs[1].finish.y ) / 2;
            target_line_angle = atan2( target_finish.y - target_line_basepoint.y , target_finish.x - target_line_basepoint.x );

        }
        else
        {
            //パイロンラインが3本以上の時：最も距離のはなれた2つを結ぶ線分
            std::vector< Point2D > center_points( pylon_line_bs.size() );
            for( size_t i=0; i<pylon_line_bs.size(); ++i )
            {
                center_points[i].x = ( pylon_line_bs[i].start.x +  pylon_line_bs[i].finish.x ) / 2;
                center_points[i].y = ( pylon_line_bs[i].start.y +  pylon_line_bs[i].finish.y ) / 2;
            }

            float best_sq_distance = -1.0;
            size_t best_index_start = 0;
            size_t best_index_finish = 1;
            for( size_t i=0;   i+1<center_points.size(); ++i )
            for( size_t j=i+1; j<center_points.size(); ++j )
            {
                float diff_x = center_points[i].x - center_points[j].x;
                float diff_y = center_points[i].y - center_points[j].y;
                float sq_distance = diff_x * diff_x + diff_y * diff_y;

                if( sq_distance > best_sq_distance )
                {
                    best_sq_distance = sq_distance;
                    best_index_start = i;
                    best_index_finish = j;
                }
            }

            target_line_basepoint.x = ( pylon_line_bs[ best_index_finish ].start.x +  pylon_line_bs[ best_index_finish ].finish.x ) / 2;
            target_line_basepoint.y = ( pylon_line_bs[ best_index_finish ].start.y +  pylon_line_bs[ best_index_finish ].finish.y ) / 2;
            Point2D target_finish;
            target_finish.x = ( pylon_line_bs[ best_index_start ].start.x +  pylon_line_bs[ best_index_start ].finish.x ) / 2;
            target_finish.y = ( pylon_line_bs[ best_index_start ].start.y +  pylon_line_bs[ best_index_start ].finish.y ) / 2;
            target_line_angle = atan2( target_finish.y - target_line_basepoint.y , target_finish.x - target_line_basepoint.x );
        }

        //オドメトリと角度が反対にならないように調整
        while( ssm_odometry.data.theta < -M_PI )
        {
            ssm_odometry.data.theta += 2 * M_PI;
        }
        while( ssm_odometry.data.theta > +M_PI )
        {
            ssm_odometry.data.theta -= 2 * M_PI;
        }

        if( fabs( ssm_odometry.data.theta - target_line_angle ) > M_PI / 2 )
        {
            target_line_angle += M_PI;
        }

        //オドメトリを指定した角度以上修正するものは無視
        if( fabs( ssm_odometry.data.theta - target_line_angle ) > spur_line_max_error_angle )
        {
            target_line_basepoint.x = ssm_odometry.data.x;
            target_line_basepoint.y = ssm_odometry.data.y;
            target_line_angle = ssm_odometry.data.theta;
        }
    }

}

#endif // ADHOCNAVIGATION_HPP



