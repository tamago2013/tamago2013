
#include <cstring>

#include "SSM-Image.hpp"

// New Transformation

//void SSMImage_to_Mat(SSMImage c1, cv::Mat *img, cv::Size size){
//    cv::Mat t1(size,CV_8UC1,c1.image);
//    *img=t1;
//}



/* ////////////////////////////
//
//   VGA(640x480)用
//
//////////////////////////// */

void ImageC1_to_Mat(ImageC1 c1, cv::Mat *img){
    cv::Mat t1(cv::Size(640,480),CV_8UC1,c1.image);
    *img=t1;
}

void ImageC3_to_Mat(ImageC3 c3, cv::Mat *img){
    cv::Mat t1(cv::Size(640,480),CV_8UC3,c3.image);
    *img=t1;
}

void StereoC1_to_Mat(StereoC1 s1, cv::Mat *imgL, cv::Mat *imgR){
    cv::Mat t1(cv::Size(640,480),CV_8UC1,s1.image[0]);
    cv::Mat t2(cv::Size(640,480),CV_8UC1,s1.image[1]);
    *imgL=t1;
    *imgR=t2;
}

void StereoC3_to_Mat(StereoC3 s3, cv::Mat *imgL, cv::Mat *imgR){
    cv::Mat t1(cv::Size(640,480),CV_8UC3,s3.image[0]);
    cv::Mat t2(cv::Size(640,480),CV_8UC3,s3.image[1]);
    *imgL=t1;
    *imgR=t2;
}

void TriStereoC1_to_Mat(TriStereoC1 s1, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC){
    cv::Mat t1(cv::Size(640,480),CV_8UC1,s1.image[0]);
    cv::Mat t2(cv::Size(640,480),CV_8UC1,s1.image[1]);
    cv::Mat t3(cv::Size(640,480),CV_8UC1,s1.image[2]);
    *imgU=t1;
    *imgR=t2;
    *imgC=t3;
}

void TriStereoC3_to_Mat(TriStereoC3 s3, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC){
    cv::Mat t1(cv::Size(640,480),CV_8UC3,s3.image[0]);
    cv::Mat t2(cv::Size(640,480),CV_8UC3,s3.image[1]);
    cv::Mat t3(cv::Size(640,480),CV_8UC3,s3.image[2]);
    *imgU=t1;
    *imgR=t2;
    *imgC=t3;
}

void TriEye_to_Mat(TriEye t, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC){
    cv::Mat t1(cv::Size(640,480),CV_8UC1,t.imageU);
    cv::Mat t2(cv::Size(640,480),CV_8UC1,t.imageR);
    cv::Mat t3(cv::Size(640,480),CV_8UC3,t.imageC);
    *imgU=t1;
    *imgR=t2;
    *imgC=t3;
}

void Mat_to_ImageC1(cv::Mat img,ImageC1 *c1){
    memcpy(c1->image,img.data,sizeof(char)*640*480*1);
}

void Mat_to_ImageC3(cv::Mat img,ImageC3 *c3){
    memcpy(c3->image,img.data,sizeof(char)*640*480*3);
}

void Mat_to_StereoC1(cv::Mat imgL,cv::Mat imgR,StereoC1 *s1){
    memcpy(s1->image[0],imgL.data,sizeof(char)*640*480*1);
    memcpy(s1->image[1],imgR.data,sizeof(char)*640*480*1);
}

void Mat_to_StereoC3(cv::Mat imgL,cv::Mat imgR,StereoC3 *s3){
    memcpy(s3->image[0],imgL.data,sizeof(char)*640*480*3);
    memcpy(s3->image[1],imgR.data,sizeof(char)*640*480*3);
}

void Mat_to_TriStereoC1(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriStereoC1 *s1){
    memcpy(s1->image[0],imgU.data,sizeof(char)*640*480*1);
    memcpy(s1->image[1],imgR.data,sizeof(char)*640*480*1);
    memcpy(s1->image[2],imgC.data,sizeof(char)*640*480*1);
}

void Mat_to_TriStereoC3(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriStereoC3 *s3){
    memcpy(s3->image[0],imgU.data,sizeof(char)*640*480*3);
    memcpy(s3->image[1],imgR.data,sizeof(char)*640*480*3);
    memcpy(s3->image[2],imgC.data,sizeof(char)*640*480*3);
}

void Mat_to_TriEye(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriEye *t){
    memcpy(t->imageU,imgU.data,sizeof(char)*640*480*1);
    memcpy(t->imageR,imgR.data,sizeof(char)*640*480*1);
    memcpy(t->imageC,imgC.data,sizeof(char)*640*480*3);
}

/* ////////////////////////////
//
//   SVGA(800x600)用
//
//////////////////////////// */

void ImageC1_SVGA_to_Mat(ImageC1_SVGA c1, cv::Mat *img){
    cv::Mat t1(cv::Size(800,600),CV_8UC1,c1.image);
    *img=t1;
}

