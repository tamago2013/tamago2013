//============================================================================
// Name        : obserbation-probability-position-tracker.cpp
// Author      : tyamada
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <time.h>

#include <ssmtype/spur-odometry.h>
#include "ssm-laser.hpp"
#include "ssm-ekf.hpp"

#include "opsm-position-tracker-opt.hpp"
#include "opsm-position-tracker-viewer.hpp"

#include "yp-coordinate-manager.hpp"
#include "yp-matrix-coordinate-convert.hpp"
#include "yp-matrix-base.hpp"

#include "gnd-observation-probability.hpp"
#include "gnd-cui.hpp"
#include "gnd-gridmap.hpp"
#include "gnd-shutoff.hpp"
#include "gnd-time.hpp"
#include "gnd-bmp.hpp"


const struct gnd::CUI::command cui_cmd[] = {
		{"Quit",					'Q',	"localizer shut-off"},
		{"help",					'h',	"show help"},
		{"show",					's',	"state show mode"},
		{"freq",					'f',	"change cycle (frequency)"},
		{"cycle",					'c',	"change cycle (cycle)"},
		{"start",					't',	"start (end stand-by mode)"},
		{"stand-by",				'B',	"stand-by mode"},
		{"viewer",					'v',	"viewer on / off"},
		{"", '\0'}
};

static const double ShowCycle = gnd_sec2time(1.0);
static const double ClockCycle = gnd_sec2time(1.0) / 60.0 ;

