//============================================================================
// Name        : main.cpp
// Author      : Shun Kawamoto (KAW)
// Version     : 0.10
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include <iostream>
#include <math.h>
#include <cstring>
#include <opencv2/opencv.hpp>
#include <signal.h>
#include <ssm.hpp>
#include <dc1394/dc1394.h>

#include <kaw_lib/SSM-Image.hpp>
#include <kaw_lib/TxtConfigurator.hpp>

//32bitか64bitかの判断
#if ((ULONG_MAX) == (UINT_MAX))
# define IS32BIT
#else
# define IS64BIT
#endif

#define CONFIG_FILE "camerasLR.conf"
#define CAMERA_NUM_MAX 2

//Image Size
#define WIDTH 640
#define HEIGHT 480

//DMA(Direct Memory Access)用のバッファ。
//DMAはデバイスがCPUを介さず直接アクセスするリングバッファメモリ領域。
//キャッシュ不可。32くらいが限界。大きすぎるとエラーはく。
#define NUM_DMA_BUFFERS 8

//White Balance
#define WHITE_P_THRESHOLD 1.0
#define WHITE_GREEN_THRESHOLD 110
//#define WHITE_GREEN_MAX 240

bool gShutOff;

dc1394camera_t *cameras[CAMERA_NUM_MAX];

SSMApi<StereoC3> stereo_images(SNAME_STEREOC3, 0);

int frame_rate = 15;
int color_mode = 1;

bool isColor=true;
bool isShowImage;
bool isShowFrameRate;
bool isWhiteCalibrated;
bool isShowWhiteCalibration;
bool isShowControl;

double WhatIsGray = 125.0;
int ShutterSpeed = 100.0;

enum{
//    _LEFT=0,
//    _RIGHT=1,
    _RED=0,
    _BLUE=1
};

typedef struct{
    uint64_t GUID;
    bool found;
}Cam_Check;


void clean_up(void);

void ctrlC(int aStatus)
{
    signal(SIGINT, NULL);
    gShutOff = true;
}
// Ctrl-C による正常終了を設定
inline void setSigInt(){ signal(SIGINT, ctrlC); }

using namespace std;
using namespace cv;

void clean_up(void)
{
    for(unsigned i = 0; i < CAMERA_NUM_MAX; i++){
        dc1394_video_set_transmission(cameras[i], DC1394_OFF);
        dc1394_capture_stop(cameras[i]);
    }
    cout << "\nClean Up." << endl;
}

void convert_RGB_to_BGR(unsigned char *input, unsigned char *result){
    unsigned char *p, *q;

    p = input;
    q = result;

    if(isColor){
        for(int i=0; i < 640*480*3; i+=3, p+=3, q+=3){
            //BとRを入れかえればよい
            q[0] = p[2];
            q[1] = p[1];
            q[2] = p[0];
        }
    }
}

