//ssm-color-detedtor

//画像中の特定の範囲の色を見つける
//cmake, Qt-creatorで開発できるようにする
//実行時間を表示できるようにする
//マウスイベントを組み込みたい

//#include "../../ysdlib/ysd-color-detector.hpp"
//#include "../../ysdlib/ysd-time.hpp"
//#include "../../ysdlib/ysd-signal.hpp"

#include <stdio.h>
#include <iostream>
#include <ssm.hpp>
#include <math.h>
//#include <kaw_lib/SSM-Image.hpp>

using namespace std;
//using namespace cv;   //これやめた方がいいだろ。何使ってるかわからなくなっちゃう

int main( int argc, char* argv[] )
{
    // ---> initialize

/*    if( !initSSM() )
    {
        cerr << "SSM Error : initSSM()" << endl;
        return 0;
    }*/

//    ysd::setSigInt();

    // <--- initialize

    // ---> operation
   // <--- operation

    // ---> finalize
//    endSSM();
    cerr << "End SSM" << endl;

    cout << "End Successfully" << endl;
    // <--- finalize

    return 0;
}