int main(int argc, char* argv[]) {
	gnd::OPSM::optimizer_basic	*optimizer = 0;			// optimizer class
	void 					*starting = 0;		// optimization starting value

	gnd::OPSM::counting_map_t	cnt_map;			// observation probability counting map
	gnd::OPSM::map_t				op_map;				// observation probability map

	SSMScanPoint2D			ssm_sokuikiraw;		// sokuiki raw streaming data
	SSMApi<Spur_Odometry>	ssm_odm;			// odometry position streaming data
	SSMApi<Spur_Odometry>	ssm_pos;			// tracking result position streaming data

	yp_coordinate_manager	coordtree;			// coordinate tree
	int cid_gl = -1,							// global coordinate node id
			coordid_rbt = -1,						// robot coordinate node id
			coordid_sns = -1,						// sensor coordinate node id
			coordid_odm = -1;							// odometry coordinate node id

	gnd::cui gcui;								// cui class

	OPSMPosTrack::configure_parameters param;	// configuration parameter
	OPSMPosTrack::options proc_opt(&param);		// process option analyze class


	{ // ---> initialization
		int ret;								// function return value
		size_t phase = 1;						// initialize phase



		// ---> read process options
		if( (ret = proc_opt.get_option(argc, argv)) != 0 ) {
			return ret;
		} // <--- read process options



		{ // ---> coordinate-tree set robot coordinate
			yp_matrix_fixed<4,4> cc; // coordinate relation matrix

			// set global coordinate
			yp_matrix_set_unit(&cc);
			cid_gl = coordtree.create_node("global", "root", &cc);

			// set robot coordinate
			yp_matrix_set_unit(&cc);
			coordid_rbt = coordtree.create_node("robot", "global", &cc);

			// set odometry coordinate
			yp_matrix_set_unit(&cc);
			coordid_odm = coordtree.create_node("odometry", "root", &cc);	// local dead-reckoning
		} // <--- coordinate-tree set robot coordinate



		{ // ---> allocate SIGINT to shut-off
			::proc_shutoff_clear();
			::proc_shutoff_alloc_signal(SIGINT);
		} // <--- allocate SIGINT to shut-off



		{ // ---> show initialize task
			::fprintf(stderr, "==========Initialize==========\n");
			::fprintf(stderr, " %d. create optimizer class \"%s\"", phase++, param.optimizer.value);
			if( param.slam.value && *param.mapdir.value ) {
				::fprintf(stderr, " %d. Map Data Load\n", phase++);
				::fprintf(stderr, " %d. Build %sMap\n", phase++, param.ndt.value ? "NDT " : "");
			}
			::fprintf(stderr, " %d. Initailize SSM\n", phase++);
			::fprintf(stderr, " %d. Create ssm-data \"\x1b[4m%s\x1b[0m\"\n", phase++, param.ssmname.value);
			::fprintf(stderr, " %d. Open ssm-data \"\x1b[4m%s\x1b[0m\"\n", phase++, param.ls_ssmname.value);
			::fprintf(stderr, " %d. Open ssm-data \"\x1b[4m%s\x1b[0m\"\n", phase++, param.odm_ssmname.value);
			::fprintf(stderr, " %d. Initialize viewer\n", phase++);
			::fprintf(stderr, "\n\n");
		} // <--- show initialize task


		// ---> set optimizer
		if( !::is_proc_shutoff() ) {
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => create optimizer class \"%s\"\n", param.optimizer.value);
			if( !::strcmp(param.optimizer.value, OPSMPosTrack::OptNewton) ){
				optimizer = new gnd::OPSM::newton;
				optimizer->create_starting_value(&starting);
				optimizer->set_converge_threshold(param.converge_dist.value,
						gnd_deg2ang( param.converge_orient.value ) );
				::fprintf(stderr, " ...\x1b[1mOK\x1b[0m\n");
			}
			else if( !::strcmp(param.optimizer.value, OPSMPosTrack::OptQMC)){
				gnd::OPSM::quasi_monte_calro::starting_value *p;
				optimizer = new gnd::OPSM::quasi_monte_calro;
				optimizer->create_starting_value(&starting);
				p = static_cast<gnd::OPSM::quasi_monte_calro::starting_value*>(starting);
				p->n = 1;
				starting = static_cast<void*>(p);
				optimizer->set_converge_threshold(param.converge_dist.value,
						gnd_deg2ang( param.converge_orient.value ) );
				::fprintf(stderr, " ...\x1b[1mOK\x1b[0m\n");
			}
			else {
				::proc_shutoff();
				::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: invalid optimizer type\n");
			}

		} // ---> set optimizer



		// ---> map data load
		if( !::is_proc_shutoff() && param.slam.value && *param.mapdir.value ){
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => Map Data Load\n");
			if( gnd::OPSM::read_counting_map(&cnt_map, param.mapdir.value) < 0){
				::proc_shutoff();
				::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to map data\n");
			}
			else {
				::fprintf(stderr, " ...\x1b[1mOK\x1b[0m\n");
			}
		} // <--- map data load



		// ---> build map
		if( !::is_proc_shutoff() && param.slam.value && *param.mapdir.value ){
			::fprintf(stderr, "\n");
			if( !param.ndt.value){
				::fprintf(stderr, " => Build Map\n");
				if( gnd::OPSM::build_map(&op_map, &cnt_map, 10, 1, 0.5) < 0) {
					::proc_shutoff();
					::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to build scan matching map\n");
				}
				else {
					::fprintf(stderr, " ...\x1b[1mOK\x1b[0m\n");
				}
			}
			else {
				::fprintf(stderr, " => Build NDT Map\n");
				if(gnd::OPSM::build_ndt_map(&op_map, &cnt_map, gnd_mm2dist(1)) < 0){
					::proc_shutoff();
					::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to build scan matching map\n");
				}
				else {
					::fprintf(stderr, " ...\x1b[1mOK\x1b[0m\n");
				}
			}
		} // <--- build map

		// set map
		optimizer->set_map(&op_map);



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


		// ---> create ssm adjust
		if( !::is_proc_shutoff() ){
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => Create ssm-data \"\x1b[4m%s\x1b[0m\"\n", param.ssmname.value);
			if(!ssm_pos.create( param.ssmname.value, 0, 2.5, 0.005 ) ){
				::fprintf(stderr, "  [\x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m]: Fail to ssm open \"\x1b[4m%s\x1b[0m\"\n", param.ssmname.value);
			}
			else {
				::fprintf(stderr, "  [\x1b[1mOK\x1b[0m]: Open ssm-data \"\x1b[4m%s\x1b[0m\"\n", param.ssmname.value);
				ssm_pos.readLast();
				ssm_pos.data.x = 0;
				ssm_pos.data.y = 0;
				ssm_pos.data.theta = 0;

			}
		} // <--- create ssm adjust



		// ---> open ssm odometry
		if( !::is_proc_shutoff() ){
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => Create ssm-data \"\x1b[4m%s\x1b[0m\"\n", param.odm_ssmname.value);
			if(!ssm_odm.openWait( param.odm_ssmname.value, 0, 0.0) ){
				::fprintf(stderr, "  [\x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m]: Fail to ssm open \"\x1b[4m%s\x1b[0m\"\n", param.odm_ssmname.value);
			}
			else {
				::fprintf(stderr, "  [\x1b[1mOK\x1b[0m]: Open ssm-data \"\x1b[4m%s\x1b[0m\"\n", param.odm_ssmname.value);
				ssm_odm.readLast();

				{ // ---> set coordinate
					yp_matrix_fixed<4,4> pos_cc;

					// odometry coordinate
					yp_matrix_coordinate_converter(&pos_cc,
							ssm_odm.data.x, ssm_odm.data.y, 0,
							::cos(ssm_odm.data.theta), ::sin(ssm_odm.data.theta), 0,
							 0, 0, 1);
					coordtree.set_node(coordid_odm, &pos_cc);
				} // ---> set coordinate
			}
		} // <--- open ssm odometry



		// ---> open ssm sokuiki raw data
		if(!::is_proc_shutoff()){
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => Open ssm-data \"\x1b[4m%s\x1b[0m\"\n", param.ls_ssmname.value);

			if( !ssm_sokuikiraw.openWait( param.ls_ssmname.value, param.ls_ssmid.value, 0.0) ){
				::proc_shutoff();
				::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to open ssm-data \"\x1b[4m%s\x1b[0m\"\n", param.ls_ssmname.value);
			}
			// get property
			else if( !ssm_sokuikiraw.getProperty() ){
				::proc_shutoff();
				::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to get property of \"\x1b[4m%s\x1b[0m\"\n",  SSM_NAME_SCAN_POINT_2D);
			}
			else {
				ssm_sokuikiraw.setBlocking(true);

				// allocate
				ssm_sokuikiraw.data.alloc(ssm_sokuikiraw.property.numPoints);

				{ // ---> coordinate-tree set sensor coordinate
					coordid_sns = coordtree.create_node("sensor", "robot", &ssm_sokuikiraw.property.coordm);
					//					yp_matrix_fixed<4,4> cc; // coordinate relation matrix
					//					yp_matrix_fixed<4,4> cp, rot;
					//
					//					yp_matrix_set_unit(&cc);
					//
					//					// Rx rotation matrix
					//					yp_matrix_set_unit(&rot);
					//					yp_matrix_set(&rot, 0, 0, 1 );
					//					yp_matrix_set(&rot, 0, 1, 0 );
					//					yp_matrix_set(&rot, 0, 2, 0 );
					//					yp_matrix_set(&rot, 1, 0, 0 );
					//					yp_matrix_set(&rot, 1, 1, cos(ssm_sokuikiraw.property.pos.Rz) );
					//					yp_matrix_set(&rot, 1, 2,-sin(ssm_sokuikiraw.property.pos.Rz) );
					//					yp_matrix_set(&rot, 2, 2, 0 );
					//					yp_matrix_set(&rot, 2, 1, sin(ssm_sokuikiraw.property.pos.Rz) );
					//					yp_matrix_set(&rot, 2, 2, cos(ssm_sokuikiraw.property.pos.Rz) );
					//					yp_matrix_copy(&cp, &cc);
					//					yp_matrix_prod(&cp, &rot, &cc);
					//
					//					// Ry
					//					yp_matrix_set_unit(&rot);
					//					yp_matrix_set(&rot, 0, 0, cos(ssm_sokuikiraw.property.pos.Ry) );
					//					yp_matrix_set(&rot, 0, 1, 0 );
					//					yp_matrix_set(&rot, 0, 2, sin(ssm_sokuikiraw.property.pos.Ry) );
					//					yp_matrix_set(&rot, 1, 0, 0 );
					//					yp_matrix_set(&rot, 1, 1, 1 );
					//					yp_matrix_set(&rot, 1, 2, 0 );
					//					yp_matrix_set(&rot, 2, 0, -sin(ssm_sokuikiraw.property.pos.Ry) );
					//					yp_matrix_set(&rot, 2, 1, 0 );
					//					yp_matrix_set(&rot, 2, 2, cos(ssm_sokuikiraw.property.pos.Ry) );
					//					yp_matrix_copy(&cp, &cc);
					//					yp_matrix_prod(&cp, &rot, &cc);
					//
					//					// Rz
					//					yp_matrix_set_unit(&rot);
					//					yp_matrix_set(&rot, 0, 0, cos(ssm_sokuikiraw.property.pos.Rx) );
					//					yp_matrix_set(&rot, 0, 1,-sin(ssm_sokuikiraw.property.pos.Rx) );
					//					yp_matrix_set(&rot, 0, 2, 0 );
					//					yp_matrix_set(&rot, 1, 0, sin(ssm_sokuikiraw.property.pos.Rx) );
					//					yp_matrix_set(&rot, 1, 1, cos(ssm_sokuikiraw.property.pos.Rx) );
					//					yp_matrix_set(&rot, 1, 2, 0 );
					//					yp_matrix_set(&rot, 2, 0, 0 );
					//					yp_matrix_set(&rot, 2, 1, 0 );
					//					yp_matrix_set(&rot, 1, 2, 1 );
					//					yp_matrix_copy(&cp, &cc);
					//					yp_matrix_prod(&cp, &rot, &cc);
					//
					//					// set transition
					//					yp_matrix_set(&cc, 0, 3, ssm_sokuikiraw.property.pos.Tx);
					//					yp_matrix_set(&cc, 1, 3, ssm_sokuikiraw.property.pos.Ty);
					//					yp_matrix_set(&cc, 2, 3, ssm_sokuikiraw.property.pos.Tz);
					//
					//					coordid_sns = coordtree.create_node("sensor", "robot", &cc);
				} // <--- coordinate-tree set robot coordinate

				ssm_sokuikiraw.readLast();
				::fprintf(stderr, " ... \x1b[1mOK\x1b[0m\n");
			}
		} // <--- open ssm sokuiki raw data




		{ // ---> viewer initialization
			gnd::gl::initialize(&argc, argv);

			gnd::gl::window.m = GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE;
			gnd::gl::window.w = 400;
			gnd::gl::window.h = 300;

			gnd::gl::window.field = 10;
			::strcpy( gnd::gl::window.name, "viewer" );

			gnd::gl::window.df = OPSMPosTrack::Viewer::display;
			gnd::gl::window.idle = OPSMPosTrack::Viewer::idle;
			gnd::gl::window.mf = OPSMPosTrack::Viewer::mouse;
			gnd::gl::window.reshf = OPSMPosTrack::Viewer::reshape;

			gnd::gl::begin();
		} // <--- viewer initialization




		// set cui command
		gcui.set_command(cui_cmd, sizeof(cui_cmd) / sizeof(cui_cmd[0]));

		// fin of initialization
		::fprintf(stderr, "\n\n");
	} // <--- initialization












	// ---> operation
	if(!::is_proc_shutoff() ){
		int ret;										// function return value

		Spur_Odometry odo_prevloop = ssm_odm.data;		// previous odometry position
		Spur_Odometry move_est;							// estimation of movement quantity

		double decimate_sqdist							// data decimation threshold
				= gnd_square( param.decimate.value );
		double lkl = 0;									// likelihood
		double lkl_prev_opt = 0;						// previous likelihood
		Spur_Odometry pos_opt;							// optimized position
		int cnt = 0;									// optimization loop counter

		yp_matrix_fixed<3,1> move_opt;					// position estimation movement by optimization

		Spur_Odometry pos_premap = ssm_pos.data;		// odometry position streaming data
		ssmTimeT time_premap = 							// previous map update time
				0;
		double sqdist_mapup =							// distance threshold of map update
				gnd_square( param.rest_dist.value );

		yp_matrix_fixed<4,4> coordm_sns2rbt;			// coordinate convert matrix from sensor to robot
		yp_matrix_fixed<4,4> coordm_sns2gl;				// coordinate convert matrix from sensor to global

		bool show_st = true;							// flag of show status mode
		double cuito = 0;								// blocking time out for cui input

		gnd::Time::IntervalTimer timer_clock;			// clock
		gnd::Time::IntervalTimer timer_operate;			// time operation timer
		gnd::Time::IntervalTimer timer_show;			// time operation timer

		bool map_update;

		// get coordinate convert matrix
		coordtree.get_convert_matrix(coordid_sns, coordid_rbt, &coordm_sns2rbt);

		{ // ---> set zero
			move_est.x = 0;
			move_est.y = 0;
			move_est.theta = 0;
			move_est.v = 0;
			move_est.w = 0;
		} // <--- set zero

		// ---> memory allocate counting map
		if( !cnt_map.plane[0].is_allocate() ){
			gnd::OPSM::init_counting_map(&cnt_map, 0.5, 10);
		} // <--- memory allocate counting map


		{ // ---> timer
			// set parameter-cycle
			timer_operate.begin(CLOCK_REALTIME, param.cycle.value);
			timer_show.begin(CLOCK_REALTIME, ShowCycle);
			timer_clock.begin(CLOCK_REALTIME,
					param.cycle.value < ClockCycle ? param.cycle.value :  ClockCycle);
		} // <--- timer

		// ---> operation loop
		while (!::is_proc_shutoff()) {
			timer_clock.wait();

			{ // ---> cui
				int cuival = 0;
				char cuiarg[512];
				// zero reset buffer
				::memset(cuiarg, 0, sizeof(cuiarg));

				// ---> get command
				if( gcui.poll(&cuival, cuiarg, sizeof(cuiarg), cuito) > 0 ){
					if( show_st ){
						// quit show status mode
						show_st = false;
						::fprintf(stderr, "-------------------- cui mode --------------------\n");
					}
					else { // ---> cui command operation
						switch(cuival){
						// exit
						case 'E': ::proc_shutoff(); break;
						// help
						default:
						case '\0':
						case 'h': gcui.show(stderr, "   "); break;
						// show status
						case 's': show_st = true; break;
						case 'f': {
							double freq = ::strtod(cuiarg, 0);
							if( freq <= 0 ){
								::fprintf(stderr, "   ... \x1b[31m\x1b[1mError\x1b[0m\x1b[39m: invalid argument value (frequency 0)\n");
								::fprintf(stderr, "       if you want to stop estimator, send \"\x1b[4mstand-by\x1b[0m\" command\n");
							}
							else {
								double cyc = 1.0 / freq;
								timer_operate.begin(CLOCK_REALTIME, cyc);
								timer_clock.begin(CLOCK_REALTIME,
										param.cycle.value < ClockCycle ? param.cycle.value :  ClockCycle);
								::fprintf(stderr, "   ... cycle %.03lf\n", cyc);
							}
						} break;

						// set freq
						case 'c': {
							double cyc = ::strtod(cuiarg, 0);
							if( cyc <= 0 ){
								::fprintf(stderr, "   ... \x1b[31m\x1b[1mError\x1b[0m\x1b[39m: invalid argument value (frequency 0)\n");
								::fprintf(stderr, "       if you want to stop estimator, send \"\x1b[4mstand-by\x1b[0m\" command\n");
							}
							else {
								timer_operate.begin(CLOCK_REALTIME, cyc);
								timer_clock.begin(CLOCK_REALTIME,
										param.cycle.value < ClockCycle ? param.cycle.value :  ClockCycle);
								::fprintf(stderr, "   ... cycle %.03lf\n", cyc);
							}
						} break;

						// start
						case 't':{
							cuito = 0.0;
						} break;
						// stand-by
						case 'B':{
							::fprintf(stderr, "   stand-by mode\n");
							cuito = -1;
						} break;

						case 'v': {
							if( gnd::gl::is_free() ){
								gnd::gl::begin();
								fprintf(stderr, "   ... create\n");
							}
							else {
								gnd::gl::end();
								fprintf(stderr, "   ... delete\n");
							}
						} break;
						}
					} // <--- cui command operation
					::fprintf(stderr, "  > ");
					gcui.poll(&cuival, cuiarg, sizeof( cuiarg ), 0);
				} // <--- get command
			}  // <--- cui



			// ---> show status
			if( show_st ){
				// ---> update
				if( timer_show.clock() > 0 ){
					::fprintf(stderr, "\x1b[0;0H\x1b[2J");	// display clear
					::fprintf(stderr, "-------------------- \x1b[1m\x1b[36m%s\x1b[39m\x1b[0m --------------------\n", OPSMPosTrack::proc_name);
					::fprintf(stderr, "       loop : %d\n", cnt);
					::fprintf(stderr, " likelihood : %.03lf\n", lkl );
					::fprintf(stderr, "        pos : %4.03lf[m], %4.03lf[m], %4.02lf[deg]\n",
							ssm_pos.data.x, ssm_pos.data.y, gnd_ang2deg( ssm_pos.data.theta ) );
					::fprintf(stderr, "   optimize : %4.03lf[m], %4.03lf[m], %4.02lf[deg]\n",
							move_opt[0][0], move_opt[1][0], gnd_ang2deg( move_opt[2][0] ) );
					::fprintf(stderr, "   move est : %4.03lf[m], %4.03lf[m], %4.02lf[deg]\n",
							move_est.x, move_est.y, gnd_ang2deg( move_est.theta ) );
					::fprintf(stderr, "      cycle : %.03lf\n", timer_operate.cycle() );
					::fprintf(stderr, "  optimizer : %s\n", ret == 0 ? "success" : "failure" );
					::fprintf(stderr, " map update : %s\n", map_update ? "true" : "false" );
					::fprintf(stderr, "\n");
					::fprintf(stderr, " Push \x1b[1mEnter\x1b[0m to change CUI Mode\n");
				} // <--- update
			} // <--- show status

			// ---> read ssm-sokuikiraw-data
			if(timer_operate.clock() > 0 && ssm_sokuikiraw.readNew()) {
				// ---> position tracking
				// ... operation flow
				//      *0. get laser scanner reading
				//       1. compute position estimation by odometry (on global coordinate)
				//		 2. set position estimation by odometry to optimization starting value
				//		 3. optimization iteration by matching laser scanner reading to map(likelihood field)
				//		 4. optimization error test and write position ssm-data
				//		 5. robot current position coordinate and odometory position coordinate
				//		 6. update map and view data


				{  // ---> 1. compute position estimation from odometry
					// get position on odometry position (on odometry cooordinate)
					if( !ssm_odm.readTime(ssm_sokuikiraw.time) ) continue;

					{ // ---> compute the movement estimation
						yp_matrix_fixed<4,1> odov_cprev;			// current odometry position vector on previous odometry coordinate

						{ // ---> compute current odometry position on previous odometry coordinate
							yp_matrix_fixed<4,4> coordm_r2podo;		// coordinate matrix of previous odometry position
							yp_matrix_fixed<4,1> ws4x1;

							// get previous odometry coordinate matrix
							coordtree.get_convert_matrix(0, coordid_odm, &coordm_r2podo);

							// multiply previous odometry coordinate matrix with current position vector
							ws4x1[0][0] = ssm_odm.data.x;
							ws4x1[1][0] = ssm_odm.data.y;
							ws4x1[2][0] = 0;
							ws4x1[3][0] = 1;
							yp_matrix_prod(&coordm_r2podo, &ws4x1, &odov_cprev);
						} // <--- compute current odometry position on previous odometry coordinate

						// get movement estimation by odometry
						move_est.x = odov_cprev[0][0];
						move_est.y = odov_cprev[1][0];
						move_est.theta = ssm_odm.data.theta - odo_prevloop.theta;
					} // <--- compute the movement estimation


					{ // ---> add movement estimation
						yp_matrix_fixed<4,1> pos_odmest;

						{ // ---> compute position estimation by odometry on global coordinate
							yp_matrix_fixed<4,4> coordm_rbt2gl;		// coordinate convert matrix from robot to global
							yp_matrix_fixed<4,1> ws4x1;

							// set search position on sensor-coordinate
							coordtree.get_convert_matrix(coordid_rbt, cid_gl, &coordm_rbt2gl);

							ws4x1[0][0] = move_est.x;
							ws4x1[1][0] = move_est.y;
							ws4x1[2][0] = 0;
							ws4x1[3][0] = 1;

							yp_matrix_prod(&coordm_rbt2gl, &ws4x1, &pos_odmest);
						} // <--- compute position estimation by odometry on global coordinate

						// set position
						ssm_pos.data.x = pos_odmest[0][0];
						ssm_pos.data.y = pos_odmest[1][0];
						ssm_pos.data.theta += move_est.theta;

						optimizer->set_starting_value( starting, ssm_pos.data.x, ssm_pos.data.y, ssm_pos.data.theta );
					} // <--- add movement estimation
				}  // <--- 1. compute position estimation from odometry



				// ---> 2. set position estimation by odometry to optimization starting value
				optimizer->begin(starting);


				yp_matrix_set_zero(&move_opt);
				{ // ---> 3. optimization iteration by matching laser scanner reading to map(likelihood field)
					double left_timer = 1.0;
					yp_matrix_fixed<3,1> delta;
					yp_matrix_fixed<2,1> reflect_prevent;

					// clear previous entered sensor reading
					yp_matrix_set_zero(&reflect_prevent);

					// ---> scanning loop for sokuikiraw-data
					for(size_t i = 0; i < ssm_sokuikiraw.data.numPoints(); i++){
						// ---> entry laser scanner reflection
						yp_matrix_fixed<4,1> reflect_csns, reflect_crbt;
						yp_matrix_fixed<3,1> ws3x1;
						yp_matrix_fixed<3,3> ws3x3;

						// if range data is null because of no reflection
						if(ssm_sokuikiraw.data[i].status == ssm::laser::STATUS_NO_REFLECTION)	continue;
						// ignore error data
						else if(ssm_sokuikiraw.data[i].isError()) 	continue;
						else if(ssm_sokuikiraw.data[i].r < ssm_sokuikiraw.property.distMin * 1.1)	continue;
						else if(ssm_sokuikiraw.data[i].r > ssm_sokuikiraw.property.distMax * 0.9)	continue;

						{ // ---> compute laser scanner reading position on robot coordinate
							// set search position on sensor-coordinate
							reflect_csns[0][0] = ssm_sokuikiraw.data[i].r * ::cos(ssm_sokuikiraw.data[i].th);
							reflect_csns[1][0] = ssm_sokuikiraw.data[i].r * ::sin(ssm_sokuikiraw.data[i].th);
							reflect_csns[2][0] = 0;
							reflect_csns[3][0] = 1;


							// data decimation with distance threshold
							if( gnd_square(reflect_csns[0][0] - reflect_prevent[0][0]) + gnd_square(reflect_csns[1][0] - reflect_prevent[1][0]) < decimate_sqdist ){
								continue;
							}
							else {
								// update previous entered data
								yp_matrix_copy(&reflect_prevent, &reflect_csns);
							}

							// convert from sensor coordinate to robot coordinate
							yp_matrix_prod(&coordm_sns2rbt, &reflect_csns, &reflect_crbt);
						} // <--- compute laser scanner reading position on robot coordinate

						// data entry
						optimizer->entry( reflect_crbt[0][0] , reflect_crbt[1][0] );
						// <--- entry laser scanner reflection
					} // <--- scanning loop for sokuikiraw-data

					// zero reset likelihood
					lkl = 0;
					// zero reset optimization iteration counter
					cnt = 0;
					yp_matrix_set_zero(&move_opt);
					do{
						// store previous optimization position likelihood
						lkl_prev_opt = lkl;

						{ // ---> step iteration of optimization
							yp_matrix_fixed<3,1> ws3x1;

							if( (ret = optimizer->iterate(&delta, &ws3x1, 0, &lkl)) < 0 ){
								break;
							}

							// get optimized position
							pos_opt.x = ws3x1[0][0];
							pos_opt.y = ws3x1[1][0];
							pos_opt.theta = ws3x1[2][0];
							// get movement by optimization
							yp_matrix_add(&move_opt, &delta, &move_opt);
						} // <--- step iteration of optimization

						// loop counting
						cnt++;
						// convergence test
						timer_operate.clock(&left_timer);
					} while( !optimizer->converge_test() && left_timer > gnd_msec2time(30)); // <--- position optimization loop
				} // ---> 3. optimization iteration by matching laser scanner reading to map(likelihood field)



				// ---> 4. optimization error test and write position ssm-data
				// check --- 1st. function error, 2nd. distance, 3rd. orient difference
				if( ret >= 0 &&
						gnd_square( ssm_pos.data.x - pos_opt.x ) + gnd_square( ssm_pos.data.y - pos_opt.y ) < gnd_square( 1 ) &&
						::fabs( ssm_pos.data.theta - pos_opt.theta ) < gnd_deg2ang(10) ) {
					ssm_pos.data.x = pos_opt.x;
					ssm_pos.data.y = pos_opt.y;
					ssm_pos.data.theta = pos_opt.theta;

					ssm_pos.write(ssm_sokuikiraw.time);
				}
				else {
					ssm_pos.write(ssm_sokuikiraw.time);
				} // <--- 4. optimization error test and write position ssm-data


				{ // ---> 5. update robot position coordinate and odometory position coordinate
					yp_matrix_fixed<4,4> coordm;

					// odometry coordinate
					yp_matrix_coordinate_converter(&coordm,
							ssm_odm.data.x, ssm_odm.data.y, 0,
							::cos(ssm_odm.data.theta), ::sin(ssm_odm.data.theta), 0,
							 0, 0, 1);
					coordtree.set_node(coordid_odm, &coordm);

					// robot position coordinate
					yp_matrix_coordinate_converter(&coordm,
							ssm_pos.data.x, ssm_pos.data.y, 0,
							::cos(ssm_pos.data.theta), ::sin(ssm_pos.data.theta), 0,
							 0, 0, 1);
					coordtree.set_node(coordid_rbt, &coordm);

					// get coordinate convert matrix
					coordtree.get_convert_matrix(coordid_sns, cid_gl, &coordm_sns2gl);
				} // ---> 5. update robot position coordinate and odometory position coordinate



				{ // ---> 6. update map and view data
					// map update check, 1st. time, 2nd. position, 3rd. orient
					map_update = ssm_sokuikiraw.time - time_premap > param.rest_cycle.value ||
							gnd_square( ssm_pos.data.x - pos_premap.x) + gnd_square( ssm_pos.data.y - pos_premap.y) > sqdist_mapup ||
							::fabs( ssm_pos.data.theta - pos_premap.theta ) > param.rest_orient.value;

					if( map_update && !param.slam.value ){ // ---> clear
						gnd::OPSM::clear_counting_map(&cnt_map);
					} // <--- clear

					{// ---> scanning loop for sokuikiraw-data
						yp_matrix_fixed<4,1> reflect_csns;
						yp_matrix_fixed<4,1> reflect_cgl;
						yp_matrix_fixed<2,1> reflect_prevent;

						OPSMPosTrack::Viewer::scan_cur.wait();
						{ // set view point data
							OPSMPosTrack::Viewer::scan_2prev.wait();
							OPSMPosTrack::Viewer::scan_prev.wait();
							// shift prev data
							OPSMPosTrack::Viewer::scan_2prev.var.clear();
							if( OPSMPosTrack::Viewer::scan_prev.var.begin() )
								OPSMPosTrack::Viewer::scan_2prev.var.copy(OPSMPosTrack::Viewer::scan_prev.var.begin(), OPSMPosTrack::Viewer::scan_prev.var.size() );
							OPSMPosTrack::Viewer::scan_prev.var.clear();
							if( OPSMPosTrack::Viewer::scan_cur.var.begin() )
								OPSMPosTrack::Viewer::scan_prev.var.copy(OPSMPosTrack::Viewer::scan_cur.var.begin(), OPSMPosTrack::Viewer::scan_cur.var.size() );
							OPSMPosTrack::Viewer::scan_prev.post();
							OPSMPosTrack::Viewer::scan_2prev.post();
						}
						OPSMPosTrack::Viewer::scan_cur.var.clear();
						// ---> scanning loop of laser scanner reading
						for(size_t i = 0; i < ssm_sokuikiraw.data.numPoints(); i++){
							yp_matrix_fixed<3,1> ws3x1;
							yp_matrix_fixed<3,3> ws3x3;
							gnd::gl::point p;

							// if range data is null because of no reflection
							if(ssm_sokuikiraw.data[i].status == ssm::laser::STATUS_NO_REFLECTION)	continue;
							// ignore error data
							else if(ssm_sokuikiraw.data[i].isError()) 	continue;
							else if(ssm_sokuikiraw.data[i].r < ssm_sokuikiraw.property.distMin)	continue;
							else if(ssm_sokuikiraw.data[i].r > ssm_sokuikiraw.property.distMax)	continue;

							{ // ---> compute laser scanner reading position on global coordinate
								// set search position on sensor-coordinate
								yp_matrix_set(&reflect_csns, 0, 0, ssm_sokuikiraw.data[i].r * ::cos(ssm_sokuikiraw.data[i].th));
								yp_matrix_set(&reflect_csns, 1, 0, ssm_sokuikiraw.data[i].r * ::sin(ssm_sokuikiraw.data[i].th));
								yp_matrix_set(&reflect_csns, 2, 0, 0);
								yp_matrix_set(&reflect_csns, 3, 0, 1);

								// data decimation with distance threshold
								if( gnd_square(reflect_csns[0][0] - reflect_prevent[0][0]) + gnd_square(reflect_csns[1][0] - reflect_prevent[1][0]) < decimate_sqdist ){
									continue;
								}
								else {
									// update previous entered data
									yp_matrix_copy(&reflect_prevent, &reflect_csns);
								}

								// convert from sensor coordinate to global coordinate
								yp_matrix_prod(&coordm_sns2gl, &reflect_csns, &reflect_cgl);
							} // <--- compute laser scanner reading position on global coordinate

							// ---> enter laser scanner reading to map
							if( map_update) {
								if( param.slam.value ){
									gnd::OPSM::update_map(&cnt_map, &op_map, reflect_cgl[0][0], reflect_cgl[1][0], gnd_m2dist(10), gnd_mm2dist(1), 0.5);
								}
								else {
									gnd::OPSM::counting_map(&cnt_map, reflect_cgl[0][0], reflect_cgl[1][0]);
								}
								// update
								time_premap = ssm_sokuikiraw.time;
								pos_premap = ssm_pos.data;
							} // <--- enter laser scanner reading to map

							// enter viewer data
							p.x = reflect_cgl[0][0];
							p.y = reflect_cgl[1][0];
							p.z = 0.0;
							OPSMPosTrack::Viewer::scan_cur.var.push_back(&p);
						} // <--- scanning loop of laser scanner reading
						OPSMPosTrack::Viewer::scan_cur.post();

					} // <--- scanning loop for sokuikiraw-data


					// ---> update map
					if( map_update) {
						if( param.slam.value ){
						}
						else {
							if( param.ndt.value ){
								if( gnd::OPSM::build_map(&op_map, &cnt_map, 10, 1.0e-3, 0) < 0 ){
									::fprintf(stderr, "\x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: invalid map property\n");
								}
							}
							else{
								if( gnd::OPSM::build_ndt_map(&op_map, &cnt_map, 1.0e-3) < 0 ){
									::fprintf(stderr, "\x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: invalid map property\n");
								}
							}
						}
					} // <--- update map
				} // ---> 6. update map and view data
				odo_prevloop = ssm_odm.data;
			} // <--- read ssm sokuikiraw

		} // <--- operation loop

	} // <--- operation



	{ // ---> finalization
		::endSSM();

		::fprintf(stderr, "\n");
		::fprintf(stderr, "Finish\n");

		optimizer->delete_starting_value(&starting);
		delete optimizer;

		// slam
		if( param.slam.value ) {
			gnd::bmp_gray_t bmp;

			gnd::OPSM::build_map(&op_map, &cnt_map, gnd_m2dist(10), gnd_mm2dist(1), 5);

			// ---> write intermediate file
			{
				::fprintf(stderr, " => write intermediate file\n");

				if( gnd::OPSM::write_counting_map(&cnt_map, "map") ) {
					::fprintf(stderr, "  ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open\n");
				}
				else {
					::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m: save counting map data\n");
				}

			} // <--- write intermediate file

			// bmp file building
			::fprintf(stderr, " => bmp map building\n");
			gnd::OPSM::buid_bmp(&bmp, &op_map, gnd_m2dist( 1.0 / 8) );

			{ // ---> file out
				{ // ---> bmp
					char fname[128];
					::fprintf(stderr, " => start map file out \n");

					::sprintf(fname, "%s.%s", "out", "bmp" );

					if( gnd::bmp::write_gray(fname, &bmp) < 0) {
						::fprintf(stderr, "  ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open \"\x1b[4m%s\x1b[0m\"\n", fname);
					}
					else {
						::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m: save map data into \"\x1b[4m%s\x1b[0m\"\n", fname);
					}
				} // <--- bmp

				{ // ---> origin
					char fname[128];
					FILE *fp;
					double x, y;

					::sprintf(fname, "%s.%s", "out", "origin.txt" );

					fp = fopen(fname, "w");

					bmp.pget_origin(&x, &y);
					fprintf(fp, "%lf %lf\n", x, y);
					fclose(fp);
				} // --->  origin
			} // <--- fileout

			::fprintf(stderr, "     end \n");
		}

	} // <--- finalization

	return 0;
}





