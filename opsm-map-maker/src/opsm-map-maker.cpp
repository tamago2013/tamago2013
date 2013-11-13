/*
 * opsm-map-maker.cpp
 *
 *  Created on: 2013/09/27
 *      Author: tyamada
 */


#include <stdio.h>
#include <time.h>

#include <ssmtype/spur-odometry.h>
#include <ssm.hpp>
#include <ssm-log.hpp>
#include "ssm-laser.hpp"

#include "opsm-map-maker-opt.hpp"
#include "opsm-map-maker-cui.hpp"

#include "gnd-coord-tree.hpp"
#include "gnd-matrix-coordinate.hpp"
#include "gnd-matrix-base.hpp"

#include "gnd-opsm.hpp"
#include "gnd-odometry-correction.hpp"
#include "gnd-gridmap.hpp"
#include "gnd-shutoff.hpp"
#include "gnd-timer.hpp"
#include "gnd-bmp.hpp"


static const double ShowCycle = gnd_sec2time(1.0);
static const double ClockCycle = gnd_sec2time(1.0) / 1000.0 ;

int main(int argc, char* argv[]) {
	gnd::opsm::cmap_t			cnt_smmap;			// probabilistic scan matching counting map

	SSMScanPoint2D				ssm_sokuikiraw;		// sokuiki raw streaming data
	SSMApi<Spur_Odometry>		ssm_pos;			// position


	gnd::matrix::coord_tree coordtree;				// coordinate tree
	int coordid_gl = -1,							// global coordinate node id
			coordid_rbt = -1,							// robot coordinate node id
			coordid_sns = -1;							// sensor coordinate node id

	gnd::cui_reader gcui;							// cui manager

	gnd::odometry::cmap cmap;

	opsm_mm::proc_configuration pconf;			// configuration parameter
	opsm_mm::options popt(&pconf);				// process option analyze class

	FILE *llog_fp = 0;

	{ // ---> initialization
		int ret;								// function return value
		uint32_t phase = 1;						// initialize phase

		// ---> read process options
		if( (ret = popt.get_option(argc, argv)) != 0 ) {
			return ret;
		} // <--- read process options


		{ // ---> coordinate-tree set robot coordinate
			gnd::coord_matrix cc; // coordinate relation matrix

			// set global coordinate
			gnd::matrix::set_unit(&cc);
			coordid_gl = coordtree.add("global", "root", &cc);

			// set robot coordinate
			gnd::matrix::set_unit(&cc);
			coordid_rbt = coordtree.add("robot", "global", &cc);

			// set scan matching coordinate
			gnd::matrix::set_unit(&cc);
			coordid_sns = coordtree.add("sensor", "robot", &cc);
		} // <--- coordinate-tree set robot coordinate



		{ // ---> allocate SIGINT to shut-off
			::proc_shutoff_clear();
			::proc_shutoff_alloc_signal(SIGINT);
		} // <--- allocate SIGINT to shut-off



		{ // ---> show initialize task
			::fprintf(stderr, "==========Initialize==========\n");
			if( *pconf.init_map.value ) {
				::fprintf(stderr, " %d. load initial map from \"\x1b[4m%s\x1b[0m\"\n", phase++, pconf.init_map.value);
			}

			::fprintf(stderr, " %d. Init ssm\n", phase++ );
			::fprintf(stderr, " %d. Open ssm-data \"\x1b[4m%s\x1b[0m\"\n", phase++, pconf.ls_name.value);
			::fprintf(stderr, " %d. Open ssm-data \"\x1b[4m%s\x1b[0m\"\n", phase++, pconf.pos_name.value);
			::fprintf(stderr, "\n\n");
		} // <--- show initialize task


		// ---> load odometry initial map
		if( !::is_proc_shutoff() ) {
			if ( *pconf.init_map.value ){
				::fprintf(stderr, "\n");
				::fprintf(stderr, " => load initial map from \"\x1b[4m%s\x1b[0m\"\n", pconf.init_map.value);
				if( gnd::opsm::read_counting_map(&cnt_smmap, pconf.init_map.value) < 0){
					::proc_shutoff();
					::fprintf(stderr, "  ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to load scan matching map \"\x1b[4m%s\x1b[0m\"\n", pconf.init_map.value);
				}
				else {
					::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m: load scan matching map \"\x1b[4m%s\x1b[0m\"\n", pconf.init_map.value);
				}
			}
			else {

			}

		} // <--- load odometry initial map


		// ---> initialize ssm
		if(!::is_proc_shutoff()){
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => Initailize SSM\n");
			if( !::initSSM() ){
				::proc_shutoff();
				::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to initialize \x1b[4mssm\x1b[0m\n");
			}
			else {
				::fprintf(stderr, " ...\x1b[1mOK\x1b[0m\n");
			}
		} // <--- initialize ssm



		// ---> open ssm position
		if( !::is_proc_shutoff() ){
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => load ssm-data \"\x1b[4m%s\x1b[0m\"\n", pconf.pos_name.value);
			if( !(*pconf.pos_name.value) ){
				// shut off
				::proc_shutoff();
				::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: missing log file operand\n" );
			}
			else {
				::fprintf(stderr, "    File \"\x1b[4m%s\x1b[0m\"\n", pconf.pos_name.value);

				if( !ssm_pos.openWait( pconf.pos_name.value, pconf.odm_id.value, 0.0, SSM_READ ) ){
					::proc_shutoff();
					::fprintf(stderr, "  [\x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m]: fail to open ssm-data \"\x1b[4m%s\x1b[0m\"\n", pconf.pos_name.value);
				}
				else {
					::fprintf(stderr, "   ...\x1b[1mOK\x1b[0m: Open ssm-data \"\x1b[4m%s\x1b[0m\"\n", pconf.pos_name.value);
					{ // ---> set coordinate
						gnd::matrix::fixed<4,4> pos_cc;

						gnd::matrix::coordinate_converter(&pos_cc,
								0, 0, 0,
								::cos(0), ::sin(0), 0,
								 0, 0, 1);

						coordtree.set_coordinate(coordid_rbt, &pos_cc);
					} // ---> set coordinate
					ssm_pos.setBlocking(true);
					ssm_pos.readLast();
				}
			}
		} // <--- open ssm position



		// ---> open ssm sokuiki raw data
		if(!::is_proc_shutoff()){
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => load ssm-data \"\x1b[4m%s\x1b[0m\"\n", pconf.ls_name.value);

			if( ! *pconf.ls_name.value ){
				::proc_shutoff();
				::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: missing log file operand\n" );
			}
			else {
				::fprintf(stderr, "    File \"\x1b[4m%s\x1b[0m\"\n", pconf.pos_name.value);

				if( !ssm_sokuikiraw.openWait( pconf.ls_name.value, pconf.ls_id.value, 0.0, SSM_READ ) ){
					::proc_shutoff();
					::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to load ssm \"\x1b[4m%s\x1b[0m\"\n", pconf.ls_name.value);
				}
				// get property
				else if( !ssm_sokuikiraw.getProperty() ){
					::proc_shutoff();
					::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to get property of \"\x1b[4m%s\x1b[0m\"\n", pconf.ls_name.value);
				}
				else {
					//					ssm_sokuikiraw.setBlocking(true);

					// allocate
					ssm_sokuikiraw.data.alloc(ssm_sokuikiraw.property.numPoints);

					// define coordinate
					coordtree.set_coordinate(coordid_sns, &ssm_sokuikiraw.property.coordm);

					ssm_sokuikiraw.readLast();
					::fprintf(stderr, " ... \x1b[1mOK\x1b[0m\n");
				}
			}
		} // <--- open ssm sokuiki raw data


		// ---> make output directory
		if( !::is_proc_shutoff() && *pconf.output_dir.value ) {
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => make output directory \"\x1b[4m%s\x1b[0m\"\n", pconf.output_dir.value);

			errno = 0;
			if( mkdir(pconf.output_dir.value, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH ) < 0) {
				if( errno != EEXIST ) {
					::proc_shutoff();
					::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to make output directory \"\x1b[4m%s\x1b[0m\"\n", pconf.output_dir.value);
				}
				else {
					struct stat st;
					::stat(pconf.output_dir.value, &st);

					if( S_ISDIR(st.st_mode)) {
						::fprintf(stderr, " ...\x1b[1mOK\x1b[0m: output directory \"\x1b[4m%s\x1b[0m\" is already exist\n", pconf.output_dir.value);
					}
					else {
						::proc_shutoff();
						::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: \"\x1b[4m%s\x1b[0m\" is already exist and it is not directory\n", pconf.output_dir.value);
					}
				}
			}
			else {
				::fprintf(stderr, " ...\x1b[1mOK\x1b[0m: make output directory \"\x1b[4m%s\x1b[0m\"\n", pconf.output_dir.value);
			}
		} // <--- make output directory

		if ( !::is_proc_shutoff() && *pconf.laserpoint_log.value) {
			char fname[512];
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => open laser point log file\n");

			if( ::snprintf(fname, sizeof(fname), "%s/%s", *pconf.output_dir.value ? pconf.output_dir.value : "./", pconf.laserpoint_log.value) == sizeof(fname) ){
				::proc_shutoff();
				::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: file path is too long\n");
			}
			else if( !(llog_fp = fopen( fname, "w" )) ) {
				::proc_shutoff();
				::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to open \"\x1b[4m%s\x1b[0m\"\n", fname);
			}
			else {
				::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m\n");
			}
		}

		// set cui command
		gcui.set_command(opsm_mm::cui_cmd, sizeof(opsm_mm::cui_cmd) / sizeof(opsm_mm::cui_cmd[0]));


		// fin of initialization
		::fprintf(stderr, "\n\n");
	} // <--- initialization












	// ---> operation
	if(!::is_proc_shutoff() ){

		Spur_Odometry prev_pos = ssm_pos.data; 			// previous odometry position
		ssmTimeT prev_time = 0;							// previous map update time




		double culling_sqdist							// data decimation threshold
		= gnd_square( pconf.culling.value );

		gnd::vector::fixed_column<3> move_opt;			// position estimation movement by optimization

		gnd::matrix::fixed<4,4> coordm_sns2rbt;			// coordinate convert matrix from sensor to robot
		gnd::matrix::fixed<4,4> coordm_sns2gl;			// coordinate convert matrix from sensor to global

		double cuito = 0;								// blocking time out for cui input

		gnd::timer::interval_timer timer_clock;			// clock
		gnd::timer::interval_timer timer_operate;			// operation timer
		gnd::timer::interval_timer timer_show;			// time operation timer
		int nline_show;

		int cnt_mapupdate = 0;


		// get coordinate convert matrix
		coordtree.get_convert_matrix(coordid_sns, coordid_rbt, &coordm_sns2rbt);

		// ---> memory allocate counting map
		if( !cnt_smmap.plane[0].is_allocate() ){
			gnd::opsm::init_counting_map(&cnt_smmap, 0.4, 10);
		} // <--- memory allocate counting map


		{ // ---> timer
			// set parameter-cycle
			timer_show.begin(CLOCK_REALTIME, ShowCycle, -ShowCycle);
			nline_show = 0;
			timer_show.begin(CLOCK_REALTIME, ShowCycle, -ShowCycle);
		} // <--- timer


		// ---> operation loop
		while ( !::is_proc_shutoff() ) {
			//			timer_clock.wait();

			{ // ---> cui
				int cuival = 0;
				char cuiarg[512];
				// zero reset buffer
				::memset(cuiarg, 0, sizeof(cuiarg));

				// ---> get command
				if( gcui.poll(&cuival, cuiarg, sizeof(cuiarg), cuito) > 0 ){
					if( timer_show.cycle() > 0 ){
						// quit show status mode
						timer_show.end();
						nline_show = 0;
						::fprintf(stderr, "-------------------- cui mode --------------------\n");
					}
					else { // ---> cui command operation
						switch(cuival){
						// exit
						case 'Q': ::proc_shutoff(); break;
						// help
						default:
						case '\0':
						case 'h': gcui.show(stderr, "   "); break;
						// show status
						case 's': {
							// console clear
							timer_show.begin(CLOCK_REALTIME, ShowCycle, -ShowCycle);
							nline_show = 0;
							break;
						}
						// start
						case 't':{
							cuito = 0.0;
						} break;
						// stand-by
						case 'B':{
							::fprintf(stderr, "   stand-by mode\n");
							cuito = -1;
						} break;

						}
					} // <--- cui command operation
					::fprintf(stderr, "  > ");
					gcui.poll(&cuival, cuiarg, sizeof( cuiarg ), 0);
				} // <--- get command
			}  // <--- cui


			// ---> show status
			if( timer_show.clock() > 0){
				// back cursor
				if( nline_show ) {
					::fprintf(stderr, "\x1b[%02dA", nline_show);
					nline_show = 0;
				}

				nline_show++; ::fprintf(stderr, "\x1b[K-------------------- \x1b[1m\x1b[36m%s\x1b[39m\x1b[0m --------------------\n", opsm_mm::proc_name);
				nline_show++; ::fprintf(stderr, "\x1b[K        position : %4.03lf[m], %4.03lf[m], %4.02lf[deg]\n",
						ssm_pos.data.x, ssm_pos.data.y, gnd_ang2deg( ssm_pos.data.theta ) );
				nline_show++; ::fprintf(stderr, "\x1b[K      map update : %d\n", cnt_mapupdate );
				if( pconf.use_range_dist.value > 0){
					nline_show++; ::fprintf(stderr, "\x1b[K*      use range : %lf [m]\n", pconf.use_range_dist.value );
				}
				if( pconf.use_range_orient.value > 0){
					nline_show++; ::fprintf(stderr, "\x1b[K*      use range : %lf [deg]\n", pconf.use_range_orient.value );
				}
				nline_show++; ::fprintf(stderr, "\x1b[K\n");
				nline_show++; ::fprintf(stderr, "\x1b[K Push \x1b[1mEnter\x1b[0m to change CUI Mode\n");
			} // <--- show status


			// ---> read ssm-sokuikiraw-data
			if( ssm_pos.readNew()  ) {
				// ---> position tracking
				// ... operation flow
				//		 0. check movement
				//       1. get scan data
				//		 2. update robot position for coordinate convert
				//		 3. convert scan data to global coordinate and add to map

				// 0. check movement
				if( ssm_pos.time - prev_time < pconf.pause_time.value &&
						gnd_square( ssm_pos.data.x - prev_pos.x ) + gnd_square( ssm_pos.data.y - prev_pos.y ) < culling_sqdist &&
						::fabs( ssm_pos.data.theta - prev_pos.theta ) < pconf.pause_orient.value ) continue;

				// 1. get laser scanner reading
				if( !ssm_sokuikiraw.readTime( ssm_pos.time ) ) continue;

				// update previous data
				prev_pos = ssm_pos.data;
				prev_time = ssm_pos.time;


				{ // ---> 2. update robot position for coordinate convert
					gnd::matrix::fixed<4,4> coordm;

					// robot position coordinate
					gnd::matrix::coordinate_converter(&coordm,
							ssm_pos.data.x, ssm_pos.data.y, 0,
							::cos( ssm_pos.data.theta ), ::sin( ssm_pos.data.theta ), 0,
							 0, 0, 1);
					coordtree.set_coordinate(coordid_rbt, &coordm);
				} // ---> 2. update robot position for coordinate convert


				{ // ---> 3. convert scan data to global coordinate and add to map
					// get coordinate convert matrix
					coordtree.get_convert_matrix(coordid_sns, coordid_gl, &coordm_sns2gl);

					cnt_mapupdate++;

					{// ---> scanning loop for sokuikiraw-data
						gnd::vector::fixed_column<4> reflect_csns;
						gnd::vector::fixed_column<4> reflect_cgl;
						gnd::vector::fixed_column<4> reflect_prev;

						// ---> scanning loop of laser scanner reading
						for(size_t i = 0; i < ssm_sokuikiraw.data.numPoints(); i++){
							gnd::vector::fixed_column<3> ws3x1;
							gnd::matrix::fixed<3,3> ws3x3;

							// if range data is null because of no reflection
							if( ssm_sokuikiraw.data[i].status == ssm::laser::STATUS_NO_REFLECTION)	continue;
							// ignore error data
							else if( ssm_sokuikiraw.data[i].isError()) 	continue;
							else if( ssm_sokuikiraw.data[i].r < ssm_sokuikiraw.property.distMin)	continue;
							else if( ssm_sokuikiraw.data[i].r > ssm_sokuikiraw.property.distMax)	continue;
							else if( pconf.use_range_dist.value > 0 && ssm_sokuikiraw.data[i].r > pconf.use_range_dist.value )		continue;
							else if( pconf.use_range_orient.value > 0 && ssm_sokuikiraw.data[i].th > pconf.use_range_orient.value )	continue;



							{ // ---> compute reflection point position on global coordinate
								// set search position on sensor-coordinate
								gnd::matrix::set(&reflect_csns, 0, 0, ssm_sokuikiraw.data[i].r * ::cos( ssm_sokuikiraw.data[i].th ));
								gnd::matrix::set(&reflect_csns, 1, 0, ssm_sokuikiraw.data[i].r * ::sin( ssm_sokuikiraw.data[i].th ) );
								gnd::matrix::set(&reflect_csns, 2, 0, 0);
								gnd::matrix::set(&reflect_csns, 3, 0, 1);

								// data culling with distance threshold
								if( gnd_square(reflect_csns[0] - reflect_prev[0]) + gnd_square(reflect_csns[1] - reflect_prev[1]) < culling_sqdist ){
									continue;
								}
								else {
									// update previous entered data
									gnd::matrix::copy(&reflect_prev, &reflect_csns);
								}

								// convert from sensor coordinate to global coordinate
								gnd::matrix::prod(&coordm_sns2gl, &reflect_csns, &reflect_cgl);
							} // <--- compute laser scanner reading position on global coordinate

							// add scan point to map
							gnd::opsm::counting_map(&cnt_smmap, reflect_cgl[0], reflect_cgl[1]);

							// log
							if( llog_fp )	::fprintf(llog_fp, "%lf %lf\n", reflect_cgl[0], reflect_cgl[1]);
						} // <--- scanning loop of laser scanner reading
						// log
						if( llog_fp )	::fprintf(llog_fp, "\n" );
					} // <--- scanning loop for sokuikiraw-data
				} // <--- 3. convert scan data to global coordinate and add to map

			} // <--- read ssm sokuikiraw

		} // <--- operation loop

	} // <--- operation



	{ // ---> finalization
		gnd::opsm::map_t smmap;
		if(llog_fp) ::fclose(llog_fp);

		ssm_pos.close();
		ssm_sokuikiraw.close();
		::endSSM();


		// ---> build map
		gnd::opsm::build_map(&smmap, &cnt_smmap, gnd_mm2dist(10), gnd_m2dist(20));

		if( pconf.opsm_map.value[0] ){ // ---> write opsm map
			char dname[512];
			bool flg = false;

			::fprintf(stderr, " => write intermediate file\n");
			::sprintf(dname, "%s/%s/", *pconf.output_dir.value ? pconf.output_dir.value : "./", pconf.opsm_map.value);

			errno = 0;
			if( mkdir(dname, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH ) < 0) {
				if( errno != EEXIST ) {
					::proc_shutoff();
					::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to make output directory \"\x1b[4m%s\x1b[0m\"\n", dname);
				}
				else {
					struct stat st;
					::stat(pconf.output_dir.value, &st);

					if( S_ISDIR(st.st_mode)) {
						::fprintf(stderr, " ...\x1b[1mOK\x1b[0m: output directory \"\x1b[4m%s\x1b[0m\" is already exist\n", dname);
						flg = true;
					}
					else {
						::proc_shutoff();
						::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: \"\x1b[4m%s\x1b[0m\" is already exist and it is not directory\n", dname);
					}
				}
			}
			else {
				::fprintf(stderr, "  ... make output directory \"\x1b[4m%s\x1b[0m\"\n", dname);
				flg = true;
			}


			if( flg && gnd::opsm::write_counting_map(&cnt_smmap, dname) ) {
				::fprintf(stderr, "  ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open\n");
			}
			else {
				::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m: save counting map data\n");
			}
		} // <--- write opsm map



		if( pconf.bmp.value ) { // ---> bmp (32bit)
			gnd::bmp32_t bmp;

			// bmp file building
			gnd::opsm::build_bmp32(&bmp, &smmap, gnd_m2dist( 1.0 / 10));
			{ // ---> bmp
				char fname[512];
				::fprintf(stderr, " => write psm-image in bmp(32bit)\n");

				if( ::snprintf(fname, sizeof(fname), "%s/%s.%s", pconf.output_dir.value, "out", "bmp" ) == sizeof(fname) ){
					::fprintf(stderr, "  ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open. file name is too long\n");
				}
				else if( gnd::bmp::write32(fname, &bmp) < 0) {
					::fprintf(stderr, "  ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open \"\x1b[4m%s\x1b[0m\"\n", fname);
				}
				else {
					::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m: save map data into \"\x1b[4m%s\x1b[0m\"\n", fname);
				}
			} // <--- bmp

			{ // ---> origin
				char fname[512];
				FILE *fp = 0;
				double x, y;

				if( ::snprintf(fname, sizeof(fname), "%s/%s.%s", pconf.output_dir.value, "out", "origin.txt"  ) == sizeof(fname) ){
					::fprintf(stderr, "  ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open. file name is too long\n");
				}
				else if( !(fp = fopen(fname, "w")) ) {
					::fprintf(stderr, "  ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open \"\x1b[4m%s\x1b[0m\"\n", fname);
				}

				bmp.pget_origin(&x, &y);
				fprintf(fp, "%lf %lf\n", x, y);
				fclose(fp);
			} // --->  origin
		} // <--- bmp (32bit)


		if( pconf.bmp.value ) { // ---> bmp (8bit)
			gnd::bmp8_t bmp8;

			gnd::opsm::build_bmp8(&bmp8, &smmap, gnd_m2dist( 1.0 / 10));
			{ // ---> bmp
				char fname[512];
				::fprintf(stderr, " => write psm-image in bmp(8bit)\n");

				if( ::snprintf(fname, sizeof(fname), "%s/%s.%s", pconf.output_dir.value, "out8", "bmp" ) == sizeof(fname) ){
					::fprintf(stderr, "  ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open. file name is too long\n");
				}
				else if( gnd::bmp::write8(fname, &bmp8) < 0) {
					::fprintf(stderr, "  ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open \"\x1b[4m%s\x1b[0m\"\n", fname);
				}
				else {
					::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m: save map data into \"\x1b[4m%s\x1b[0m\"\n", fname);
				}
			} // <--- bmp

			{ // ---> origin
				char fname[512];
				FILE *fp = 0;
				double x, y;

				if( ::snprintf(fname, sizeof(fname), "%s/%s.%s", pconf.output_dir.value, "out8", "origin.txt"  ) == sizeof(fname) ){
					::fprintf(stderr, "  ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open. file name is too long\n");
				}
				else if( !(fp = fopen(fname, "w")) ) {
					::fprintf(stderr, "  ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open \"\x1b[4m%s\x1b[0m\"\n", fname);
				}
				bmp8.pget_origin(&x, &y);
				fprintf(fp, "%lf %lf\n", x, y);
				fclose(fp);
			} // --->  origin
		} // ---> bmp (8bit)


		::fprintf(stderr, "\n");
		::fprintf(stderr, "Finish\n");

	} // <--- finalization

	return 0;
}


