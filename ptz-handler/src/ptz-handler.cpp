//-------------------------------------------------
//ptz-handler.cpp
//YSD
//since: 2013-9-25
//-------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ssm.hpp>
#include <math.h>
#include <string.h>

#include <curl/curl.h>

#include <opencv2/opencv.hpp>
#include <kaw_lib/SSM-Image.hpp>

#include "../../ysdlib/ysd-time.hpp"
#include "../../ysdlib/ysd-signal.hpp"
#include "ssm-ptz.hpp"

//gndlib
#include "gnd-timer.hpp"

using namespace std;

char pan_c[256];
bool foward_right;
inline void pan_swing(double current_pan, double pan_left, double pan_right)
{
//    fprintf(stderr, "\npan swing\n");
//    fprintf(stderr, "current pan: %lf\n", current_pan);
    if( foward_right == true && current_pan >= pan_right-1.0 )
    {
        system("curl http://172.16.30.15/axis-cgi/com/ptz.cgi?move=stop");
//        fprintf(stderr, "pan left\n");
        sprintf(pan_c, "curl http://172.16.30.15/axis-cgi/com/ptz.cgi?pan=%lf", pan_left);
//        printf(pan_c);
        system(pan_c);
        foward_right = false;
    }
    else if(foward_right ==false && current_pan <= pan_left+1.0)
    {
        system("curl http://172.16.30.15/axis-cgi/com/ptz.cgi?move=stop");
//        fprintf(stderr, "pan right\n");
        sprintf(pan_c, "curl http://172.16.30.15/axis-cgi/com/ptz.cgi?pan=%lf", pan_right);
//        printf(pan_c);
        system(pan_c);
        foward_right = true;
    }
//    else
//    {
//        fprintf(stderr, "するーーーーーーー\n");
//        cerr << "foward_right: " << foward_right << endl;
//    }

}

