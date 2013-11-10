//============================================================================
// Name        : main.cpp
// Author      : Shun Kawamoto (KAW)
// Version     : 0.10
// Copyright   : Your copyright notice
// Description : Stereo Processor 2
//============================================================================

#include <iostream>
#include <math.h>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <ssm.hpp>
#include <signal.h>

#include <kaw_lib/TxtConfigurator.hpp>
#include <kaw_lib/SSM-Image.hpp>
#include <kaw_lib/FrameRate.hpp>

const char *intrinsic_filename = "./intrinsics.yml";
const char *extrinsic_filename = "./extrinsics.yml";

//enum{
//    _LEFT,
//    _RIGHT
//};

//#define _DEBUG

using namespace std;
using namespace cv;

bool gShutOff;
bool isDebug;
StereoBM bm; // for block matching

//from camera_posture.conf
float height_offset = 0.0;
float depth_offset = 0.0;
double pitch_rotation = 0.0;

int tbar_WinSize = 8;
int tbar_UniqueRatio = 4;
//cv 2.1
//int tbar_Speckle_WinSize = 4;
//int tbar_Speckle_Range = 30;
//cv 2.3
int tbar_Speckle_WinSize = 30;
int tbar_Speckle_Range = 4;

int tbar_preFilter_Cap = 31;
int tbar_Texture_Threshold = 21;
int tbar_NumDisp=5;

int tbar_Disp12MaxDiff = -1;
int tbar_isSmallerWindow = 0;

void ctrlC(int aStatus)
{
    signal(SIGINT, NULL);
    gShutOff = true;
}

void paramUpdate(){

    if(tbar_WinSize < 3) tbar_WinSize = 3;
    if(tbar_UniqueRatio < 1) tbar_UniqueRatio = 1;
    if(tbar_Speckle_WinSize < 1) tbar_Speckle_WinSize = 1;
    if(tbar_Speckle_Range < 3) tbar_Speckle_Range = 3;
    if(tbar_preFilter_Cap < 1) tbar_preFilter_Cap = 1;
    if(tbar_Texture_Threshold < 1) tbar_Texture_Threshold = 1;
    if(tbar_NumDisp < 1) tbar_NumDisp = 1;

    //if(tbar_WinSize%2 != 0){
        //cout << "winSize " <<  tbar_WinSize << endl;
    bm.state->SADWindowSize = (tbar_WinSize)*2 + 1;
    //}

    bm.state->numberOfDisparities = tbar_NumDisp * 16;

    bm.state->uniquenessRatio = tbar_UniqueRatio;
    bm.state->speckleWindowSize = tbar_Speckle_WinSize * 16;
    bm.state->speckleRange = tbar_Speckle_Range;
    bm.state->textureThreshold = tbar_Texture_Threshold;
    bm.state->preFilterCap = tbar_preFilter_Cap;
    //bm.state->disp12MaxDiff = tbar_Disp12MaxDiff;
    bm.state->trySmallerWindows = tbar_isSmallerWindow;
}

// Ctrl-C による正常終了を設定
inline void setSigInt(){ signal(SIGINT, ctrlC); }

