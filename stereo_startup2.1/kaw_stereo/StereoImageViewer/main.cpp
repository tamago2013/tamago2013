//============================================================================
// Name        : main.cpp
// Author      : Shun Kawamoto (KAW)
// Version     : 0.10
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <opencv2/opencv.hpp>
#include <signal.h>
#include <ssm.hpp>

#include <kaw_lib/SSM-Image.hpp>

#define SAVE_GRAY

bool gShutOff;

void ctrlC(int aStatus)
{
	signal(SIGINT, NULL);
	gShutOff = true;
}
// Ctrl-C による正常終了を設定
inline void setSigInt(){ signal(SIGINT, ctrlC); }

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    Mat imgL,imgR;
#ifdef SAVE_GRAY
    Mat grayL,grayR;
#endif

	using boost::lexical_cast;

	int save_count=1;
	string prefixL("imgL");
	string prefixR("imgR");
//	string postfix(".png");
//	string postfix(".pgm");
	string postfix(".ppm");

	string saveL,saveR;

    SSMApi<StereoC3> stereo_images(SNAME_STEREOC3, 0);

    if(!initSSM()){ cout << "SSM Initialize error." << endl; return -1;}

    setSigInt();
    if(!(stereo_images.open( SSM_READ ))){ endSSM(); return 1; }

    //namedWindow("Left Camera Image",0);
    //namedWindow("Right Camera Image",0);

    while(!gShutOff){

        if(stereo_images.readNew()){
            StereoC3_to_Mat(stereo_images.data, &imgL, &imgR);
            imshow("Left Camera Image", imgL);
            imshow("Right Camera Image", imgR);
        }

        char key = waitKey(1);


        switch(key){
        case 27:
            gShutOff = true;
            break;
        case 's':
            saveL = prefixL + lexical_cast<string>(save_count) + postfix;
            saveR = prefixR + lexical_cast<string>(save_count) + postfix;
#ifdef SAVE_GRAY
            cvtColor(imgL,grayL,CV_BGR2GRAY);
            cvtColor(imgR,grayR,CV_BGR2GRAY);
            imwrite(saveL.c_str(),grayL);
            imwrite(saveR.c_str(),grayR);
#else
            imwrite(saveL.c_str(),imgL);
            imwrite(saveR.c_str(),imgR);
#endif
            cout << "saved "<< saveL << ", " <<  saveR << endl;
            save_count++;
            break;
        default:
            break;
        }

        usleepSSM(20000);
    }

    stereo_images.close();

    endSSM();
    cout << "\nEnd Successfully." << endl;
    cerr << "end." << endl;
    return 0;
}
