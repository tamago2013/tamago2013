//-------------------------------------------------
//target-recognizer.hpp
//YSD
//since: 2013-11-09
//-------------------------------------------------


#ifndef TARGET_RECOGNIZER_HPP_
#define TARGET_RECOGNIZER_HPP_


#include <stdio.h>
#include <iostream>
#include <algorithm>

//#include <kaw_lib/SSM-Image.hpp>

#include "opencv2/opencv.hpp"
#include "cvblob.h"

#include <vector>

#include "ysd-color-detector.hpp"
//#include "ysd-time.hpp"
//#include "ysd-signal.hpp"

#include "ssm-cluster.hpp"


#define WIDTH 640
#define HEIGHT 480

#define CENTER_X 320
#define CENTER_Y 240
#define RECT_WIDTH 640
#define RECT_HEIGHT 480

using namespace std;
//using namespace cv;   //これやめた方がいいだろ。何使ってるかわからなくなっちゃう

namespace color_detector
{   // ---> namespace color_detector

typedef struct x_y
{
    union
    {
        int x;
        int c;
    };
    union
    {
        int y;
        int r;
    };

}x_y;

typedef struct size
{
    int width;
    int height;
}size;

}   // <--- namespace color_detector

namespace target_recognizer
{   // ---> namespace target_recognizer

//
void make_dst_image(cv::Mat &src, cv::Mat* out, cv::Rect detect_rect, ysd::color_detector::threshold_HSV threshold, cv::Scalar flame_color)
{
    // ---> ROI設定
    cv::Mat src_roi = src(detect_rect);
    cv::Mat mask = src.clone();
    cv::Mat mask_roi = mask(detect_rect);
    // <--- ROI設定

    cv::Mat gray_src;

    cvtColor(src, gray_src, CV_BGR2GRAY);  //出力画像の背景を白黒にす
    cvtColor(gray_src, gray_src, CV_GRAY2BGR);

    ysd::color_detector::color_detct(src_roi, mask_roi, threshold);

    //    cv::imshow("mask", mask);


    cv::Mat white_rect(cv::Size(src.cols, src.rows), CV_8UC3, cv::Scalar(0,0,0));
    cv::rectangle(white_rect, cv::Point(detect_rect.x, detect_rect.y),
                  cv::Point(detect_rect.x + detect_rect.width, detect_rect.y + detect_rect.height)
                  , white, -1, CV_AA);
    cv::bitwise_and(mask, white_rect, mask);

    cv::bitwise_and(gray_src, white_rect, gray_src);
    src.copyTo(gray_src, mask);
    gray_src.copyTo(*out, white_rect);

    //    cv::imshow("out", *out);

    cv::Mat color_mask;
    cv::Mat color_img(cv::Size(src.cols, src.rows), CV_8UC3, white);    //yellow
    color_img.copyTo(color_mask, mask);
    cv::bitwise_and(color_mask, white_rect, color_mask);
    color_mask.copyTo(*out, white_rect);


    //    cv::Mat color_mask_on_gray = gray_src.clone();
    //    color_mask.copyTo(color_mask_on_gray, mask);

    //    cv::rectangle(color_mask_on_gray, cv::Point(detect_rect.x, detect_rect.y),
    //                  cv::Point(detect_rect.x + detect_rect.width, detect_rect.y + detect_rect.height),
    //                  cv::Scalar(255,255,255)/*BGR*/, 2/*太さ*/, 4);

    //    *out = color_mask_on_gray.clone();


    cv::rectangle(*out, cv::Point(detect_rect.x, detect_rect.y),
                  cv::Point(detect_rect.x + detect_rect.width, detect_rect.y + detect_rect.height),
                  flame_color/*BGR*/, 2/*太さ*/, 4);
}

}   // <--- namespace target_recognizer


inline void blob_detect(cv::Mat &src, cvb::CvBlobs* blobs)
{
    IplImage img = src;
    IplImage *chB = cvCreateImage(cvGetSize(&img), 8, 1);
    IplImage *labelImg = cvCreateImage(cvGetSize(&img), IPL_DEPTH_LABEL, 1);

    cvSplit(&img, chB, NULL, NULL, NULL);

    unsigned int result = cvb::cvLabel(chB, labelImg, *blobs);

    cvReleaseImage(&chB);
    cvReleaseImage(&labelImg);
}

