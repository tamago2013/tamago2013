#ifndef __SSM_IMAGE2_HPP__
#define __SSM_IMAGE2_HPP__

#include <opencv2/opencv.hpp>

//VGA
#define SNAME_IMGC3 "imgC3"
#define SNAME_IMGC1 "imgC1"
#define SNAME_STEREOC3 "stereoC3"
#define SNAME_STEREOC1 "stereoC1"
#define SNAME_TRI_STEREOC3 "tri_stereoC3"
#define SNAME_TRI_STEREOC1 "tri_stereoC1"
#define SNAME_TRI_EYE "tri_eye"
#define SNAME_STEREOP3D "stereoP3D"
#define SNAME_RECT_IMGL "rect_imgL"

//SVGA
#define SNAME_IMGC3_SVGA "imgC3_SVGA"
#define SNAME_IMGC1_SVGA "imgC1_SVGA"
#define SNAME_STEREOC3_SVGA "stereoC3_SVGA"
#define SNAME_STEREOC1_SVGA "stereoC1_SVGA"
#define SNAME_TRI_EYE_SVGA "tri_eye_SVGA"
#define SNAME_STEREOP3D_SVGA "stereoP3D_SVGA"
#define SNAME_RECT_IMGL_SVGA "rect_imgL_SVGA"

//Quad-VGA
#define SNAME_IMGC3_QUAD "imgC3_Quad"
#define SNAME_IMGC1_QUAD "imgC1_Quad"
#define SNAME_STEREOC3_QUAD "stereoC3_Quad"
#define SNAME_STEREOC1_QUAD "stereoC1_Quad"
#define SNAME_STEREOMIX_QUAD "stereoMix_Quad"
#define SNAME_TRI_STEREOC3_QUAD "tri_stereoC3_Quad"
#define SNAME_TRI_STEREOC1_QUAD "tri_stereoC1_Quad"
#define SNAME_TRI_EYE_QUAD "tri_eye_Quad"
#define SNAME_STEREOP3D_QUAD "stereoP3D_Quad"
#define SNAME_RECT_IMGL_QUAD "rect_imgL_Quad"

//Mode2
#define SNAME_TRI_EYE_MODE2 "tri_eye_Mode2"
#define SNAME_STEREOP3D_MODE2 "stereoP3D_Mode2"
#define SNAME_RECT_IMGL_MODE2 "rect_imgL_Mode2"

//2eye
#define _LEFT 0
#define _RIGHT 1

//3eye
#define _UPPER 0
//#define _RIGHT 1
#define _CENTER 2

/* ////////////////////////////
//
//   SSM-Image
//
//////////////////////////// */

//typedef char* SSMImage;



/* ////////////////////////////
//
//   VGA(640x480)用
//
//////////////////////////// */

/* 3チャンネルのVGA画像 */
typedef struct
{
	//width*height*channels
	unsigned char image[640*480*3];
}ImageC3;

/* １チャンネルのVGA画像 */
typedef struct
{
	//width*height*channels
	unsigned char image[640*480*1];
}ImageC1;

/* 3チャンネルのVGAステレオ画像 */
typedef struct
{
	//width*height*channels
	unsigned char image[2][640*480*3];
}StereoC3;

/* １チャンネルのVGAステレオ画像 */
typedef struct
{
	//width*height*channels
	unsigned char image[2][640*480*1];
}StereoC1;

/* 3チャンネルのVGA三眼ステレオ画像 */
typedef struct
{
	//width*height*channels
	unsigned char image[3][640*480*3];
}TriStereoC3;

/* １チャンネルのVGA三眼ステレオ画像 */
typedef struct
{
	//width*height*channels
	unsigned char image[3][640*480*1];
}TriStereoC1;

/* 1C x 2、3C x 1のVGA三眼ステレオ画像 */
typedef struct
{
	//width*height*channels
	unsigned char imageU[640*480*1];
	unsigned char imageR[640*480*1];
	unsigned char imageC[640*480*3];
}TriEye;

/* ポイントクラウド行列 */
typedef struct
{
	//3チャンネルは画像のxyz座標に対応する、(X=0 Y=1 Z=2)
	float image[640][480][3];
}StereoPoints3D;


/* ////////////////////////////
//
//   SVGA(800x600)用
//
//////////////////////////// */

#define SVGA_WIDTH 800
#define SVGA_HEIGHT 600
#define SVGA_SIZE (SVGA_WIDTH * SVGA_HEIGHT)

/* 3チャンネルのSVGA画像 */
typedef struct
{
    //width*height*channels
    unsigned char image[SVGA_SIZE*3];
}ImageC3_SVGA;

/* １チャンネルのSVGA画像 */
typedef struct
{
    //width*height*channels
    unsigned char image[SVGA_SIZE*1];
}ImageC1_SVGA;