void ImageC3_SVGA_to_Mat(ImageC3_SVGA c3, cv::Mat *img){
    cv::Mat t1(cv::Size(800,600),CV_8UC3,c3.image);
    *img=t1;
}

void StereoC1_SVGA_to_Mat(StereoC1_SVGA s1, cv::Mat *imgL, cv::Mat *imgR){
    cv::Mat t1(cv::Size(800,600),CV_8UC1,s1.image[0]);
    cv::Mat t2(cv::Size(800,600),CV_8UC1,s1.image[1]);
    *imgL=t1;
    *imgR=t2;
}

void StereoC3_SVGA_to_Mat(StereoC3_SVGA s3, cv::Mat *imgL, cv::Mat *imgR){
    cv::Mat t1(cv::Size(800,600),CV_8UC1,s3.image[0]);
    cv::Mat t2(cv::Size(800,600),CV_8UC1,s3.image[1]);
    *imgL=t1;
    *imgR=t2;
}

void TriEye_SVGA_to_Mat(TriEye_SVGA t, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC){
    cv::Mat t1(cv::Size(800,600),CV_8UC1,t.imageU);
    cv::Mat t2(cv::Size(800,600),CV_8UC1,t.imageR);
    cv::Mat t3(cv::Size(800,600),CV_8UC3,t.imageC);
    *imgU=t1;
    *imgR=t2;
    *imgC=t3;
}

void Mat_to_ImageC1_SVGA(cv::Mat img,ImageC1_SVGA *c1){
    memcpy(c1->image,img.data,sizeof(char)*img.step);
}


void Mat_to_ImageC3_SVGA(cv::Mat img,ImageC3_SVGA *c3){
    memcpy(c3->image,img.data,sizeof(char)*img.step);
}

void Mat_to_StereoC1_SVGA(cv::Mat imgL,cv::Mat imgR,StereoC1_SVGA *s1){
    memcpy(s1->image[0],imgL.data,sizeof(char)*imgL.step);
    memcpy(s1->image[1],imgR.data,sizeof(char)*imgL.step);
}

void Mat_to_StereoC3_SVGA(cv::Mat imgL,cv::Mat imgR,StereoC3_SVGA *s3){
    memcpy(s3->image[0],imgL.data,sizeof(char)*imgL.step);
    memcpy(s3->image[1],imgR.data,sizeof(char)*imgL.step);
}

void Mat_to_TriEye_SVGA(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriEye_SVGA *t){
    memcpy(t->imageU,imgU.data,sizeof(char)*imgU.step);
    memcpy(t->imageR,imgR.data,sizeof(char)*imgR.step);
    memcpy(t->imageC,imgC.data,sizeof(char)*imgC.step);
}

/* ////////////////////////////
//
//   Quad-VGA(1280x960)用
//
//////////////////////////// */

void ImageC1_Quad_to_Mat(ImageC1_Quad c1, cv::Mat *img){
    cv::Mat t1(cv::Size(1280,960),CV_8UC1,c1.image);
    *img=t1;
}

void ImageC3_Quad_to_Mat(ImageC3_Quad c3, cv::Mat *img){
    cv::Mat t1(cv::Size(1280,960),CV_8UC3,c3.image);
    *img=t1;
}

void StereoC1_Quad_to_Mat(StereoC1_Quad s1, cv::Mat *imgL, cv::Mat *imgR){
    cv::Mat t1(cv::Size(1280,960),CV_8UC1,s1.image[0]);
    cv::Mat t2(cv::Size(1280,960),CV_8UC1,s1.image[1]);
    *imgL=t1;
    *imgR=t2;
}

void StereoC3_Quad_to_Mat(StereoC3_Quad s3, cv::Mat *imgL, cv::Mat *imgR){
    cv::Mat t1(cv::Size(1280,960),CV_8UC3,s3.image[0]);
    cv::Mat t2(cv::Size(1280,960),CV_8UC3,s3.image[1]);
    *imgL=t1;
    *imgR=t2;
}

void StereoMix_Quad_to_Mat(StereoC3_Quad s, cv::Mat *imgL, cv::Mat *imgR){
    cv::Mat t1(cv::Size(1280,960),CV_8UC3,s.image[0]);
    cv::Mat t2(cv::Size(1280,960),CV_8UC1,s.image[1]);
    *imgL=t1;
    *imgR=t2;
}

void TriStereoC1_Quad_to_Mat(TriStereoC1 s1, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC){
    cv::Mat t1(cv::Size(1280,960),CV_8UC1,s1.image[0]);
    cv::Mat t2(cv::Size(1280,960),CV_8UC1,s1.image[1]);
    cv::Mat t3(cv::Size(1280,960),CV_8UC1,s1.image[2]);
    *imgU=t1;
    *imgR=t2;
    *imgC=t3;
}