inline bool isin(int x, int y, int w, int h)
{
    if(   (0 <= x) && (x <= 640)
       && (0 <= y) && (y <= 480)
       && (0 <= (x + w)) && ((x + w) <= 640)
       && (0 <= (y + h)) && ((y + h) <= 480) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

namespace target_recognizer
{   // ---> namespace target_recognizer

    static const double A = 320.0;
    static const double B = 1.0 / tan(M_PI * (51.6 /2.0) / 180.0);

    void find_range_pos(ysd::_rect target_pos, double *min_x, double *min_y, double *max_x, double *max_y)
    {
        if (target_pos.y1 <= 0.0 && target_pos.y2 <= 0.0)       //右にいる
        {
            *max_x = min(target_pos.x1, target_pos.x2);
            *max_y = min(target_pos.y1, target_pos.y2);
            *min_x = max(target_pos.x1, target_pos.x2);
            *min_y = max(target_pos.y1, target_pos.y2);
//            fprintf(stderr, "min (%lf, %lf)\n", *min_x, *min_y);
//            fprintf(stderr, "max (%lf, %lf)\n", *max_x, *max_y);
        }
        else if(target_pos.y1 > 0.0 && target_pos.y2 > 0.0)     //左にいる
        {
            *max_x = max(target_pos.x1, target_pos.x2);
            *max_y = min(target_pos.y1, target_pos.y2);
            *min_x = min(target_pos.x1, target_pos.x2);
            *min_y = max(target_pos.y1, target_pos.y2);
//            fprintf(stderr, "min (%lf, %lf)\n", *min_x, *min_y);
//            fprintf(stderr, "max (%lf, %lf)\n", *max_x, *max_y);
        }
        else                                                    //ほぼ正面にいる
        {
            *max_x = min(target_pos.x1, target_pos.x2);
            *max_y = min(target_pos.y1, target_pos.y2);
            *min_x = min(target_pos.x1, target_pos.x2);
            *min_y = max(target_pos.y1, target_pos.y2);
//            fprintf(stderr, "min (%lf, %lf)\n", *min_x, *min_y);
//            fprintf(stderr, "max (%lf, %lf)\n", *max_x, *max_y);
        }
    }

    double C = 2.0*640.0/(M_PI*51.6/180.0);

    int calc_pixel_width(double distance, double real_width)
    {
        return C*atan2(real_width, 2.0*distance);
    }

    bool target_recognize( cv::Mat src,      //最適な角度で取得された画像
                           cv::Mat *out,     //描画用の画像
                           const double camera_pan,       //srcのときのカメラpan角度[rad]
                           const ysd::_rect *target_pos    //target_の位置 <~~ camera座標系
                           /*, color_threshold*/ )
    {
        fprintf(stderr, "pan %lf[deg]\n", 180 * camera_pan / M_PI);
        fprintf(stderr, "target_pos\n");
        fprintf(stderr, "G(%lf, %lf), 1(%lf, %lf), 2(%lf, %lf)\n", target_pos->x_g, target_pos->y_g,
                target_pos->x1, target_pos->y1, target_pos->x2, target_pos->y2);

//        cv::imshow("src", src);
//        cv::waitKey(0);
//        return true;    //debug


        //declaration
//        ysd::color_detector::threshold_HSV threshold_cap(17.000, 177.000, 190.000, 63.000, 255.000, 235.000);
//        ysd::color_detector::threshold_HSV threshold_cap(41, 157, 255, 42, 255, 66); //全て含む

        ysd::color_detector::threshold_HSV threshold_cap(35.000, 170.000, 255.000, 19.000, 255.000, 19.000); //test/7.jpg

//        ysd::color_detector::threshold_HSV threshold_cap(13.000, 157.000, 255.000, 50.000, 255.000, 66.000); //2
//        ysd::color_detector::threshold_HSV threshold_cap(120.000, 82.000, 157.000, 0.000, 217.000, 65.000); //blue


        double cap_real_width_min = 0.15;
        double cap_real_width_max = 0.40;
        double distance = sqrt( (target_pos->x_g*target_pos->x_g)+(target_pos->y_g*target_pos->y_g) );
        double cap_pixel_width_min = calc_pixel_width(distance, cap_real_width_min);
        double cap_pixel_width_max = calc_pixel_width(distance, cap_real_width_max);

        cv::Mat mask_cap;
//        ysd::color_detector::color_detct(src, mask_cap, threshold_cap);
        cvb::CvBlobs blobs_cap;


        //color-thresholdでしきい値処理
        ysd::color_detector::color_detct(src, mask_cap, threshold_cap);


        //srcからtarget_posの部分を切り取る
        double max_x, max_y, min_x, min_y;
        target_recognizer::find_range_pos(*target_pos, &min_x, &min_y, &max_x, &max_y);

//        fprintf(stderr, "min (%lf, %lf)\n", min_x, min_y);  //debug
//        fprintf(stderr, "max (%lf, %lf)\n", max_x, max_y);  //debug

        double pos1_th = atan2(min_y, min_x);
        int column1 = target_recognizer::A * (1.0 - target_recognizer::B*tan(pos1_th-camera_pan) );
        double pos2_th = atan2(max_y, max_x);
        int column2 = target_recognizer::A * (1.0 - target_recognizer::B*tan(pos2_th-camera_pan) );

        cerr << "column1 " << column1 << "\n";  //debug
        cerr << "column2 " << column2 << "\n";  //debug

        if(0 > column1 || column1 > 640 || 0 > column2 || column2 > 640 || column1 >= column2)
        {
            cerr << "はんいがおかしい！！！\n";
//            return false;
        }

        if(0 > column1)
        {
            column1 = 0;
            cerr << "1\n";
        }
        if(column1 > 640)
        {
            column1 = 640;
            cerr << "2\n";
        }
        if(0 > column2)
        {
            column2 = 0;
            cerr << "3\n";
        }
        if(column2 > 640)
        {
            column2 = 640;
            cerr << "4\n";
        }
        if(column1 >= column2)
        {
            int a = column1;
            column1 = column2;
            column2 = a;
            cerr << "5\n";
        }

        cv::rectangle(*out, cv::Point(column1, 0), cv::Point(column2, 240), cv::Scalar(255,255,255), 2, 4); //debug
//        cerr << "!\n";
//        cv::imshow("out", *out);    //debug

        cv::Rect cap_detect_area(column1, 0, column2-column1, 240);    //(x, y, w, h)
        cv::Mat cap_detect_rect = mask_cap(cap_detect_area);

//        cv::imshow("cap_detect_rect", cap_detect_rect); //debug

        //blob detect
        IplImage img = cap_detect_rect;    //debug
        IplImage *labelImg = cvCreateImage(cvGetSize(&img), IPL_DEPTH_LABEL, 1);  //debug

        blob_detect(cap_detect_rect, &blobs_cap);

        cvb::cvRenderBlobs(labelImg, blobs_cap, &img, &img);    //debug
        cv::Mat cap_detect = cv::cvarrToMat(&img);              //debug

        cvb::cvFilterByArea(blobs_cap, 5, 100000);

        cv::imshow("cap_detect", cap_detect);
        bool ret = false;

        //判断してreturn true/false
        for(cvb::CvBlobs::iterator it = blobs_cap.begin(); it != blobs_cap.end(); ++it)
        { // ---> for each cap blob
            cvb::CvBlob *blob_cap=(*it).second;

            double width = blob_cap->maxx - blob_cap->minx;
            double hight = blob_cap->maxy - blob_cap->miny;

//            fprintf(stderr, "\n---------------------------------------\n");
            fprintf(stderr, "max(%d, %d) min(%d, %d) ", blob_cap->maxx, blob_cap->maxy, blob_cap->minx, blob_cap->miny);
            fprintf(stderr, "centroid(%lf, %lf) ", blob_cap->centroid.x, blob_cap->centroid.y);
            cerr << "blob->area " << blob_cap->area;
            cerr << " blob->label " << blob_cap->label;
            cerr << "\n";


//            if(/*width > 1.7*hight && */blob_cap->area > 25)    //これも買えないと
            if( cap_pixel_width_min <= (blob_cap->maxx-blob_cap->minx)
                    && (blob_cap->maxx-blob_cap->minx) <= cap_pixel_width_max )
            {
                cerr << "HIT!\n";
                ret = true;
            }
        }

        //描画用の画像をoutに格納

//        cv::waitKey(0);     //debug

        return ret;
//        return true;
    }

}   // ---> namespace target_recognizer



#endif