int main(int argc, char ** argv)
{
    TxtConfigurator tc("camera_posture.conf");
    float pitch_sinth=0.0,pitch_costh=0.0;

    int numberOfDisparities = 0;

    Size sizeVGA(640, 480);

    SSMApi<ImageC3> rectified_imgL(SNAME_RECT_IMGL,0);
    SSMApi<StereoC3> stereo_images(SNAME_STEREOC3, 0);
    SSMApi<StereoPoints3D> stereo_points3D(SNAME_STEREOP3D, 0);

    FileStorage fs1(intrinsic_filename, CV_STORAGE_READ);
    FileStorage fs2(extrinsic_filename, CV_STORAGE_READ);
    if(!fs1.isOpened())
    {
        cout << "Failed to open file : "<< intrinsic_filename << endl;
        return -1;
    }
    if(!fs2.isOpened())
    {
        cout << "Failed to open file : "<< extrinsic_filename << endl;
        return -1;
    }

    Rect roi1, roi2;
    Mat Q;
    Mat M1, D1, M2, D2, R, T, R1, P1, R2, P2;
    fs1["M1"] >> M1;
    fs1["D1"] >> D1;
    fs1["M2"] >> M2;
    fs1["D2"] >> D2;
    fs2["R"] >> R;
    fs2["T"] >> T;

    tc.set("HEIGHT_OFFSET", &height_offset);
    tc.set("DEPTH_OFFSET", &depth_offset);
    tc.set("PITCH_ROTATION", &pitch_rotation);
    tc.substitute();

    pitch_sinth = (float) sin((pitch_rotation / 180.0) * M_PI);
    pitch_costh = (float) cos((pitch_rotation / 180.0) * M_PI);

    //ひずみ補正と平行化のための処理
    stereoRectify( M1, D1, M2, D2, sizeVGA, R, T, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY, 0, Size(0,0), &roi1, &roi2 );

    cout << "Q:" << endl;
    cout << Q << endl;

    Mat map11, map12, map21, map22;
    initUndistortRectifyMap(M1, D1, R1, P1, sizeVGA, CV_16SC2, map11, map12);
    initUndistortRectifyMap(M2, D2, R2, P2, sizeVGA, CV_16SC2, map21, map22);

    if(!initSSM()){ cout << "SSM Initialize error." << endl; return -1;}

    setSigInt();
    if(!(stereo_images.open( SSM_READ ))){ endSSM(); return 1; }
    if(!(stereo_points3D.create(1.5, 1.0 / 15.0 ))){ endSSM(); return 1; }
    if(!(rectified_imgL.create(1.5, 1.0 / 15.0 ))){ endSSM(); return 1; }

    // --- preparation --- //

    //-16でアンドをとっているのは、-16 = -2^4なので、二進数における下４桁を０にする働きをもつ。
    numberOfDisparities = numberOfDisparities > 0 ? numberOfDisparities : ((sizeVGA.width/8) + 15) & -16;

    cout << "numOfDisp : " << numberOfDisparities<< endl;

    // --- various parameters for the block matching --- //
    bm.state->roi1 = roi1;
    bm.state->roi2 = roi2;
    bm.state->preFilterCap = tbar_preFilter_Cap;
    bm.state->SADWindowSize = tbar_WinSize;//SADWindowSize > 0 ? SADWindowSize : 15;
    bm.state->minDisparity = 0;

    bm.state->numberOfDisparities = tbar_NumDisp * 16;//numberOfDisparities;
//    bm.state->numberOfDisparities = numberOfDisparities;

    bm.state->textureThreshold = tbar_Texture_Threshold;
    bm.state->uniquenessRatio = tbar_UniqueRatio;
    bm.state->speckleWindowSize = tbar_Speckle_WinSize * 16;
    bm.state->speckleRange = tbar_Speckle_Range;
    bm.state->disp12MaxDiff = tbar_Disp12MaxDiff;
    bm.state->trySmallerWindows = tbar_isSmallerWindow;

    //cout << "NumofDis" << numberOfDisparities << endl;

#ifdef _DEBUG
    namedWindow("Left Camera Image",1);
    namedWindow("Right Camera Image",1);
    //for Trackbar
    namedWindow("Disparity",1);
    createTrackbar("WinSize(2*n+1)", "Disparity",&tbar_WinSize,16);

    createTrackbar("NumOfDisparity(16*n)", "Disparity",&tbar_NumDisp,16);
    createTrackbar("Unique Ratio", "Disparity",&tbar_UniqueRatio,15);
    createTrackbar("SpecklesFilter WinSize", "Disparity",&tbar_Speckle_WinSize,50);
    createTrackbar("SpecklesFilter Range", "Disparity",&tbar_Speckle_Range,100);
    createTrackbar("Texture Threshold", "Disparity",&tbar_Texture_Threshold,50);
    createTrackbar("preFilter Cap", "Disparity",&tbar_preFilter_Cap,50);

    //createTrackbar("disp12MaxDiff", "Disparity",&tbar_Disp12MaxDiff,10);
    createTrackbar("trySmallerWindows", "Disparity",&tbar_isSmallerWindow,1);
#endif
    Mat colorImg[2];
    Mat grayImg[2];
    Mat disp(sizeVGA,CV_32FC1),disp8(sizeVGA,CV_32FC1);
    Mat tmp_color_imgL;

    while(!gShutOff){
        Mat xyz;
        double frame_rate;

//        if(stereo_images.readNew()){
        if(stereo_images.readLast()){
            getFrameRate(&frame_rate);
            cout << "+ " <<  frame_rate << endl;

            StereoC3_to_Mat(stereo_images.data, &colorImg[_LEFT], &colorImg[_RIGHT]);

            cvtColor(colorImg[_LEFT],grayImg[_LEFT],CV_BGR2GRAY);
            cvtColor(colorImg[_RIGHT],grayImg[_RIGHT],CV_BGR2GRAY);

            //imshow("Left Raw", grayImg[_LEFT]);
            //imshow("Right Raw", grayImg[_RIGHT]);

            Mat tmp_imgL, tmp_imgR;
            remap(colorImg[_LEFT], tmp_color_imgL,map11,map12,INTER_CUBIC);
            remap(grayImg[_LEFT], tmp_imgL, map11, map12, INTER_CUBIC);
            remap(grayImg[_RIGHT], tmp_imgR, map21, map22, INTER_CUBIC);

            grayImg[_LEFT] = tmp_imgL;
            grayImg[_RIGHT] = tmp_imgR;
        }else{
           // cout << "else" << endl;
        }

#ifdef _DEBUG
        imshow("Left Camera Image", grayImg[_LEFT]);
        imshow("Right Camera Image", grayImg[_RIGHT]);
#endif

        paramUpdate();

        bm(grayImg[_LEFT],grayImg[_RIGHT],disp);

        //if( alg != STEREO_VAR )
        disp.convertTo(disp8, CV_8U, 255/(numberOfDisparities*16.));
        //disp.convertTo(disp8, CV_8U, 255/((tbar_NumDisp * 16)*16.));
        //disp.convertTo(disp8, CV_8U);

#ifdef _DEBUG
        //namedWindow("disparity", CV_WINDOW_FREERATIO);
        imshow("Disparity", disp8);
#endif

        //3次元への再投影
        reprojectImageTo3D(disp, xyz, Q, true);

        //ロボット座標系に変換
        const double max_z = 1.0e4;

        //debug
        //static bool isFirst = true;

        for(int y = 0; y < xyz.rows; y++)
        {
            for(int x = 0; x < xyz.cols; x++)
            {
                Vec3f point = xyz.at<Vec3f>(y, x);
                if(fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z){
                    stereo_points3D.data.image[x][y][0] = -1.0;
                    stereo_points3D.data.image[x][y][1] = 0.0;
                    stereo_points3D.data.image[x][y][2] = 0.0;
                }
                //debug
                /*
                else if(point[2] > -2000.0){
                    if(isFirst){
                        cout << point[2] << endl;
                    }
                }
                */

                stereo_points3D.data.image[x][y][0] =  (float) ((-point[2] + depth_offset)*pitch_costh - point[1]*pitch_sinth) / 10.0;
                stereo_points3D.data.image[x][y][1]  = (float) (point[0]*pitch_costh) / 10.0;
                stereo_points3D.data.image[x][y][2]  = (float) ((-point[2] + depth_offset)*pitch_sinth + point[1]*pitch_costh) / 10.0 + height_offset;
            }
        }

        //debug
        //isFirst = false;

        //memcpy(stereo_points3D.data.image,(uchar *)xyz.data,sizeof(float) * 640 * 480 * 3);
        //3D point cloud の書き込み
        stereo_points3D.write(stereo_images.time);

        //Rectified Left Image の書き込み
        memcpy(rectified_imgL.data.image, tmp_color_imgL.data, sizeof(unsigned char) * 640* 480 * 3);
        rectified_imgL.write(stereo_images.time);

        // 途中で止まらぬよう、コメントアウトしてしまいました。
        if(waitKey(1) == 27){
          //  gShutOff = true;
            cout << "Currently Esc to End is Not Available." << endl;
        }
        //usleepSSM(20000);
    }

    rectified_imgL.release();
    stereo_points3D.release();
    stereo_images.close();

    endSSM();
    cout << "\nEnd Successfully." << endl;
    cerr << "end." << endl;
    return 0;
}