void TriStereoC3_Quad_to_Mat(TriStereoC3 s3, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC){
    cv::Mat t1(cv::Size(1280,960),CV_8UC3,s3.image[0]);
    cv::Mat t2(cv::Size(1280,960),CV_8UC3,s3.image[1]);
    cv::Mat t3(cv::Size(1280,960),CV_8UC3,s3.image[2]);
    *imgU=t1;
    *imgR=t2;
    *imgC=t3;
}

void TriEye_Quad_to_Mat(TriEye_Quad t, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC){
    cv::Mat t1(cv::Size(1280,960),CV_8UC1,t.imageU);
    cv::Mat t2(cv::Size(1280,960),CV_8UC1,t.imageR);
    cv::Mat t3(cv::Size(1280,960),CV_8UC3,t.imageC);
    *imgU=t1;
    *imgR=t2;
    *imgC=t3;
}


void Mat_to_ImageC1_Quad(cv::Mat img,ImageC1_Quad *c1){
    memcpy(c1->image,img.data,sizeof(char)*1280*960*1);
}

void Mat_to_ImageC3_Quad(cv::Mat img,ImageC3_Quad *c3){
    memcpy(c3->image,img.data,sizeof(char)*1280*960*3);
}

void Mat_to_StereoC1_Quad(cv::Mat imgL,cv::Mat imgR,StereoC1_Quad *s1){
    memcpy(s1->image[0],imgL.data,sizeof(char)*1280*960*1);
    memcpy(s1->image[1],imgR.data,sizeof(char)*1280*960*1);
}

void Mat_to_StereoC3_Quad(cv::Mat imgL,cv::Mat imgR,StereoC3_Quad *s3){
    memcpy(s3->image[0],imgL.data,sizeof(char)*1280*960*3);
    memcpy(s3->image[1],imgR.data,sizeof(char)*1280*960*3);
}

void Mat_to_StereoMix_Quad(cv::Mat imgL,cv::Mat imgR,StereoC3_Quad *s){
    memcpy(s->image[0],imgL.data,sizeof(char)*1280*960*3);
    memcpy(s->image[1],imgR.data,sizeof(char)*1280*960*1);
}

void Mat_to_TriStereoC1_Quad(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriStereoC1_Quad *s1){
    memcpy(s1->image[0],imgU.data,sizeof(char)*1280*960*1);
    memcpy(s1->image[1],imgR.data,sizeof(char)*1280*960*1);
    memcpy(s1->image[2],imgC.data,sizeof(char)*1280*960*1);
}
void Mat_to_TriStereoC3_Quad(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriStereoC3_Quad *s3){
    memcpy(s3->image[0],imgU.data,sizeof(char)*1280*960*3);
    memcpy(s3->image[1],imgR.data,sizeof(char)*1280*960*3);
    memcpy(s3->image[2],imgC.data,sizeof(char)*1280*960*3);
}

void Mat_to_TriEye_Quad(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriEye_Quad *t){
    memcpy(t->imageU,imgU.data,sizeof(char)*1280*960*1);
    memcpy(t->imageR,imgR.data,sizeof(char)*1280*960*1);
    memcpy(t->imageC,imgC.data,sizeof(char)*1280*960*3);
}


/* ////////////////////////////
//
//   Format7-Mode2(1280x480)用
//
//////////////////////////// */

void ImageC1_Mode2_to_Mat(ImageC1_Mode2 c1, cv::Mat *img){
    cv::Mat t1(cv::Size(1280,480),CV_8UC1,c1.image);
    *img=t1;
}

void ImageC3_Mode2_to_Mat(ImageC3_Mode2 c3, cv::Mat *img){
    cv::Mat t1(cv::Size(1280,480),CV_8UC3,c3.image);
    *img=t1;
}

void Mat_to_ImageC1_Mode2(cv::Mat img,ImageC1_Mode2 *c1){
    memcpy(c1->image,img.data,sizeof(char)*1280*480*1);
}

void Mat_to_ImageC3_Mode2(cv::Mat img,ImageC3_Mode2 *c3){
    memcpy(c3->image,img.data,sizeof(char)*1280*480*3);
}


void TriEye_Mode2_to_Mat(TriEye_Mode2 t, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC){
    cv::Mat t1(cv::Size(1280,480),CV_8UC1,t.imageU);
    cv::Mat t2(cv::Size(1280,480),CV_8UC1,t.imageR);
    cv::Mat t3(cv::Size(1280,480),CV_8UC3,t.imageC);
    *imgU=t1;
    *imgR=t2;
    *imgC=t3;
}


void Mat_to_TriEye_Mode2(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriEye_Mode2 *t){
    memcpy(t->imageU,imgU.data,sizeof(char)*1280*480*1);
    memcpy(t->imageR,imgR.data,sizeof(char)*1280*480*1);
    memcpy(t->imageC,imgC.data,sizeof(char)*1280*480*3);
}
