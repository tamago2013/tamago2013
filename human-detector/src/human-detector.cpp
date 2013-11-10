//-------------------------------------------------
//human-detector.cpp
//YSD
//since: 2013-11-9
//-------------------------------------------------

#include <stdio.h>
#include <vector>
#include <iostream>
#include <math.h>
//#include <kaw_lib/SSM-Image.hpp>

#include <stdint.h>
#include "gnd-gridmap.hpp"
//#include "gnd-bmp.hpp"
//#include "gnd-opsm.hpp"

#include <opencv2/opencv.hpp>
#include <cvblob.h>

#include <ssm.hpp>
#include "ssm-laser.hpp"
#include <ssmtype/spur-odometry.h>
#include "ssm-cluster.hpp"

//#include "ysd-color-detector.hpp"
#include "ysd-time.hpp"
#include "ysd-signal.hpp"

using namespace std;
//using namespace cv;   //これやめた方がいいだろ。何使ってるかわからなくなっちゃう

class cell
{
public:
    int acm;
    bool isacm;
    int clustering_label;

    cell()  //initialize
    {
        this->acm = 0;
        this->isacm = false;
        this->clustering_label = 0;
    }

};


int check_grid_label(gnd::gridmap::gridplane<cell> &grid_map, int r, int c)
{
    cell *p;
    if( !(p = grid_map.pointer(r, c)) )
    {//グリッドがない（はしっっこ）
        return -1;
    }
    else
    {//通常の処理
        return p->clustering_label;
    }
}

vector< vector <int> > look_up_table;

int readjust(int n_0, int n_1)
{
    int ans = n_0;
    for(int i = 0; i < look_up_table.size(); ++i)
    {
        if(look_up_table[i][0] == n_1){
            ans = i;
            //            fprintf(stderr, "%d is searched [%d][%d]\n", n_1, look_up_table[ans][0], look_up_table[ans][1]);
            look_up_table[i][1] = readjust(i, look_up_table[i][1]);
        }
    }
    return look_up_table[ans][1];
}

inline void blob_detect(cv::Mat &src, cvb::CvBlobs* blobs)
{
    IplImage img = src;
    IplImage *chB = cvCreateImage(cvGetSize(&img), 8, 1);
    IplImage *labelImg = cvCreateImage(cvGetSize(&img), IPL_DEPTH_LABEL, 1);

    cvSplit(&img, chB, NULL, NULL, NULL);

    unsigned int result = cvb::cvLabel(chB, labelImg, *blobs);

    cvReleaseImage(&chB);
    cvReleaseImage(&labelImg);
}

inline void calc_axis(cvb::CvBlob *blob, double *major_axis, double *minor_axis)
{
    //    *major_axis = sqrt(2.0*(blob->u20 + blob->u02
    //                            + sqrt((blob->u20 - blob->m01)*(blob->u20 - blob->m01) + 4.0*blob->u11*blob->u11)));
    //    *minor_axis = sqrt(2.0*(blob->u20 + blob->u02
    //                            - sqrt((blob->u20 - blob->m01)*(blob->u20 - blob->m01) + 4.0*blob->u11*blob->u11)));

    *major_axis = sqrt(2.0*(blob->n20 + blob->n02
                            + sqrt((blob->n20 - blob->m01)*(blob->n20 - blob->m01) + 4.0*blob->n11*blob->n11)));
    *minor_axis = sqrt(2.0*(blob->n20 + blob->n02
                            - sqrt((blob->n20 - blob->m01)*(blob->n20 - blob->m01) + 4.0*blob->n11*blob->n11)));
}






