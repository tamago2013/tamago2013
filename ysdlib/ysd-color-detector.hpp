#include "opencv2/opencv.hpp"
#include <stdio.h>

namespace ysd
{	// ---> namespace ysd
namespace color_detector
{   // ---> namespace color_color_detector

//using namespace cv;
//using namespace std;

#define CV_2_RGB( r, g, b ) cv::Scalar( b, g, r )

#define black CV_2_RGB( 0, 0, 0)
#define gray CV_2_RGB( 128, 128, 128)
#define silver CV_2_RGB( 192, 192, 192)
#define white CV_2_RGB( 255, 255, 255)

#define red CV_2_RGB( 255, 0, 0)
#define yellow CV_2_RGB( 255, 255, 0)
#define limey CV_2_RGB( 0, 255, 0)
#define aqua CV_2_RGB( 0, 255, 255)

#define blue CV_2_RGB( 0, 0, 255)
#define fuchsia CV_2_RGB( 255, 0, 255)
#define maroon CV_2_RGB( 128, 0, 0)
#define olive CV_2_RGB( 128, 128, 0)

#define green CV_2_RGB( 0, 128, 0)
#define teal CV_2_RGB( 0, 0, 128)
#define navy CV_2_RGB( 0, 128, 128)
#define purple CV_2_RGB( 128, 0, 128)

class max_min
{
public:
    double max, min;
};

class threshold_HSV
{
public:
    max_min h, s, v;

    threshold_HSV(){}
    ~threshold_HSV(){}

    threshold_HSV(double h_max, double h_min, double s_max, double s_min, double v_max, double v_min)
    {
        this->h.max = h_max;
        this->h.min = h_min;
        this->s.max = s_max;
        this->s.min = s_min;
        this->v.max = v_max;
        this->v.min = v_min;
    }

    void set_threshold(double h_max, double h_min, double s_max, double s_min, double v_max, double v_min)
    {
        this->h.max = h_max;
        this->h.min = h_min;
        this->s.max = s_max;
        this->s.min = s_min;
        this->v.max = v_max;
        this->v.min = v_min;
    }