//左カメラ画像の中央80x80pixel(10pxごとに間引き)を基準にキャリブレーション。
//libdc1394の使用により、CIE Luv均等色空間でホワイトバランス補正をしなくてはならない。
//vが赤み。u値を上げると赤っぽくなり、下げると緑っぽくなる。ゆえに、赤と緑がつり合うように補正する。
//uが青み。v値を上げると青っぽくなり、下げると黄っぽくなる。赤と緑から黄らしさを求め、黄と青がつり合うように補正する。
bool white_balance_calibration(unsigned char *imageL_ptr){
    const double Kp = 0.45;
    const double Ki = 0.0001;
    const int WaitLoop=10;
    int sw,ew;
    int sh,eh;
    double averageR = 0.0,averageG = 0.0,averageB = 0.0;
    double averageY = 0.0;
    double Proportional[2] = {0.0, 0.0};
    static double Integral[2] = {0.0,0.0};
    static double Result[2] = {700.0, 700.0};
    unsigned char *ptr;

    //起動時には画素値が安定していないので、少し待つ。
    static int count = 0;
    if(count < WaitLoop){
        count++;
        dc1394_feature_whitebalance_set_value(cameras[_LEFT],(uint32_t)Result[_BLUE],(uint32_t)Result[_RED]);
        dc1394_feature_whitebalance_set_value(cameras[_RIGHT],(uint32_t)Result[_BLUE],(uint32_t)Result[_RED]);
        cout << "Preparing... (" << (WaitLoop-count) << ")" << endl;
        return false;
    }


    sw = WIDTH/2 - 40+1;
    ew = WIDTH/2 + 40+1;
    sh = HEIGHT/2 - 40+1;
    eh = HEIGHT/2 + 40+1;

    //パラメータ変えるとき面倒なので、pixel数もループで数える
    double pixel_num = 0.0;

    for(int i = sh; i < eh; i += 1){
        ptr = (uchar *)imageL_ptr + (3 * i * WIDTH);
        for(int k = sw; k < ew; k += 1){
            pixel_num+=1.0;
            //frame->imageではRGB (MatではBGR)
            averageR += ((double) ptr[3*k+0]);
            averageG += ((double) ptr[3*k+1]);
            averageB += ((double) ptr[3*k+2]);
        }
    }

    averageR /= pixel_num;
    averageG /= pixel_num;
    averageB /= pixel_num;

    averageY = (averageR + averageG)/2.0;

    Proportional[_BLUE] = averageY - averageB;
    Proportional[_RED] = averageG - averageR;

    Integral[_BLUE] += Proportional[_BLUE] / 15.0;
    Integral[_RED] += Proportional[_RED] / 15.0;

    if(isShowWhiteCalibration){
        cout << "-------------" << endl;
        cout << "averageR            : "  << averageR  << endl;
        cout << "averageG            : "  << averageG  << endl;
        cout << "averageB            : "  << averageB  << endl;
        //メータを表示。かっこいいから。
        cout << "Calibrating..." << endl;
        cout << "U : ";
        if(Proportional[_BLUE] > 0) for(int i=0; i < Proportional[_BLUE]; i++) cout << "■";
        else if(Proportional[_BLUE] < 0) for(int i=0; i > Proportional[_BLUE]; i--) cout << "□";
        cout << endl;
        cout << "V : ";
        if(Proportional[_RED] > 0) for(int i=0; i < Proportional[_RED]; i++) cout << "■";
        else if(Proportional[_RED] < 0) for(int i=0; i > Proportional[_RED]; i--) cout << "□";
        cout << endl;

        cout << "Proportional[_BLUE] : "  << Proportional[_BLUE] << endl;
        cout << "Proportional[_RED]  : "  << Proportional[_RED]  << endl;


        //現在の値の取得と表示
        uint32_t Current[2];
        dc1394_feature_whitebalance_get_value(cameras[_LEFT],&Current[_BLUE],&Current[_RED]);
        //dc1394_feature_whitebalance_get_value(cameras[_RIGHT],&Result[_BLUE],&Result[_RED]);
        /*
        cout << "CurrentB : ";
        for(int i=0; i < (int)Current[_BLUE]%1023 ; i+=10) cout << "|";
        cout << endl;
        cout << "CurrentR : ";
        for(int i=0; i < (int)Current[_RED]%1023 ; i+=10) cout << "|";
        cout << endl;
        */
        cout << "Current(B,R) = (" << Current[_BLUE] << ", " << Current[_RED] << ")" << endl;
    }

    //RG差、GB差がWHITE_P_THRESHOLDより小さく、
    //GがWHITE_GREEN_THRESHOLDよりも大きく、WHITE_GREEN_MAXよりも小さければ補正完了。
    if(fabs(Proportional[_RED])<WHITE_P_THRESHOLD
            && fabs(Proportional[_BLUE])<WHITE_P_THRESHOLD
            /*
                    && WHITE_GREEN_THRESHOLD < averageG
                    && averageG < WHITE_GREEN_MAX*/){
        isWhiteCalibrated = true;
        cout << "White Balance is Calibrated!!" << endl;
        return true;
    }


    //PI制御
    Result[_BLUE] += (Kp * Proportional[_BLUE]) + (Ki * Integral[_BLUE]);
    Result[_RED] += (Kp * Proportional[_RED]) + (Ki * Integral[_RED]);

    if( Result[_BLUE] > 1023.0) Result[_BLUE] = 1023.0;
    else if (Result[_BLUE] < 0.0) Result[_BLUE] = 0.0;

    if( Result[_RED] > 1023.0) Result[_RED] = 1023.0;
    else if (Result[_RED] < 0.0) Result[_RED] = 0.0;

    dc1394_feature_whitebalance_set_value(cameras[_LEFT],(uint32_t)Result[_BLUE],(uint32_t)Result[_RED]);
    dc1394_feature_whitebalance_set_value(cameras[_RIGHT],(uint32_t)Result[_BLUE],(uint32_t)Result[_RED]);

    //793,652

    //    dc1394_feature_whitebalance_set_value(cameras[_LEFT],793,652);
    //    dc1394_feature_whitebalance_set_value(cameras[_RIGHT],793,652);

    return false;
}