/* 3チャンネルのSVGAステレオ画像 */
typedef struct
{
    //width*height*channels
    unsigned char image[2][SVGA_SIZE*3];
}StereoC3_SVGA;

/* １チャンネルのSVGAステレオ画像 */
typedef struct
{
    //width*height*channels
    unsigned char image[2][SVGA_SIZE*1];
}StereoC1_SVGA;

///* 3チャンネルのSVGA三眼ステレオ画像 */
//typedef struct
//{
//    //width*height*channels
//    unsigned char image[3][SVGA_SIZE3];
//}TriStereoC3_SVGA;

///* １チャンネルのSVGA三眼ステレオ画像 */
//typedef struct
//{
//    //width*height*channels
//    unsigned char image[3][SVGA_SIZE*1];
//}TriStereoC1_SVGA;

/* 1C x 2、3C x 1のSVGA三眼ステレオ画像 */
typedef struct
{
    //width*height*channels
    unsigned char imageU[SVGA_SIZE*1];
    unsigned char imageR[SVGA_SIZE*1];
    unsigned char imageC[SVGA_SIZE*3];
}TriEye_SVGA;

/* ポイントクラウド行列 */
typedef struct
{
    //3チャンネルは画像のxyz座標に対応する、(X=0 Y=1 Z=2)
    float image[SVGA_WIDTH][SVGA_HEIGHT][3];
}StereoPoints3D_SVGA;


/* ////////////////////////////
//
//   Quad-VGA(1280x960)用
//
//////////////////////////// */

/* 3チャンネルのQuad-VGA画像 */
typedef struct
{
	//width*height*channels
	unsigned char image[1280*960*3];
}ImageC3_Quad;

/* １チャンネルのQuad-VGA画像 */
typedef struct
{
	//width*height*channels
	unsigned char image[1280*960*1];
}ImageC1_Quad;

/* 3チャンネルのQuad-VGAステレオ画像 */
typedef struct
{
	//width*height*channels
	unsigned char image[2][1280*960*3];
}StereoC3_Quad;

/* １チャンネルのQuad-VGAステレオ画像 */
typedef struct
{
	//width*height*channels
	unsigned char image[2][1280*960*1];
}StereoC1_Quad;

/* 左3チャンネル+右1チャンネルQuad-VGAステレオ画像 */
typedef struct
{
    //width*height*channels
    unsigned char imageL[1280*960*3];
    unsigned char imageR[1280*960*1];
}StereoMix_Quad;

/* 3チャンネルのQuad-VGA三眼ステレオ画像 */
typedef struct
{
	//width*height*channels
	unsigned char image[3][1280*960*3];
}TriStereoC3_Quad;

/* １チャンネルのQuad-VGA三眼ステレオ画像 */
typedef struct
{
	//width*height*channels
	unsigned char image[3][1280*960*1];
}TriStereoC1_Quad;

/* 1C x 2、3C x 1のQuad-VGA三眼ステレオ画像 */
typedef struct
{
    //width*height*channels
    unsigned char imageU[1280*960*1];
    unsigned char imageR[1280*960*1];
    unsigned char imageC[1280*960*3];
}TriEye_Quad;

/* ポイントクラウド行列 */
typedef struct
{
	//3チャンネルは画像のxyz座標に対応する、(X=0 Y=1 Z=2)
	float image[1280][960][3];
}StereoPoints3D_Quad;


/* ////////////////////////////
//
//   Format7-mode2(1280x480)用
//
//////////////////////////// */

/* 3チャンネルのFormat7-Mode2画像 */
typedef struct
{
    //width*height*channels
    unsigned char image[1280*480*3];
}ImageC3_Mode2;

/* １チャンネルのFormat7-Mode2画像 */
typedef struct
{
    //width*height*channels
    unsigned char image[1280*480*1];
}ImageC1_Mode2;

/* 1C x 2、3C x 1のformat7-Mode2三眼ステレオ画像 */
typedef struct
{
    //width*height*channels
    unsigned char imageU[1280*480*1];
    unsigned char imageR[1280*480*1];
    unsigned char imageC[1280*480*3];
}TriEye_Mode2;

/* format7-Mode2のポイントクラウド行列 */
typedef struct
{
    //3チャンネルは画像のxyz座標に対応する、(X=0 Y=1 Z=2)
    float image[1280][480][3];
}StereoPoints3D_Mode2;

/////////////////////////////////
/////////////////////////////////

//prototype declaration