inline bool is_near_value(double value, double target, double error)
{
    if( (target - error <= value) && (value <= target + error) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


SSMApi<ImageC3> cam_image( SNAME_IMGC3, 1 );

double foward_angle = -8.0;
unsigned int pre_timeID = 0;
int sleep_cnt = 0;
void kakukaku(double current_pan, unsigned int timeID)
{
    if(is_near_value(current_pan, foward_angle, 0.5) == true)
    {
        system("curl http://172.16.30.15/axis-cgi/com/ptz.cgi?move=stop");
        if(timeID > pre_timeID)
        {
            ++sleep_cnt;
            if(sleep_cnt == 5)
            {
                if(foward_angle == -8.0)
                {
                    foward_angle = -30.0;
                }
                else if(foward_angle == -172.0)
                {
                    foward_angle = -8.0;
                }
                else
                {
                    foward_angle = foward_angle - 30.0;
                    if(foward_angle == -180.0)
                    {
                        foward_angle = -172.0;
                    }
                }
                sleep_cnt = 0;
            }

            sprintf(pan_c, "curl http://172.16.30.15/axis-cgi/com/ptz.cgi?pan=%lf", foward_angle);
            system(pan_c);
            pre_timeID = 0;

        }
        else if(pre_timeID == 0)
        {
            pre_timeID = timeID;
        }
    }
}

#define SNAME_INFO_IS_STOP "info_is_stop"
typedef struct {

    bool is_stop_searchpoint;

}info_is_stop;
SSMApi<info_is_stop> is_stop_searchpoint(SNAME_INFO_IS_STOP, 0);


double pan_c2fs(double pan_camera)
{
    return -1.0 * (pan_camera + 90.0);
}

double pan_fs2c(double pan_fs)
{
    return -1.0 * (pan_fs + 90.0);
}


int main( int argc, char* argv[] )
{
    //=================================================
    // ---> declaration
    FILE *fp;

    char pan_c[256];
    char tilt_c[256];
    char zoom_c[256];
    char brightness_c[256];
    char autofocus_c[256];

    double swing_pan_right = -8.0;
    double swing_pan_left = -172.0;

    double get_time;

    SSMApi<ysd::PTZ> ptz(SNAME_PTZ, 0);
    SSMApi<bool> ptz_move_start_cue;

    double interval = 0.05;

    CURL *curl;

    bool iskakukaku = true;

    vector<double> kakukaku_angs;   //angle of camera
    int split = 8;
    double pan_min_fs = -82.0;
    double pan_max_fs = 82.0;
    int ang_num = 0;
//    double pan_min_fs = -90.0;
//    double pan_max_fs = 90.0;
    double wait_time_threshold = 0.5;

    gnd::timer::interval_timer timer_operation;
    gnd::timer::stopwatch waiting_stop_watch;
    bool is_waiting_timer_started;

    cv::Mat src;
    char img_name[128];

    // <--- declaration




    //=================================================
    // ---> initialize

    // initialize kakukaku angle
    kakukaku_angs.resize(split+1);
    kakukaku_angs[0] = pan_min_fs;
    for(unsigned int i = 0; i < kakukaku_angs.size(); ++i)
    {
        kakukaku_angs[i] = pan_fs2c( pan_min_fs + (double)i*(pan_max_fs-pan_min_fs)/split );
        cerr << i << " " << kakukaku_angs[i] << "\n";   //debug
    }


    if( !initSSM() )
    {
        cerr << "SSM Error : initSSM()" << endl;
        return 0;
    }

    int ret = ptz.create(SNAME_PTZ, 0, 3.0, 0.1);
    if(ret != 1)
    {
        fprintf(stderr, "create ssm ptz fail\n");
        return -1;
    }

    if( !cam_image.openWait(0.0, SSM_READ) )
    {
        cerr << "SSM Error : cam_image.open()" << endl;
        return 1;
    }

//    if(!is_stop_searchpoint.openWait(0.0, SSM_READ))
//    {
//        cerr << "SSM Error : is_stop_searchpoint.open()" << endl;
//        iskakukaku = false;
//        return 1;
//    }

    if( !ptz_move_start_cue.openWait("ptz_move_start_cue", 0, 0.0, SSM_READ) )
    {
        cerr << "SSM Error : ptz_move_start_cue.openWait()" << endl;
        iskakukaku = false;
        return 1;
    }

    ysd::setSigInt();

    curl = curl_easy_init();
    if(!curl)
    {
        printf("couldn't init curl\n");
        return 1;
    }

    fprintf(stderr, "move to home possition...\n");
//    ret = system("curl http://172.16.30.15/axis-cgi/com/ptz.cgi?move=home");    //ホームポジション移動

    //    double tstart = ysd::time::gettimeofday_sec();
    curl_easy_setopt( curl, CURLOPT_URL, "http://172.16.30.15/axis-cgi/com/ptz.cgi?move=home");
    CURLcode curl_ret;
    curl_ret = curl_easy_perform( curl );
//    fprintf(stderr, "\ntime: %lf\n", ysd::time::gettimeofday_sec() - tstart);



    sleepSSM(2.0);
    fprintf(stderr, "PT speed setting...\n");
    ret = system("curl http://172.16.30.15/axis-cgi/com/ptz.cgi?speed=100");     //PTのスピード調節

    timer_operation.begin( CLOCK_REALTIME , 0.05 , -1.0 );


    printf("Inittialize Finished.\n");
    // <--- initialize





    //=================================================
    // ---> operation
    printf("Operation Start...\n");

    sprintf(pan_c, "curl http://172.16.30.15/axis-cgi/com/ptz.cgi?pan=%lf", -90.0);
//    printf(pan_c);
    system(pan_c);
    foward_right = true;
    bool is_first;
    bool is_first_default_pos = true;
    fprintf(stderr, "Main loop started...\n");
    while( !ysd::gShutOff )
    {   // ---> main loop

//        cerr << "time: " << ysd::time::gettimeofday_sec() << endl;

        // ---> interval timer
//        ysd::time::interval_timerSSM(interval);
//        fprintf(stderr, "time: %lf\n", ysd::time::gettimeofday_sec() );
        if( !timer_operation.clock() )
        {
//            cerr << "wait\n";
            timer_operation.wait();
        }
        // <--- interval timer

        //取得、ファイル出力


        ret = system("curl http://172.16.30.15/axis-cgi/com/ptz.cgi?query=position -o /dev/shm/ptz 2>/dev/null");  //標準エラーはゴミ

        get_time = ysd::time::gettimeofday_sec();


        //ファイルオープン
//        fprintf(stdout, "ret = %d\n", ret);
        if((fp = fopen("/dev/shm/ptz", "r")) == NULL)
        {
            fprintf(stderr, "file open error!!\n");
//            exit(EXIT_FAILURE);
        }

        //ファイルから入力
        fgets(pan_c, 256, fp);
//        printf("pan_c: %s", pan_c);
        strtok(pan_c, "=");
        ptz.data.pan = atof( strtok(NULL, "=") );

        fgets(tilt_c, 256, fp);
//        printf("tilt_c: %s", tilt_c);
        strtok(tilt_c, "=");
        ptz.data.tilt = atof( strtok(NULL, "=") );

        fgets(zoom_c, 256, fp);
//        printf("zoom_c: %s", zoom_c);
        strtok(zoom_c, "=");
        ptz.data.zoom = atof( strtok(NULL, "=") );

        fgets(brightness_c, 256, fp);
//        printf("brightness_c: %s", brightness_c);
        strtok(brightness_c, "=");
        ptz.data.brightness = atof( strtok(NULL, "=") );

        fgets(autofocus_c, 256, fp);
//        printf("autofocus_c: %s", autofocus_c);
        strtok(autofocus_c, "=");
        char *af = strtok(NULL, "=");
        ptz.data.autofocus = false;
        if(af[0] == 'o')
        {
            ptz.data.autofocus = true;
        }

        ptz.write(get_time);

        //ファイルクローズ
        fclose(fp);

        //スウィングしろよクズヤロー
        //        pan_swing(ptz.data.pan, swing_pan_left, swing_pan_right);

        //カクカクしろよごみやろー
        if(iskakukaku == true)
        {
            //司令をどこで受けるか?
            if(ptz_move_start_cue.readNew() == true && ptz_move_start_cue.data == true)
            {
                fprintf(stderr, "kakukakuします\n");

                ang_num = 0;
                sprintf(pan_c, "curl http://172.16.30.15/axis-cgi/com/ptz.cgi?pan=%lf", kakukaku_angs[ang_num]);
//                printf(pan_c);
                cerr << pan_c << "\n";
                system(pan_c);
            }
            else
            {
                if( kakukaku_angs[ang_num]-0.5 < ptz.data.pan && ptz.data.pan < kakukaku_angs[ang_num]+0.5 )    //その角度になったら
                {
                    cerr << "目標角度到達\n";
                    if(is_waiting_timer_started == false)
                    {
                        cerr << "ストップウォッチスタート\n";
                        waiting_stop_watch.begin(CLOCK_REALTIME);
                        is_waiting_timer_started = true;
                    }
                    else
                    {
                        double waiting_time,lap;
                        waiting_stop_watch.get(&waiting_time, &lap);
                        if(waiting_time > wait_time_threshold)  //しばらく待った後
                        {
                            cerr << "画像を保存します\n";
                            is_waiting_timer_started = false;
                            waiting_stop_watch.end();

                            //画像を保存
                            cam_image.readLast();
                            ImageC3_to_Mat(cam_image.data, &src);
                            sprintf( img_name, "test-%0d.jpg", ang_num);
                            cv::imwrite( img_name, src );
                            printf( "saved %s \n", img_name );
//                            cv::imwrite(img_name, src);

                            ang_num++;
                            if(ang_num == split+1)    //kakukaku終了
                            {
                                cerr << "ホームポジションに戻ります\n";
                                sprintf(pan_c, "curl http://172.16.30.15/axis-cgi/com/ptz.cgi?pan=%lf", pan_fs2c(0.0));
//                                printf(pan_c);
                                cerr << pan_c << "\n";
                                system(pan_c);
                            }
                            else
                            {
                                cerr << "次の角度に向かいます\n";
                                sprintf(pan_c, "curl http://172.16.30.15/axis-cgi/com/ptz.cgi?pan=%lf", kakukaku_angs[ang_num]);
//                                printf(pan_c);
                                cerr << pan_c << "\n";
                                system(pan_c);
                            }
                        }
                        else
                        {
                            //しばらく待つ
                            cerr << "しばらく待つ\n";
                        }
                    }
                }
                else
                {
                }
            }
        }

        //コンソール出力
//        printf("\ntime: %lf\n", get_time);
//        printf("pan: %lf\n", ptz.data.pan);
//        printf("tilt: %lf\n", ptz.data.tilt);
//        printf("zoom: %lf\n", ptz.data.zoom);
//        printf("brightness: %lf\n", ptz.data.brightness);
//        cout << "autofocus: "<< ptz.data.autofocus << endl;

    }// <--- main loop
    printf("Operation Finished.\n");
    // <--- operation




    //=================================================
    // ---> finalize
    printf("Finalize Start...\n");

    system("curl http://172.16.30.15/axis-cgi/com/ptz.cgi?move=stop");

    ret = system("rm /dev/shm/ptz");
    cerr << "/dev/shm/ptz removed" << endl;

    curl_easy_cleanup( curl );

    cam_image.close();
    ptz.release();
    if(iskakukaku == true)
    {
//        is_stop_searchpoint.close();
        ptz_move_start_cue.close();
    }
    endSSM();
    cerr << "End SSM" << endl;

    printf("Finalize Finished.\n");
    // <--- finalize



    cout << "End Successfully" << endl;
    return 0;
}