void camera_control(unsigned char *imageL_ptr){
    const double Kp = 0.45;
    const double Ki = 0.0001;
    double Proportional = 0.0;
    static double Integral=0.0;
    static double Result_gain=53.0;
    //    static double Result_shutter=200.0;
    const int x_offset = 20, y_offset=20;
    //double gray_average = 0.0;
    double gray_max=0.0, gray_min=256.0,gray_mid = 128.0;
    //const double d = (1.0 * HEIGHT - y_offset*2.0)/10.0 * (1.0 * WIDTH - x_offset*2.0)/10.0;
    uint32_t val1,val2,val3;
    unsigned char *ptr;

    if(!isColor) return;

    for (int y = y_offset ; y < ( HEIGHT - y_offset); y+=10) {
        ptr = (uchar *)imageL_ptr + (y * WIDTH);
        for (int x = x_offset; x < WIDTH - x_offset; x+=10) {

            //普通に三色合計の平均でgrayにするとうまくいかない。
            //double gray_val = (ptr[x*3] + ptr[x*3+1] + ptr[x*3+2])/ (3.0);

            //代わりにOpenCVみたく、グレースケールへの変換はRGB値を巧妙に混ぜる。
            //GRAY = (0.299*R) + (0.587*G) + (0.114*B)
            double gray_val = (0.299 * (double) ptr[x*3+0]) + (0.587 * (double) ptr[x*3+1]) + (0.114 * (double) ptr[x*3+2]);

            //			gray_average += (ptr[x*3] + ptr[x*3+1] + ptr[x*3+2])/ (3.0*d);
            if(gray_max < gray_val) gray_max = gray_val;
            if(gray_min > gray_val) gray_min = gray_val;
        }
    }

    gray_mid = (gray_max + gray_min)/2.0;

    //Averageを中央値にして、扱える色の範囲を大きくしておきたい。
    //Proportional = 128.0 - gray_average;
    Proportional = WhatIsGray - gray_mid;
    Integral += Proportional / 15.0;

    Result_gain += Kp * Proportional + Ki * Integral;
    //Result_shutter += Kp * Proportional + Ki * Integral;

    if(Result_gain > 800){
        Result_gain = 800;
    }else if(Result_gain <= 5){
        Result_gain = 5;
    }

    /*
    if(Result_shutter > 800){
        Result_shutter = 800;
    }else if(Result_shutter <= 5){
        Result_shutter = 5;
    }
    */

    if(isShowControl){
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
        //cout << "gray_average : " << gray_average << endl;
        cout << "gray_mid : " << gray_mid << endl;
        cout << "Proportional : " << Proportional << endl;
        cout << "Result_gain          : " << Result_gain << endl;
        //cout << "Result_shutter       : " << Result_shutter << endl;
    }

    //範囲は0から4096まででもそこまでいかないかな。
    for(int i = 0; i < CAMERA_NUM_MAX; i++){
        //gainは上げれば上げるほど、暗いところも見れるが、ノイズが増える。
        //dc1394_feature_set_value(cameras[i], DC1394_FEATURE_GAIN, (uint32_t) Result_gain);
        //shutterは上げると、シャッター一回の時間が長くなり、シャッタースピードが落ちる。時間あたりの分解能が低いが、明るくなる。
        dc1394_feature_set_value(cameras[i], DC1394_FEATURE_SHUTTER, ShutterSpeed/*1000*/);
        //彩度
        //dc1394_feature_set_value(cameras[i], DC1394_FEATURE_SATURATION, 500/*768*/);

        dc1394_feature_get_value(cameras[i], DC1394_FEATURE_GAIN, &val1);
        dc1394_feature_get_value(cameras[i], DC1394_FEATURE_SHUTTER, &val2);
        dc1394_feature_get_value(cameras[i], DC1394_FEATURE_SATURATION, &val3);

        if(isShowControl /*&& i == 0*/){
            cout << "gain[" << i << "]       : "<< val1 << endl;
            cout << "shutter[" << i << "]    : "<< val2 << endl;
            cout << "saturation[" << i << "] : "<< val3 << endl;
            cout << "----" << endl;
        }
    }
}

