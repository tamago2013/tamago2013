//ptz-hadler

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


using namespace std;
//using namespace cv;   //これやめた方がいいだろ。何使ってるかわからなくなっちゃう

namespace ysd {
#define SNAME_PTZ "ptz"
typedef struct {

    double pan;
    double tilt;
    double zoom;
    double brightness;
    bool autofocus;

}PTZ;
}


int main( int argc, char* argv[] )
{
    // ---> initialize
    FILE *fp;
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
    int ret = ptz.create(SNAME_PTZ, 0, 3.0, 0.1);
    if(ret != 1)
    {
        fprintf(stderr, "create ssm ptz fail\n");
        return -1;
    }

    ysd::setSigInt();

    // <--- initialize

    // ---> operation

    while( !ysd::gShutOff )
    {
//        cerr << "time: " << ysd::gettimeofday_sec() << endl;

        //取得、ファイル出力
        ret = system("curl http://172.16.30.15/axis-cgi/com/ptz.cgi?query=position -o /dev/shm/ptz 2>/dev/null");  //標準エラーはゴミ
        get_time = ysd::gettimeofday_sec();
        printf("\ntime: %lf\n", get_time);

        //ファイルオープン
//        fprintf(stdout, "ret = %d\n", ret);
        if ((fp = fopen("/dev/shm/ptz", "r")) == NULL) {
            printf("file open error!!\n");
            exit(EXIT_FAILURE);
        }

        //ファイルから入力
//        printf("おe\n");

        fgets(pan_c, 256, fp);
//        printf("pan_c: %s", pan_c);
        strtok(pan_c, "=");
        ptz.data.pan = atof( strtok(NULL, "=") );
        printf("pan: %lf\n", ptz.data.pan);
//        double pan = atof( strtok(NULL, "=") );
//        printf("pan: %lf\n", pan);

        fgets(tilt_c, 256, fp);
//        printf("tilt_c: %s", tilt_c);
        strtok(tilt_c, "=");
        ptz.data.tilt = atof( strtok(NULL, "=") );
        printf("tilt: %lf\n", ptz.data.tilt);
//        double tilt = atof( strtok(NULL, "=") );
//        printf("tilt: %lf\n", tilt);

        fgets(zoom_c, 256, fp);
//        printf("zoom_c: %s", zoom_c);
        strtok(zoom_c, "=");
        ptz.data.zoom = atof( strtok(NULL, "=") );
        printf("zoom: %lf\n", ptz.data.zoom);
//        double zoom = atof( strtok(NULL, "=") );
//        printf("zoom: %lf\n", zoom);

        fgets(brightness_c, 256, fp);
//        printf("brightness_c: %s", brightness_c);
        strtok(brightness_c, "=");
        ptz.data.brightness = atof( strtok(NULL, "=") );
        printf("brightness: %lf\n", ptz.data.brightness);
//        double brightness = atof( strtok(NULL, "=") );
//        printf("brightness: %lf\n", brightness);

        fgets(autofocus_c, 256, fp);
//        printf("autofocus_c: %s", autofocus_c);
        strtok(autofocus_c, "=");
        char *af = strtok(NULL, "=");
        ptz.data.autofocus = false;
        if(af[0] == 'o')
        {
            ptz.data.autofocus = true;
        }
        cout << "autofocus: "<< ptz.data.autofocus << endl;

        ptz.write(get_time);
//        printf("/おe\n");

        //ファイルクローズ
        fclose(fp);

        usleep(29 * 1000);

    }
    // <--- operation

    // ---> finalize
    ret = system("rm /dev/shm/ptz");
    cerr << "/dev/shm/ptz removed" << endl;

    ptz.release();
        endSSM();
    cerr << "End SSM" << endl;

    cout << "End Successfully" << endl;
    // <--- finalize

    return 0;
}
