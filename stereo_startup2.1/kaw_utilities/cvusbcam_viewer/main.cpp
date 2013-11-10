
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

	if(argc == 2){
		cam_id = atoi(argv[1]);
	}

	if(!initSSM()){
		cerr << "SSM Error : initSSM()" << endl;
		return 0;
	}

	SSMApi<ImageC3> cam_image(SNAME_IMGC3, cam_id);

	if( !cam_image.open( ) ){
		cerr << "SSM Error : create()" << endl;
		return 1;
	}

	setSigInt();

	//ループ処理の開始
	cerr << "Main Loop Started" << endl;

	LogPolar_Interp nearest(640,480,cv::Point2i(310,230),640,3,INTER_NEAREST);

	Mat C,R;
	Mat Ctf;

	while(!gShutOff){
		char key;

		cam_image.readLast();

		ImageC3_to_Mat(cam_image.data,&frm);
		imshow("camera",frm);

		C = nearest.to_cortical(frm);	
		R = nearest.to_cartesian(C);	
		flip(C.t(),Ctf,0);
		
				imshow("C",Ctf);
				imshow("R",R);		

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
	cam_image.close();

    endSSM();
    cerr << "End SSM" << endl;
    cout << "End Successfully" << endl;
    return 0;
}