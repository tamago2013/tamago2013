/*
 * observation-probability-position-tracker-view.hpp
 *
 *  Created on: 2012/01/10
 *      Author: tyamada
 */

#ifndef OBSERVATION_PROBABILITY_POSITION_TRACKER_VIEW_HPP_
#define OBSERVATION_PROBABILITY_POSITION_TRACKER_VIEW_HPP_

#include "gnd-gl.hpp"
#include "gnd-gldraw.hpp"

#ifndef OPSMPosTrack
#define OPSMPosTrack ObservationProbabilityScanMatching::PositionTracker
#endif

namespace ObservationProbabilityScanMatching {
	namespace PositionTracker {
		namespace Viewer {
			gnd_semvariable< gnd_queue< gnd_gl::point > > scan_2prev;	///< previous point
			gnd_semvariable< gnd_queue< gnd_gl::point > > scan_prev;	///< previous point
			gnd_semvariable< gnd_queue< gnd_gl::point > > scan_cur;		///< current point

			inline
			void display() {
				::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);

				gnd_gl::project();

				scan_2prev.wait();
				gnd_gl::draw_points( scan_2prev.var.begin(), scan_2prev.var.size(), 2, 1.0, 1.0, 1.0);
				scan_2prev.post();

				scan_prev.wait();
				gnd_gl::draw_points( scan_prev.var.begin(), scan_prev.var.size(), 2, 1.0,  0, 0);
				scan_prev.post();

				scan_cur.wait();
				gnd_gl::draw_points( scan_cur.var.begin(), scan_cur.var.size(), 2, 0, 1.0, 0);
				scan_cur.post();

				::glFlush();
				::glutSwapBuffers();
			}

			inline
			void idle() {
				display();
			}

			inline
			void mouse(int b, int s, int x, int y) {
				double f,tmp;
				switch(b){
				case 3:
					gnd_gl::get_field(&f);
					tmp = f / 1.1;
					f = tmp < 1.0  ? 1.0 : tmp;
					gnd_gl::set_field(f);
					break;
				case 4:
					gnd_gl::get_field(&f);
					gnd_gl::set_field(f*1.1);
					break;
				}
			}


			inline
			void reshape(int w, int h) {
				//	::view_display();
			}
		}
	}
}

#endif /* OBSERVATION_PROBABILITY_POSITION_TRACKER_VIEW_HPP_ */
