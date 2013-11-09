//-------------------------------------------------
//camera_image_handler.cpp
//KAW, YSD
//since: 2013-11-9
//-------------------------------------------------



#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <signal.h>
#include "opencv2/opencv.hpp"
#include <ssm.hpp>
#include <kaw_lib/SSM-Image.hpp>

#define HEIGHT 480
#define WIDTH 640

using namespace std;
using namespace cv;

bool gShutOff;

void ctrlC(int aStatus)
{
    signal(SIGINT, NULL);
    gShutOff = true;
}

// Ctrl-C による正常終了を設定
inline void setSigInt(){ signal(SIGINT, ctrlC); }

int main(int argc, char ** argv)
{
	int cam_id = 0;

	Mat frm;

//	if(argc == 2){
//		cam_id = atoi(argv[1]);
//	}

	if(!initSSM()){
		cerr << "SSM Error : initSSM()" << endl;
		return 0;
	}

	SSMApi<ImageC3> cam_image(SNAME_IMGC3, 1);

    //3秒保持、10fps
	if( !cam_image.create( 3.0, 1.0/10.0 ) ){
		cerr << "SSM Error : create()" << endl;
		return 1;
	}

	setSigInt();

	VideoCapture cap("http://root:root@172.16.30.15/axis-cgi/mjpg/video.cgi?resolution=640x480&req_fps=10&.mjpg");
	if(!cap.isOpened()){
		cerr << "failed open " << endl;
		return 1;
	}
	
//	cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
//	cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
//	val = cap.get(CV_CAP_PROP_FRAME_WIDTH);
//	cout << "val : " << val << endl;


	//ループ処理の開始
	cerr << "Main Loop Started" << endl;

	while(!gShutOff){
		int key;

		cap >> frm;

//        memcpy(cam_image.data.image,frm.data,sizeof(char)*WIDTH*HEIGHT*3);
        Mat_to_ImageC3(frm,&cam_image.data);

        cam_image.write();
	//	imshow("camera",frm);


		key = waitKey(1);
		switch(key){
		case 27:
			gShutOff = true;
			break;
			/*
		case 's':
			imwrite("test.jpg",frm);
			cout << "saved test.jpg" << endl;
			break;
			*/
		default:
			break;
		}
	}
	cam_image.release();

    endSSM();
    cerr << "End SSM" << endl;
    cout << "End Successfully" << endl;
    return 0;
}