//同期ズレが起きた際に、再スタートできるようにmainとは別にしてある。
bool MainLoop(uint64_t leftGUID, uint64_t rightGUID){
    unsigned long capture_time[CAMERA_NUM_MAX];
    Cam_Check left_cam,right_cam;

    left_cam.GUID = leftGUID;
    right_cam.GUID = rightGUID;

    //KAW 最適化対策。これをいれないと、検出処理がバグる
    left_cam.found =false;
    right_cam.found =false;

    dc1394_t * dc;
    dc1394error_t err;
    dc1394camera_list_t * list;
    dc1394video_frame_t *frames[CAMERA_NUM_MAX];


    //dc1394の関数を使う際には必須
    dc  = dc1394_new();
    if(!dc){
        cout << "dc1394_new error" << endl;
        return -1;
    }

    err = dc1394_camera_enumerate(dc, &list);
    DC1394_ERR_RTN(err, "Failed to enumerate cameras");

    if(list->num >= 2){
        cout << "# of cameras : " << list->num << endl;
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;

        for(unsigned i = 0; i < list->num; i++){
            if(list->ids[i].guid == left_cam.GUID){
                cout << "Found Left Camera" << endl;
                cameras[_LEFT] = dc1394_camera_new(dc, list->ids[i].guid);
                left_cam.found = true;
            }else if(list->ids[i].guid == right_cam.GUID){
                cout << "Found Right Camera" << endl;
                cameras[_RIGHT] = dc1394_camera_new(dc, list->ids[i].guid);
                right_cam.found = true;
            }
        }

        dc1394_camera_free_list(list);

        if(right_cam.found && left_cam.found){
            cout << "Found Stereo Camera!!(`･ω･´)" << endl;
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
        }
        else{
            cout << "Stereo Camera Not Found...(´･ω･`)" << endl;
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
            return 1;
        }

    }else{
        cout << "Need More Cameras...(´･ω･`)" << endl;
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
        dc1394_camera_free_list(list);
        return 1;
    }

    for(unsigned i = 0; i < CAMERA_NUM_MAX; i++){
        if(!cameras[i]){
            cout << "camera" << i << ": not found" << endl;
            return 1;
        }
    }

    //Captureのためのセットアップ
    for(int i = 0; i < CAMERA_NUM_MAX; i++){

        //Operation Mode
        err = dc1394_video_set_operation_mode(cameras[i], DC1394_OPERATION_MODE_1394B);
        DC1394_ERR_CLN_RTN(err, clean_up(), "Cannot Set Operation Mode");
        //cout << "Operation Mode = 1394B" << endl;

        //ISO Speed : ISO感度（フィルムの感度）
        err = dc1394_video_set_iso_speed(cameras[i], DC1394_ISO_SPEED_800);
        DC1394_ERR_CLN_RTN(err, clean_up(), "Cannot Set ISO Speed");
        //cout << "ISO Speed = 800" << endl;

        //Video Mode : サイズとカラー
        err = dc1394_video_set_mode(cameras[i], (dc1394video_mode_t) color_mode);
        DC1394_ERR_CLN_RTN(err, clean_up(), "Cannot Set Video Mode");
        //cout << "Image Size = VGA (640 x 480)" << endl;

        //Frame Rate : フレームレート
        err = dc1394_video_set_framerate(cameras[i], (dc1394framerate_t) frame_rate);
        DC1394_ERR_CLN_RTN(err, clean_up(), "Cannot Set Frame Rate");

        //Setup : フレームレートとモードが有効なら通るはず
        err = dc1394_capture_setup(cameras[i], NUM_DMA_BUFFERS,DC1394_CAPTURE_FLAGS_DEFAULT);
        DC1394_ERR_CLN_RTN(err, clean_up(), "Cannot Setup Cameras");

        //Transmission : 信号の送信(ON/OFF)
        err = dc1394_video_set_transmission(cameras[i], DC1394_ON);
        DC1394_ERR_CLN_RTN(err, clean_up(), "Cannot Start Transmission");

        // -- Feature Mode Setting -- //


        //Gain : カメラ感度
        //        err = dc1394_feature_set_mode(cameras[i], DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_MANUAL);
        err = dc1394_feature_set_mode(cameras[i], DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_AUTO);
        DC1394_ERR_CLN_RTN(err, clean_up(), "Cannot Set Feature : Gain");

        //Shutter : シャッタースピード
        err = dc1394_feature_set_mode(cameras[i], DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_MANUAL);
//        err = dc1394_feature_set_mode(cameras[i], DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_AUTO);
        DC1394_ERR_CLN_RTN(err, clean_up(), "Cannot Set Feature : Shutter");


        //White Balance : ホワイトバランス
        //        err = dc1394_feature_set_mode(cameras[i], DC1394_FEATURE_WHITE_BALANCE, DC1394_FEATURE_MODE_MANUAL);
        err = dc1394_feature_set_mode(cameras[i], DC1394_FEATURE_WHITE_BALANCE, DC1394_FEATURE_MODE_AUTO);
        DC1394_ERR_CLN_RTN(err, clean_up(), "Cannot Set Feature : White Balance");

        // Saturation : 彩度
        //        err = dc1394_feature_set_mode(cameras[i], DC1394_FEATURE_SATURATION, DC1394_FEATURE_MODE_MANUAL);
        err = dc1394_feature_set_mode(cameras[i], DC1394_FEATURE_SATURATION, DC1394_FEATURE_MODE_AUTO);
        DC1394_ERR_CLN_RTN(err, clean_up(), "Cannot Set Feature : Saturation");
    }

    //    cout << "Set Up Done" << endl;
    //    cout << "Transmission Starts!!" << endl;
    //    cout << "Gain, Shutter, White Balance and Saturation Are Set Manually" << endl;

    //    cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;


    //    cout << "Calibrationg White Balance..." << endl;


    bool isSync = false;
    int sync_succeed=0, sync_fail=0;

    //Main Loop
    while(!gShutOff){
        //左右のキャプチャ時間(ミリ単位UNIX時間タイムスタンプ)を格納。
        double timestamps[2];
        double dif_timestamp;

        unsigned char show_frame[640*480*3];

        for(int i = 0; i < CAMERA_NUM_MAX; i++){
            if(dc1394_capture_dequeue(cameras[i],DC1394_CAPTURE_POLICY_WAIT, &frames[i]) != DC1394_SUCCESS){
                dc1394_log_error("Failed To Capture From CAM");
            }
            capture_time[i] = (unsigned long) frames[i]->timestamp;
            timestamps[i] = (double)floor(frames[i]->timestamp / 1000.0);
        }

        //cout << "timestamps[0] : " << timestamps[_LEFT] << endl;
        //cout << "timestamps[1] : " << timestamps[_RIGHT] << endl;
        dif_timestamp = timestamps[_RIGHT] - timestamps[_LEFT];
        //cout << "dif_timestamp : " << dif_timestamp << endl;

        if(!isSync){
            if(dif_timestamp != 0){
                sync_fail++;
                cout << "Error : Seem to have a problem in Syncronization" << endl;
                if(sync_fail > 60){
                    cout << "Restart!!" << endl;
                    clean_up();
                    return false;
                }else if(sync_fail > 0){
                    cout << "This Capture will restart in " << (60 - sync_fail)/15 << " seconds..." << endl;
                }

            }else{
                sync_succeed++;
                if(sync_succeed > 45){
                    isSync = true;
                    cout << "Syncronization Succeeded!!" << endl;
                }
            }
        }

        camera_control(frames[_LEFT]->image);

//        if(!isWhiteCalibrated){
//            isWhiteCalibrated = white_balance_calibration(frames[_LEFT]->image);
//        }

        for(int i = 0; i< CAMERA_NUM_MAX;i++){
            convert_RGB_to_BGR(frames[i]->image,show_frame);
            memcpy(stereo_images.data.image[i],show_frame,sizeof(char)*WIDTH*HEIGHT*3);

            if(isShowImage){
                if(i == _LEFT){
                    Mat showImage(Size(WIDTH,HEIGHT),CV_8UC3,show_frame);
                    rectangle(showImage, Point(320-40,240-40),Point(320 + 40,240 + 40), Scalar(255,0,0),  1,  8,  0);
                    imshow("Left Camera Image", showImage);
                }
            }
        }

        stereo_images.write();

        if(isShowImage && waitKey(1) == 27){
            gShutOff = true;
        }

        if(isShowFrameRate){
            static unsigned long time_a = 0,time_b = -1;

            time_a = capture_time[0];
            //unsignedのtime_bを-1で初期化しているので、最初だけ表示がされない仕組み。
            if(time_a > time_b){
                //UNIX時間でマイクロ秒を表示しているので、1000000で割る。
                cout << "Frame Rate : " << (1000000.0 / (double)(time_a - time_b)) << endl;
            }
            time_b = time_a;
        }

        for(int i = 0; i < CAMERA_NUM_MAX; i++){
            if(frames[i]){
                //バッファに領域をエンキューして返さないとと止まる。
                dc1394_capture_enqueue(cameras[i],frames[i]);
            }
        }
    }
    clean_up();

    return true;
}

