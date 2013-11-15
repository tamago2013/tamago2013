/*

発見した探索対象者の位置を保存する。

2013-11-15 MTM

*/
#ifndef MAINCONTROLLERTARGETLOGGER_HPP
#define MAINCONTROLLERTARGETLOGGER_HPP

#include<iostream>
#include<fstream>
#include<ctime>

namespace targetlogger
{

    //-----------------------------------------------
    // データ
    const std::string filename = "target-detection-log.csv";
    std::ofstream ofs;
    bool is_initialized = false;

    //-----------------------------------------------
    // 初期化
    bool initialize()
    {
        ofs.open( filename.c_str() );
        if( ofs )
        {
            ofs << "detection-time,x,y" << std::endl;
            is_initialized = true;
            return true;
        }
        else
        {
            std::cerr << "[\033[31mERROR\033[39m]: cannot open output log file \"" << filename << "\"!\n";
            return false;
        }
    }

    //-----------------------------------------------
    // 関数本体
    void write( double target_x , double target_y )
    {
        if( !is_initialized )
        {
            initialize();
        }

        time_t now = time(NULL);
        struct tm *pnow = localtime(&now);

        ofs << pnow->tm_hour << ":" << pnow->tm_min << ":" << pnow->tm_sec << "," << target_x << "," << target_y << std::endl;
    }

}

#endif // MAINCONTROLLERTARGETLOGGER_HPP