//   VGA(640x480)用
void ImageC1_to_Mat(ImageC1 c1, cv::Mat *img);
void ImageC3_to_Mat(ImageC3 c3, cv::Mat *img);
void StereoC1_to_Mat(StereoC1 s1, cv::Mat *imgL, cv::Mat *imgR);
void StereoC3_to_Mat(StereoC3 s3, cv::Mat *imgL, cv::Mat *imgR);
void TriStereoC1_to_Mat(TriStereoC1 s1, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC);
void TriStereoC3_to_Mat(TriStereoC3 s3, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC);
void TriStereoC3_to_Mat(TriStereoC3 s3, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC);
void TriEye_to_Mat(TriEye t, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC);
void Mat_to_ImageC1(cv::Mat img,ImageC1 *c1);
void Mat_to_ImageC3(cv::Mat img,ImageC3 *c3);
void Mat_to_StereoC1(cv::Mat imgL,cv::Mat imgR,StereoC1 *s1);
void Mat_to_StereoC3(cv::Mat imgL,cv::Mat imgR,StereoC3 *s3);
void Mat_to_TriStereoC1(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriStereoC1 *s1);
void Mat_to_TriStereoC3(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriStereoC3 *s3);
void Mat_to_TriEye(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriEye *t);

//   SVGA(800x600)用
void ImageC1_SVGA_to_Mat(ImageC1_SVGA c1, cv::Mat *img);
void ImageC3_SVGA_to_Mat(ImageC3_SVGA c3, cv::Mat *img);
void StereoC1_SVGA_to_Mat(StereoC1_SVGA s1, cv::Mat *imgL, cv::Mat *imgR);
void StereoC3_SVGA_to_Mat(StereoC3_SVGA s3, cv::Mat *imgL, cv::Mat *imgR);
//void TriStereoC1_to_Mat(TriStereoC1_SVGA s1, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC);
//void TriStereoC3_to_Mat(TriStereoC3_SVGA s3, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC);
//void TriStereoC3_to_Mat(TriStereoC3_SVGA, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC);
void TriEye_SVGA_to_Mat(TriEye_SVGA t, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC);
void Mat_to_ImageC1_SVGA(cv::Mat img,ImageC1_SVGA *c1);
void Mat_to_ImageC3_SVGA(cv::Mat img,ImageC3_SVGA *c3);
void Mat_to_StereoC1_SVGA(cv::Mat imgL,cv::Mat imgR,StereoC1_SVGA *s1);
void Mat_to_StereoC3_SVGA(cv::Mat imgL,cv::Mat imgR,StereoC3_SVGA *s3);
//void Mat_to_TriStereoC1(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriStereoC1 *s1);
//void Mat_to_TriStereoC3(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriStereoC3 *s3);
void Mat_to_TriEye_SVGA(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriEye_SVGA *t);


//   Quad-VGA(1280x960)用
void ImageC1_Quad_to_Mat(ImageC1_Quad c1, cv::Mat *img);
void ImageC3_Quad_to_Mat(ImageC3_Quad c3, cv::Mat *img);
void StereoC1_Quad_to_Mat(StereoC1_Quad s1, cv::Mat *imgL, cv::Mat *imgR);
void StereoC3_Quad_to_Mat(StereoC3_Quad s3, cv::Mat *imgL, cv::Mat *imgR);
void StereoMix_Quad_to_Mat(StereoMix_Quad s, cv::Mat *imgL, cv::Mat *imgR);
void TriStereoC1_Quad_to_Mat(TriStereoC1 s1, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC);
void TriStereoC3_Quad_to_Mat(TriStereoC3 s3, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC);
void TriEye_Quad_to_Mat(TriEye_Quad t, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC);
void Mat_to_ImageC1_Quad(cv::Mat img,ImageC1_Quad *c1);
void Mat_to_ImageC3_Quad(cv::Mat img,ImageC3_Quad *c3);
void Mat_to_StereoC1_Quad(cv::Mat imgL,cv::Mat imgR,StereoC1_Quad *s1);
void Mat_to_StereoC3_Quad(cv::Mat imgL,cv::Mat imgR,StereoC3_Quad *s3);
void Mat_to_StereoMix_Quad(cv::Mat imgL,cv::Mat imgR,StereoC3_Quad *s);
void Mat_to_TriStereoC1_Quad(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriStereoC1_Quad *s1);
void Mat_to_TriStereoC3_Quad(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriStereoC3_Quad *s3);
void Mat_to_TriEye_Quad(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriEye_Quad *t);

//   Format7-Mode2(1280x480)用
void ImageC1_Mode2_to_Mat(ImageC1_Mode2 c1, cv::Mat *img);
void ImageC3_Mode2_to_Mat(ImageC3_Mode2 c3, cv::Mat *img);
void Mat_to_ImageC1_Mode2(cv::Mat img,ImageC1_Mode2 *c1);
void Mat_to_ImageC3_Mode2(cv::Mat img,ImageC3_Mode2 *c3);
void TriEye_Mode2_to_Mat(TriEye_Mode2 t, cv::Mat *imgU,cv::Mat *imgR,cv::Mat *imgC);
void Mat_to_TriEye_Mode2(cv::Mat imgU,cv::Mat imgR,cv::Mat imgC,TriEye_Mode2 *t);

#endif
