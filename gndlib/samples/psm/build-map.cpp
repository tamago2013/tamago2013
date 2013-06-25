/*
 * sample.cpp
 *
 *  Created on: 2013/06/02
 *      Author: tyamada
 */

#include <stdio.h>
#include "gnd-psm.hpp"


int main(int argc, char* argv[]) {
	FILE						*fp;		// data file of laser scanner
	gnd::psm::cmap_t		cnt_map;	// laser scanner data collection map (it use for building environment map)


	gnd::psm::debug_set_log_level(2);
	gnd::gridmap::debug_set_log_level(2);

	{ // ---> initialize
		if( argc < 2 ) {
			::fprintf(stderr, "error: missing data file operand\n");
			::fprintf(stderr, "samples$ ./%s laser.dat\n", argv[0]);
			return 1;
		}
		else if( !(fp = ::fopen(argv[1], "r")) ){
			::fprintf(stderr, "fail to open %s\n", argv[1]);
			return 1;
		}

		gnd::psm::init_counting_map(&cnt_map, 0.5, 10 );
	} // <--- initialize


	{ // ---> operation

		{ // ---> build map
			double x, y;
			int ret;

			// ---> scanning loop (data fiile)
			while( !::feof(fp)  ) {
				// read data file
				ret = ::fscanf(fp, "%lf %lf\n", &x, &y);
				// when read all data, break
				if( ret < 0 ) break;

				// counting
				gnd::psm::counting_map(&cnt_map, x, y);
			} // ---> scanning loop (data fiile)


		} // <--- build map


	} // <--- operation



	{ // ---> finalize
		gnd::bmp8_t bmp;
		gnd::psm::map_t			psm_map;	// environment map for scan matching

		// save counting map
		// output in current directory
		gnd::psm::write_counting_map(&cnt_map, "./");
		::fprintf(stdout, "make counting map data observ-prob.**.cmap\n");


		{ // ---> build bmp image (to visualize for human)
			// build environmental map
			gnd::psm::build_map(&psm_map, &cnt_map);
			// make bmp image: it show the likelihood field
			gnd::psm::build_bmp(&bmp, &psm_map, 1.0 / 8);
			// file out
			gnd::bmp::write8("map-image.bmp", &bmp);

			::fprintf(stdout, "make map image %s\n", "map-image.bmp");
			bmp.deallocate();
		} // <--- build bmp image (to visualize for human)

	} // <--- finalize

	return 0;
}




