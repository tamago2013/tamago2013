//ssm-ptz-checker

//画像中の特定の範囲の色を見つける
//cmake, Qt-creatorで開発できるようにする
//実行時間を表示できるようにする
//マウスイベントを組み込みたい

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ssm.hpp>
#include <math.h>
#include <string.h>

//#include "../../ysdlib/ysd-color-detector.hpp"
#include "../../ysdlib/ysd-time.hpp"
#include "../../ysdlib/ysd-signal.hpp"
#include "ssm-ptz.hpp"

using namespace std;
//using namespace cv;   //これやめた方がいいだろ。何使ってるかわからなくなっちゃう

//namespace ysd {
//#define SNAME_PTZ "ptz"
//typedef struct {

//    double pan;
//    double tilt;
//    double zoom;
//    double brightness;
//    bool autofocus;

//}PTZ;
//}

int main( int argc, char* argv[] )
{
    // ---> initialize
    char pan_c[256];
    char tilt_c[256];
    char zoom_c[256];
    char brightness_c[256];
    char autofocus_c[256];
    double get_time;
    if( !initSSM() )
    {
        cerr << "SSM Error : initSSM()" << endl;
        return 0;
    }

    SSMApi<ysd::PTZ> ptz(SNAME_PTZ, 0);
    int ret = ptz.openWait(SNAME_PTZ, 0, 0.0, SSM_READ);
    if(ret != 1)
    {
        fprintf(stderr, "open ssm ptz fail\n");
        return -1;
    }

    ysd::setSigInt();

    // <--- initialize

    // ---> operation

    while( !ysd::gShutOff )
    {
//        cerr << "time: " << ysd::gettimeofday_sec() << endl;

        //取得、ファイル出
//        get_time = ysd::gettimeofday_sec();
//        printf("\ntime: %lf\n", get_time);

        ptz.readLast();
        printf("time: %lf\n", ptz.time);
        printf("pan: %lf\n", ptz.data.pan);
        printf("tilt: %lf\n", ptz.data.tilt);
        printf("zoom: %lf\n", ptz.data.zoom);
        printf("brightness: %lf\n", ptz.data.brightness);
        printf("autofocus: %d\n", ptz.data.autofocus);

        usleep(100 * 1000);

    }
    // <--- operation

    // ---> finalize

    ptz.close();
        endSSM();
    cerr << "End SSM" << endl;

    cout << "End Successfully" << endl;
    // <--- finalize

    return 0;
}
