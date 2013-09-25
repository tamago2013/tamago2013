#include "opencv2/opencv.hpp"
#include <stdio.h>

namespace ysd{	// ---> ysd

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

    void print(void)
    {
//        printf("threshold\n");
        printf("H(max, min): (%.03lf, %.03lf)\n", this->h.max, this->h.min);
        printf("S(max, min): (%.03lf, %.03lf)\n", this->s.max, this->s.min);
        printf("V(max, min): (%.03lf, %.03lf)\n", this->v.max, this->v.min);
    }


};



//void color_detct(Mat &src, Mat &out, int threshold[][2])
void color_detct(cv::InputArray src, cv::OutputArray out, threshold_HSV threshold)
{
//    Mat src2 = src.getMat();

    cv::Mat mask, work, src_HSV;
    std::vector <cv::Mat> HSV_split;
    std::vector <cv::Mat> HSV_mask;
    HSV_mask.resize(3);

    cv::cvtColor(src, src_HSV, CV_BGR2HSV);
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

}


}       // <--- ysd