int main( int argc, char* argv[] )
{
    //=================================================
    // ---> declaration
    SSMApi<Spur_Odometry> ssm_odom;
    SSMSOKUIKIData3D fs;

    SSMApi<ysd::cluster> cluster;

    SSMApi<double> human_detector_start_cue;


    double acm_term = 5.0;      //[sec]
    double acm_interval = 0.1;  //[sec]
    int acm_threshold = 5;

    int diff_thresh = 50;   //いらん

    double row_size = 10.0;         //[m]
    double column_size = 10.0;      //[m]
    double cel_size_row = 0.05;     //[m]
    double cel_size_column = 0.05;  //[m]

    double distance = 15.0;

    double start_time;


    /*  //1005は差分とらない
    gnd::opsm::map_t  opsm_map;
    gnd::opsm::cmap_t cnt_map;
    gnd::bmp8_t       bmp_map;
    */

    /*//1005は差分とらない
    string fname = "../../data/LBuilding/opsm-map";
    const char *dirname = fname.c_str();
    */



    // <--- declaration


    //=================================================
    // ---> initialize

    if( !initSSM() )
    {
        cerr << "SSM Error : initSSM()" << endl;
        return -1;
    }

    int ret = ssm_odom.openWait(SNAME_GLOBAL /*SNAME_ADJUST*/, 0, 0.0, SSM_READ);
    if( ret != 1 )
    {
        fprintf(stderr, "open ssm_odom failure\n");
        return -1;
    }

    ret = fs.openWait(SSM_NAME_SOKUIKI_3D_FS, /*fs_ssm_id*/0, 0.0, SSM_READ) && fs.getProperty();	//fsのssmdデータとプロパティ情報の取得
    if( ret != 1 )
    {
        fprintf(stderr, "open sokuiki_fs failure %s\n", SSM_NAME_SOKUIKI_3D_FS);
        return -1;
    }

    cluster.create(SNAME_CLUSTER, 0, 20.0, 1);

    ret = human_detector_start_cue.openWait("human_detector_start_cue", 0, 0.0, SSM_READ);
    if( ret != 1 )
    {
        fprintf(stderr, "open human_detector_start_cue\n");
        return -1;
    }





    // ---> gnd-gridmap initialize

    //    gnd::gridmap::gridplane<cell> grid_map;
    //    grid_map.pallocate(row_size, column_size, cel_size_row, cel_size_column);
    //    ssm_odom.readLast();
    ////    grid_map.pset_origin(row_size/2.0, column_size/2.0);
    //    grid_map.pset_origin(ssm_odom.data.x, ssm_odom.data.y);
    //    cell init_cell;
    //    grid_map.set_uniform(&init_cell);
    //    fprintf(stderr, "time : %lf (sec)\n", ysd::time::gettimeofday_sec() - start);

    // <--- gnd-gridmap initialize


    /*// ---> map load    //1005は差分とらない
    if( gnd::opsm::read_counting_map(&cnt_map, dirname) < 0)
    {
        fprintf(stderr, "map death!\n");
    }
    if( gnd::opsm::build_map(&opsm_map, &cnt_map, gnd_mm2dist(1)) < 0 ) {
        fprintf(stderr, "map death!\n");
    }
    if( gnd::opsm::build_bmp(&bmp_map, &opsm_map, gnd_m2dist(1.0/10)) < 0 )
    {
        fprintf(stderr, "map death!\n");
    }
    fprintf(stderr, "まっぷ読み込み成功\n");
    */// <--- map load

    ysd::setSigInt();
    // <--- initialize



    //=================================================
    // ---> operation


    while( !ysd::gShutOff)
    {   // ---> main loop

        ysd::time::interval_timerSSM(0.5);



//        cout << "sっておして" << endl;
//        char in;
//        cin >> in;
//        if(in == 's')
//        if(human_detector_start_cue.readNew() == true)
        {
            cerr << "\n\nhuman detection start!\n";
            distance = human_detector_start_cue.data;
            distance = 5;

            // ---> make gridmap
            gnd::gridmap::gridplane<cell> grid_map;
            grid_map.pallocate(row_size, column_size, cel_size_row, cel_size_column);
            ssm_odom.readLast();
            start_time = ssm_odom.time;
//            fprintf(stderr, "grid make time : %lf\n", ssm_odom.time);   //debug
//            fprintf(stderr, "(x, y, th) : (%lf, %lf, %lf)\n", ssm_odom.data.x, ssm_odom.data.y, ssm_odom.data.theta);   //debug
            grid_map.pset_origin(ssm_odom.data.x, ssm_odom.data.y);
            cell init_cell;
            grid_map.set_uniform(&init_cell);
            // <--- make gridmap

            double start = ysd::time::gettimeofday_sec();
            while( !ysd::gShutOff )
            {  // ---> acm
                //    grid_map.pset(1.0, 1.0, &one);  //これでもいい

                // ---> intereval timer
                ysd::time::interval_timerSSM(acm_interval);
//                fprintf(stderr, "[!acm!] time: %lf\n", ysd::time::gettimeofday_sec() );
                if ((ysd::time::gettimeofday_sec() - start) > acm_term) break;   //debug用のだっっっっしゅつ
                // <--- intereval timer

                fs.readLast();
                ssm_odom.readTime(fs.time);
                double sin_theta = sin(ssm_odom.data.theta);
                double cos_theta = cos(ssm_odom.data.theta);
                cell *p;

                for(int step = 0; step < fs.property.numPoints; ++step)
                {
                    if(fs.data[step].isWarning() == false)
                    {
                        //一定距離以内をとってくる
                        if( (distance*distance) > ((fs.data[step].reflect.x*fs.data[step].reflect.x) + (fs.data[step].reflect.y*fs.data[step].reflect.y)) )
                        {
                            double scan_point_gl_x = fs.data[step].reflect.x*cos_theta - fs.data[step].reflect.y*sin_theta + ssm_odom.data.x;
                            double scan_point_gl_y = fs.data[step].reflect.x*sin_theta + fs.data[step].reflect.y*cos_theta + ssm_odom.data.y;
                            //            printf("[scan_URG] %lf %lf\n", scan_point_gl_x, scan_point_gl_y);
                            if( !(p = grid_map.ppointer(scan_point_gl_x, scan_point_gl_y)) )
                            {
                                grid_map.reallocate(scan_point_gl_x, scan_point_gl_y);
                                p = grid_map.ppointer(scan_point_gl_x, scan_point_gl_y);
                            }
                            if(p->isacm == false)
                            {
//                                cerr << "!\n";
                                p->acm = p->acm + 1;  //ここで投票
                                p->isacm = true;
                            }
                        }
                    }
                }
                for(int c=0; c<(signed)grid_map.column(); c++)
                {
                    for(int r=0; r<(signed)grid_map.row(); r++)
                    {
                        p = grid_map.pointer(r, c);
                        p->isacm = false;
                    }
                }

            }// <--- acm


            double tstart = ysd::time::gettimeofday_sec();

            fprintf(stderr, "動物体取り除く\n");
            // ---> remove moving object
            for(int c = 0; c < grid_map.column(); c++)
            {
                for(int r = 0; r < grid_map.row(); r++)
                {
                    cell *p;
                    p = grid_map.pointer(r, c);
                    if(grid_map.value(r, c).acm < acm_threshold)
                    {
                        p->acm = 0;
//                                        fprintf(stderr, "remove moving object\n");
                    }
                    else
                    {
                        p->acm = 1;
//                        fprintf(stderr, "save\n");
                    }
                }
            }
            //デバッグ用履きだっ数
//                for(int c=0; c<(signed)grid_map.column(); c++)
//                {
//                    for(int r=0; r<(signed)grid_map.row(); r++)
//                    {
//                        if( grid_map.value(r, c).acm >=diff_thresh )
//                        {
//                            double x, y;
//                            grid_map.pget_pos_core(r, c, &x, &y);
//                            printf("[scan_rm_mov] %lf %lf %d\n", x, y, grid_map.value(r, c).acm);
//                            fflush(stdout);
//                        }
//                    }
//                }

            // <--- remove moving object



            // ---> take diff
            //やらん
            //        fprintf(stderr, "差分とる\n");
            // <--- take diff

            cv::Mat current_scan = cv::Mat::zeros(grid_map.row(), grid_map.column(), CV_8U);

            for(int r = 0; r < grid_map.row(); ++r)
            {
                for(int c = 0; c < grid_map.column(); ++c)
                {
                    if(grid_map.value(r, c).acm > 0)
                    {
                        //                current_scan.at<unsigned char>(r, c) = 255;
                        current_scan.data[r * current_scan.cols + c] = 255;
                    }
                }
            }

//            cv::imwrite("test.jpg", current_scan);

            //        cv::namedWindow("current_scan", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO);
            //        cv::imshow("current_scan", current_scan);
            //        cv::imshow("a", a);

            cv::Mat current_scan_color;
            cv::cvtColor(current_scan, current_scan_color, CV_GRAY2BGR);

            cvb::CvBlobs blobs;

            blob_detect(current_scan_color, &blobs);


            //クラスタの大きさでフィルタをかける
            //min < bolb < max
            //
            //        cvb::cvFilterByArea(blobs, 5, 100000);
            for(int num = 5; blobs.size() > 32; ++num)
            {
                cvb::cvFilterByArea(blobs, num, 100000);
                printf("cluster reduction\n");
            }


//            IplImage img = current_scan_color;    //debug
//            IplImage *labelImg = cvCreateImage(cvGetSize(&img), IPL_DEPTH_LABEL, 1);  //debug
//            cvb::cvRenderBlobs(labelImg, blobs, &img, &img);    //debug
//            cv::Mat out = cv::cvarrToMat(&img); //debug
            //        cv::namedWindow("out", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO); //debug
            //        cv::imshow("out", out); //debug



            fprintf(stderr, "%d blobs\n", blobs.size());


            int cluster_num = 0;
            for(cvb::CvBlobs::iterator it = blobs.begin(); it != blobs.end(); ++it)
            { // ---> for each cap blob
                cvb::CvBlob *blob=(*it).second;
                double max_x, max_y, min_x, min_y, cent_x, cent_y, major_axis, minor_axis;

                grid_map.pget_pos_lower(blob->miny, blob->minx, &min_x, &min_y);
                grid_map.pget_pos_upper(blob->maxy, blob->maxx, &max_x, &max_y);
                grid_map.pget_pos_core(blob->centroid.y, blob->centroid.x, &cent_x, &cent_y);

                double cluster_size = sqrt((max_x - min_x)*(max_x - min_x) + (max_y - min_y)*(max_y - min_y));
                printf("[%d] pos : max(%lf, %lf) min(%lf, %lf) centroid(%lf, %lf) cluster size : %lf",
                       cluster_num, max_x, max_y, min_x, min_y, cent_x, cent_y, cluster_size);
                if(0.1 < cluster_size && cluster_size < 2.0)    //これをcondfigにしろし
                {
//                    printf("[%d] pos : max(%lf, %lf) min(%lf, %lf) centroid(%lf, %lf) cluster size : %lf\n",
//                           cluster_num, max_x, max_y, min_x, min_y, cent_x, cent_y, cluster_size);
                    fprintf(stdout, " ===> HIT!");
                    cluster.data.rect[cluster_num].set(cent_x, cent_y, min_x, min_y, max_x, max_y);
                    ++cluster_num;
                }
                fprintf(stdout, "\n");
            }
            cluster.data.set_num_clusters(cluster_num);
            //        cluster.write(start_time);
            cluster.write(start_time);


            printf("%d blobs wrote at time : %lf\n", cluster_num, cluster.time);
            fprintf(stderr, "time : %lf (sec)\n", ysd::time::gettimeofday_sec() - tstart);
            grid_map.deallocate();

        }

        // ---> debug
        //            cv::waitKey(0);   //debug
        //        bool isshutoff = false;
        //        while(!ysd::gShutOff)
        //        {
        //            char key = cv::waitKey( 10 );
        //            switch( key )
        //            {
        //            case 27:
        //                ysd::gShutOff = true;
        //                break;

        //            case 's':
        //                break;

        //            default:
        //                break;
        //            }
        //        }
        // <--- debug








    }// ---> main loop
    // <--- operation


    //=================================================
    // ---> finalize
    //    grid_map.deallocate();
    //    bmp_map.deallocate(); //1005は差分とらない

    ssm_odom.close();
    fs.close();
    human_detector_start_cue.close();
    cluster.release();
    endSSM();
    cerr << "\n" << "End SSM" << endl;

    cerr << "End Successfully" << endl;
    // <--- finalize

    return 0;
}