    void print(void)
    {
//        printf("threshold\n");
//        printf("H(max, min): (%.03lf, %.03lf)\n", this->h.max, this->h.min);
//        printf("S(max, min): (%.03lf, %.03lf)\n", this->s.max, this->s.min);
//        printf("V(max, min): (%.03lf, %.03lf)\n", this->v.max, this->v.min);
//                printf("(max, min): H(%.03lf, %.03lf), S(%.03lf, %.03lf), V(%.03lf, %.03lf)\n",
//                       this->h.max, this->h.min, this->s.max, this->s.min, this->v.max, this->v.min);
                printf("%.03lf, %.03lf, %.03lf, %.03lf, %.03lf, %.03lf\n",
                       this->h.max, this->h.min, this->s.max, this->s.min, this->v.max, this->v.min);

    }


};

void set_H_offset(cv::Mat &input, int offset)
{   // ---> set H offset
    uchar* const tmp = input.data;
    for(int r = 0; r < input.rows; ++r)
    {
        for(int c = 0; c < input.cols; ++c)
        {
            unsigned int i = 3*(r * input.cols + c);
            if((tmp[i] + offset) > 180)
            {
                tmp[i] = tmp[i] - 180 + offset;
            }
            else
            {
                tmp[i] = tmp[i] + offset;
            }
        }
    }
}   // <--- set H offset

void offset_H_threshold(ysd::color_detector::threshold_HSV* threshold, int offset)
{   // ---> offset H threshold
    if((threshold->h.max + offset) > 180)
    {
        threshold->h.max = threshold->h.max - 180 + offset;
    }
    else
    {
        threshold->h.max = threshold->h.max + offset;
    }

    if((threshold->h.min + offset) > 180)
    {
        threshold->h.min = threshold->h.min - 180 + offset;
    }
    else
    {
        threshold->h.min = threshold->h.min + offset;
    }
}   // <--- offset H threshold


//void color_detct(Mat &src, Mat &out, int threshold[][2])
void color_detct(cv::InputArray src, cv::OutputArray out, threshold_HSV threshold)
{   // ---> color_detect
//    Mat src2 = src.getMat();

    cv::Mat /*mask,*/ work, src_HSV;
    std::vector <cv::Mat> HSV_split;
    std::vector <cv::Mat> HSV_mask;
    HSV_mask.resize(3);

    cv::cvtColor(src, src_HSV, CV_BGR2HSV);
    // ---> set H offset
    uchar* const tmp = src_HSV.data;
    for(int r = 0; r < src_HSV.rows; ++r)
    {
        for(int c = 0; c < src_HSV.cols; ++c)
        {
            unsigned int offset = 3*(r * src_HSV.cols + c);
            if((tmp[offset] + 30) > 180)
            {
                tmp[offset] = tmp[offset] - 180 + 30;
            }
            else
            {
                tmp[offset] = tmp[offset] + 30;
            }
        }
    }
    // <--- set H offset

    // ---> modify H threshold
    if((threshold.h.max + 30) > 180)
    {
        threshold.h.max = threshold.h.max - 180 + 30;
    }
    else
    {
        threshold.h.max = threshold.h.max + 30;
    }
    if((threshold.h.min + 30) > 180)
    {
        threshold.h.min = threshold.h.min - 180 + 30;
    }
    else
    {
        threshold.h.min = threshold.h.min + 30;
    }
    // <--- modify H threshold

    //    cv::cvtColor(src_HSV, mask, CV_HSV2BGR);  //なんだこれ?
    cv::split( src_HSV, HSV_split );

    //これによりしきい値いないの画素は255、以外の画素は0になる
    cv::threshold( HSV_split[0], HSV_split[0], threshold.h.max, 0, cv::THRESH_TOZERO_INV );
    cv::threshold( HSV_split[0], HSV_mask[0], threshold.h.min, 255, cv::THRESH_BINARY );
    //    imshow("HSV_mask[0]", HSV_mask[0]);

    cv::threshold( HSV_split[1], HSV_split[1], threshold.s.max, 0, cv::THRESH_TOZERO_INV );
    cv::threshold( HSV_split[1], HSV_mask[1], threshold.s.min, 255, cv::THRESH_BINARY );
    //    imshow("HSV_mask[1]", HSV_mask[1]);

    cv::threshold( HSV_split[2], HSV_split[2], threshold.v.max, 0, cv::THRESH_TOZERO_INV );
    cv::threshold( HSV_split[2], HSV_mask[2], threshold.v.min, 255, cv::THRESH_BINARY );
    //    imshow("HSV_mask[2]", HSV_mask[2]);

    cv::bitwise_and(HSV_mask[0], HSV_mask[1], work);
//    bitwise_and(HSV_mask[0], HSV_mask[1], src2);

    cv::Mat out_;
    cv::bitwise_and(work, HSV_mask[2], out_);

//    std::cerr<< "out.channels(): " << out.channels() << std::endl;
    cv::cvtColor(out_, out_, CV_GRAY2BGR);
//    std::cerr<< "out.channels(): " << out.channels() << std::endl;

    out_.copyTo(out);
//    cv::imshow("ycd_out_", out_ );
//    cv::imshow("ycd_out", out );

}   // <--- color_detect

int count_color_detect(cv::InputArray src, ysd::color_detector::threshold_HSV color_threshold_HSV)
{   // ---> count_color_detect

    cv::Mat src_HSV;
    int count = 0;

//    cv::imshow("src@ccd", src); //debug

    //-----------------------
    //convert BGR to HSV
    //-----------------------
    cv::cvtColor(src, src_HSV, CV_BGR2HSV);

    //-----------------------
    //set H offset
    //-----------------------
    ysd::color_detector::set_H_offset(src_HSV, 30);

//    cv::Mat tmp;                            //debug
//    cv::cvtColor(src_HSV, tmp, CV_HSV2BGR); //debug
//    cv::imshow("offset@ccd", tmp);          //debug

    //-----------------------
    //offset H threshold
    //-----------------------
    ysd::color_detector::offset_H_threshold(&color_threshold_HSV, 30);

    uchar* const p = src_HSV.data;
    unsigned int offset;    // = 3*(r * src_HSV.cols + c);
    for(int r = 0; r < src_HSV.rows; ++r)
    {
        offset = 3*(r * src_HSV.cols);
        for(int c = 0; c < src_HSV.cols; ++c)
        {
//            printf("(%d, %d) %d %d ", c, r, offset, 3*(r * src_HSV.cols + c));  //debug
//            printf("src_HSV.at<Vec3b>(y, x)[0] : %3d, %3d, %3d", src_HSV.at<cv::Vec3b>(r, c)[0], src_HSV.at<cv::Vec3b>(r, c)[1], src_HSV.at<cv::Vec3b>(r, c)[2]);   //debig
//            printf("p[offset] : %3d, %3d, %3d\n", p[offset], p[offset+1], p[offset+2]); //debug

            if( (color_threshold_HSV.h.max >= p[offset]) && (p[offset] >= color_threshold_HSV.h.min)                //H
                    && (color_threshold_HSV.s.max >= p[offset+1]) && (p[offset+1] >= color_threshold_HSV.s.min)     //S
                    && (color_threshold_HSV.v.max >= p[offset+2]) && (p[offset+2] >= color_threshold_HSV.v.min))    //V
            {   //hit!!!
                ++count;
            }
            offset = offset + 3;
        }
    }

//    printf("src_HSV.at<Vec3b>(y, x)[0] : %d, %d, %d\n", src_HSV.at<cv::Vec3b>(r, c)[0], src_HSV.at<cv::Vec3b>(r, c)[1], src_HSV.at<cv::Vec3b>(r, c)[2]);
//    printf("p[offset] : %d, %d, %d\n", p[offset], p[offset+1], p[offset+2]);

    return count;
}   // <--- count_color_detect







}   // <--- namespace color_color_detector
}   // <--- namespace ysd
