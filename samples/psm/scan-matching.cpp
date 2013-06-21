/*
 * scan-matching.cpp
 *
 *  Created on: 2013/06/08
 *      Author: tyamada
 */


#include <stdio.h>
#include "gnd-psm.hpp"
#include "gnd-config-file.hpp"

#ifdef __linux__
#include "gnd-time.hpp"

#define TIMER_INIT() 			gnd::stopwatch _sw_
#define TIMER_BEGIN() 			_sw_.begin( CLOCK_REALTIME )
#define TIMER_REC(t,l)										\
		do {													\
			_sw_.get( 0, (l) );									\
			*t += *l;											\
		} while(0)

#define TIMER_SHOW(t, l)									\
		do {													\
			::fprintf(stdout, "   lap time = %lf\n", *(l));		\
			::fprintf(stdout, " total time = %lf\n", *(t));		\
		} while(0)

#else

#define TIMER_INIT()
#define TIMER_BEGIN()
#define TIMER_REC(t, l)
#define TIMER_SHOW(t, l)

#endif




int main( int argc, char* argv[] ) {
	gnd::psm::cmap_t		cmap;					// counting map
	gnd::psm::map_t			map;					// scan matching map
	gnd::matrix::fixed<3,1> pos;					// matching result
	FILE					*fp;					// matched scan data (input)

	gnd::conf::parameter_array<char, 256> cmap_dir = {
			"cmap-dir", 							// item name
			"./", 									// default value
			"cmap directory path" 					// comment
	};
	gnd::conf::parameter_array<double, 3> pos_ini = {
			"init-pos", 							// item name
			{0, 0, 0}, 								// default value
			"initial position for scan matching (x[m], y[m], theta[deg])" 	// comment
	};
	gnd::conf::parameter_array<char, 256> scan_file = {
			"scan-data",							// item name
			"scan.txt",						// default value
			"scan data file path"					// comment
	};

	{ // ---> debug mode
		gnd::psm::debug_set_log_level(2);
		gnd::gridmap::debug_set_log_level(2);
	} // <--- debug mode

	{ // ---> initialize
		int ret;
		gnd::conf::file_stream fs;

		// set parameter item and default parameter
		if( argc < 2 ) {
			static const char fname[] = "_scan-matching.conf";
			::fprintf(stderr, " error: missing file operand\n");
			::fprintf(stderr, "        please input configuration file path\n");
			::fprintf(stderr, "        e.g.: %s scan-matching.conf\n", argv[0]);
			::fprintf(stderr, "        ... \n");
			// write configuration file
			gnd::conf::set_parameter(&fs, &cmap_dir);
			gnd::conf::set_parameter(&fs, &pos_ini);
			gnd::conf::set_parameter(&fs, &scan_file);
			fs.write(fname);
			::fprintf(stderr, "         => generate configuration file template \"%s\"\n", fname);

			return -1;
		}

		// configuration file read
		ret = fs.read(argv[1]);
		if( ret < 0 ) {
			::fprintf(stderr, " error: fail to read configuration file\n");
			return -1;
		}

		// get parameter value
		gnd::conf::get_parameter(&fs, &cmap_dir);
		gnd::conf::get_parameter(&fs, &pos_ini);
		gnd::conf::get_parameter(&fs, &scan_file);

		if( !cmap_dir.value[0] ) {
			::fprintf(stdout, " error: missing counting map\n");
			::fprintf(stdout, " please set counting map directory\n");
			return -1;
		}

		// read counting map
		ret = gnd::psm::read_counting_map( &cmap, cmap_dir.value );
		if( ret < 0 ) {
			::fprintf(stderr, " error: fail to read counting map form directory \"%s\"\n", cmap_dir.value);
			return 0;
		}

		// build scan matching map
		ret = gnd::psm::build_map( &map, &cmap );

		if( !scan_file.value[0] ) {
			::fprintf(stdout, " error: missing scan data\n");
			::fprintf(stdout, "        please set scan data file\n");
			return -1;
		}

		// open scan data file
		fp = ::fopen( scan_file.value, "r" );
		if( !fp ) {
			::fprintf(stdout, " error: fail to open \"%s\"\n", scan_file.value);
			return -1;
		}

	} // <--- initialize



	{ // ---> operation
		int ret;
		gnd::psm::mcl						mcl;		// monte calro method class
		gnd::psm::mcl::initial_parameter	mcl_ini;

		gnd::matrix::fixed<3,1> 			delta;
		gnd::matrix::fixed<3,3> 			cov;
		double 								likelihood;
		int 								cnt = 0;

		// timer init (for linux)
		TIMER_INIT();

		// set map
		mcl.set_map(&map);

		// set random seed
		gnd::random_set_seed();

		// initial position
		mcl_ini.x = pos_ini.value[0];
		mcl_ini.y = pos_ini.value[1];
		mcl_ini.theta = pos_ini.value[2] * 3.141592 / 180;

		// number of particle
		mcl_ini.n = 500;

		// initial particles distribution
		// co-variance matrix
		gnd::matrix::set_zero( &mcl_ini.var_ini );
		mcl_ini.var_ini[0][0] = 0.03 * 0.03;									// x's standard deviation is 0.1
		mcl_ini.var_ini[1][1] = 0.03 * 0.03;									// y's standard deviation is 0.1
		mcl_ini.var_ini[2][2] = (3 * 3.141592 / 180) * (3 * 3.141592 / 180);	// orientation's standard deviation is 5 [deg]

		// set initial parameter
		mcl.begin( &mcl_ini );


		{// ---> set scan data (robot coordinate) and file out scan data on initial position
			FILE					*fp_ini;				// scan data on initial position (file out)
			double cosv = cos(mcl_ini.theta);
			double sinv = sin(mcl_ini.theta);

			// open output file (scan on initial position)
			fp_ini = ::fopen("scan-on-init-pos.txt", "w");
			if( !fp_ini ) {
				::fprintf(stdout, " error: fail to open \"%s\"\n", "scan-on-init-pos.txt");
				return -1;
			}

			while( !::feof(fp)  ) {
				double x, y;
				// read data file
				ret = ::fscanf(fp, "%lf %lf\n", &x, &y);
				// when read all data, break
				if( ret < 0 ) break;

				// set scan data
				mcl.set_scan_point( x, y );

				// file out scan data on initial position
				::fprintf(fp_ini, "%lf %lf\n",
						x * cosv - y * sinv + mcl_ini.x,
						x * sinv + y * cosv + mcl_ini.y);
			}

			::fclose(fp_ini);
		} // ---> set scan data (robot coordinate) and file out scan data on initial position


		// set convergence test parameter
		// difference from previous position 0.001m in distance and 0.1deg in orientation
		mcl.set_converge_threshold( 0.001, 0.01 * 3.141592 / 180 );

		// show initial position
		::fprintf(stdout, "       init pos = %lf, %lf, %lf\n", mcl_ini.x, mcl_ini.y, mcl_ini.theta * 180 / 3.141592);
		::fprintf(stdout, " scan point num = %d\n", mcl.nscan_point());

		::fprintf(stdout, "\n");
		::fprintf(stdout, " => Scan Matching Begin\n");
		// iterative optimization
		double t = 0, l;
		do {
			// optimize
			TIMER_BEGIN();
			mcl.iterate( &delta, &pos, &cov, &likelihood);
			TIMER_REC(&t, &l);
			cnt++;

			// show result
			::fprintf(stdout, "-------- optimization loop %d --------\n", cnt);
			::fprintf(stdout, "       delta = %lf, %lf, %lf\n", delta[0][0], delta[1][0], delta[2][0] * 180 / 3.141592);
			::fprintf(stdout, "         pos = %lf, %lf, %lf\n", pos[0][0], pos[1][0], pos[2][0] * 180 / 3.141592);
			::fprintf(stdout, "  likelihood = %lf\n", likelihood);
			::fprintf(stdout, " co-variance = %.4lf, %.4lf, %.4lf\n", cov[0][0], cov[0][1], cov[0][2]);
			::fprintf(stdout, "               %.4lf, %.4lf, %.4lf\n", cov[1][0], cov[1][1], cov[1][2]);
			::fprintf(stdout, "               %.4lf, %.4lf, %.4lf\n", cov[2][0], cov[2][1], cov[2][2]);
			TIMER_SHOW(&t, &l);
			::fprintf(stdout, "\n");
			::fprintf(stdout, "\n");

			// convergence test
		} while( !mcl.converge_test() || cnt < 5);

		::fprintf(stdout, " ... Finish\n");

	} // <--- operation


	{ // ---> finalize

		{ // ---> output matched scan data (global coordinate)
			int ret;
			double cosv = cos(pos[2][0]);
			double sinv = sin(pos[2][0]);
			FILE					*fp_m;					// scan data on matching position (file out)

			::fprintf(stdout, " => file-out matched scan points on global coordinate\n");


			// open output file (scan on matching position)
			fp_m = ::fopen("scan-on-match-pos.txt", "w");
			if( !fp_m ) {
				::fprintf(stdout, " error: fail to open \"%s\"\n", "scan-on-match-pos.txt");
				return -1;
			}

			// ---> fileout
			::fseek(fp, 0, SEEK_SET);
			while( !::feof(fp)  ) {
				double x, y;
				// read data file
				ret = ::fscanf(fp, "%lf %lf\n", &x, &y);
				// when read all data, break
				if( ret < 0 ) break;

				::fprintf(fp_m, "%lf %lf\n",
						x * cosv - y * sinv + pos[0][0],
						x * sinv + y * cosv + pos[1][0]
				);

			} // ---> fileout

			::fprintf(stdout, " ... output \"matched-scan-data.txt\"\n");

			// file close
			::fclose(fp_m);
		} // <--- output matched scan data (global coordinate)

		// file close
		::fclose(fp);

		gnd::psm::destroy_counting_map(&cmap);
		gnd::psm::destroy_map(&map);
	} // <--- finalize


	return 0;
}