int main(int argc, char ** argv)
{
    uint64_t leftGUID,rightGUID;
    //設定ファイル読み込み用クラス by KAW
    TxtConfigurator tc(CONFIG_FILE);

    //SSM初期化
    if(!initSSM()){
        cout << "SSM Initialize error." << endl;
        return -1;
    }

    //設定ファイルの読み込み。
    //設定ファイルの読み込み。
#ifdef IS32BIT
    cout << "This is 32 bit environment" << endl;
    tc.set("LEFT_GUID",(long long *)&leftGUID);
    tc.set("RIGHT_GUID",(long long *)&rightGUID);
#else
    cout << "This is 64 bit environment" << endl;
    tc.set("LEFT_GUID",(long *)&leftGUID);
    tc.set("RIGHT_GUID",(long *)&rightGUID);
#endif
    tc.set("FRAME_RATE",&frame_rate);
    tc.set("COLOR_MOED",&color_mode);
    tc.set("SHOW_IMAGE",&isShowImage);
    tc.set("SHOW_FRAME_RATE",&isShowFrameRate);
    tc.set("SHOW_WHITE_CALIBRATION",&isShowWhiteCalibration);
    tc.set("SHOW_CONTROL",&isShowControl);
    tc.set("WHAT_IS_GRAY",&WhatIsGray);
    tc.set("SHUTTER_SPEED",&ShutterSpeed);
    tc.substitute();//必須

    tc.close(); //してもしなくてもいい。

    cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;

    switch(color_mode){
    case 0:
        cout << "Color Mode = OFF" << endl;
        color_mode = DC1394_VIDEO_MODE_640x480_MONO8;
        isColor = false;
        break;
    case 1:
        cout << "Color Mode = ON" << endl;
        color_mode = DC1394_VIDEO_MODE_640x480_RGB8;
        isColor = true;
        break;
    default:
        cout << "Color Mode Not Set : 0(Mono) 1(Color) Only" << endl;
        cout << "Color Mode = ON" << endl;
        color_mode = DC1394_VIDEO_MODE_640x480_RGB8;
        isColor = true;
        break;
    }

    switch(frame_rate){
    case 7:
        frame_rate = DC1394_FRAMERATE_7_5;
        cout << "Frame Rate = 7.5" << endl;
        break;
    case 15:
        frame_rate = DC1394_FRAMERATE_15;
        cout << "Frame Rate = 15" << endl;
        break;
    case 30:
        cout << "Frame Rate = 30" << endl;
        frame_rate = DC1394_FRAMERATE_30;
        break;
    case 60:
        frame_rate = DC1394_FRAMERATE_60;
        cout << "Frame Rate = 60" << endl;
        break;
    default:
        cout << "Frame Rate Not Set : 7(7.5), 15, 30, 60 Only" << endl;
        cout << "Frame Rate = 15" << endl;
        frame_rate = DC1394_FRAMERATE_15;
        break;
    }

    if( !stereo_images.create( 1.5, 1 / 15.0) ){
        return 1;
    }

    setSigInt();

    bool isEnd = false;
    while(!isEnd){
        cout << "Start Main Loop " << endl;
        isEnd = MainLoop(leftGUID,rightGUID);
    }

    stereo_images.release();
    endSSM();
    cout << "End Successfully." << endl;
    return 0;
}
