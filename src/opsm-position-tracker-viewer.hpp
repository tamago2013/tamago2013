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
			gnd_semvariable< gnd::queue< gnd::gl::point > > scan_2prev;	///< previous point
			gnd_semvariable< gnd::queue< gnd::gl::point > > scan_prev;	///< previous point
			gnd_semvariable< gnd::queue< gnd::gl::point > > scan_cur;	///< current point

			struct pos2di {
				int x;
				int y;
			};
			gnd_semvariable< struct pos2di > 				mouse_pointer;	///< mouse_pointer
			struct pos2dd {
				double x;
				double y;
			} offset;
			struct position {
				double x;
				double y;
				double t;
			};
			gnd_semvariable< position > robot_pos;					///< robot position

			inline
			void display() {
				::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);


				gnd::gl::project();

				scan_2prev.wait();
				gnd::gl::draw_points( scan_2prev.var.begin(), scan_2prev.var.size(), 2, 1.0, 1.0, 1.0);
				scan_2prev.post();

				scan_prev.wait();
				gnd::gl::draw_points( scan_prev.var.begin(), scan_prev.var.size(), 2, 1.0,  0, 0);
				scan_prev.post();

				scan_cur.wait();
				gnd::gl::draw_points( scan_cur.var.begin(), scan_cur.var.size(), 2, 0, 1.0, 0);
				scan_cur.post();

				{
					gnd::gl::triangle t;

					robot_pos.wait();
					{
						gnd::gl::vertex_isoscales_triangle(&t, robot_pos.var.x, robot_pos.var.y, robot_pos.var.t, 0.5, 0.70710678 );
						gnd::gl::camera.center.x = robot_pos.var.x + offset.x;
						gnd::gl::camera.center.y = robot_pos.var.y + offset.y;
					}
					robot_pos.post();

					gnd::gl::draw_triangle_color( &t, 1, 1.0, 1.0, 1.0);
					gnd::gl::camera.eye.x = gnd::gl::camera.center.x;
					gnd::gl::camera.eye.y = gnd::gl::camera.center.y;
				}

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
				case GLUT_LEFT_BUTTON:
				case GLUT_RIGHT_BUTTON:
					mouse_pointer.wait();
					mouse_pointer.var.x = x;
					mouse_pointer.var.y = y;
					mouse_pointer.post();
					break;

				case 3:
					gnd::gl::get_field(&f);
					tmp = f / 1.1;
					f = tmp < 1.0  ? 1.0 : tmp;
					gnd::gl::set_field(f);
					break;
				case 4:
					gnd::gl::get_field(&f);
					gnd::gl::set_field(f*1.1);
					break;
				}
			}

			inline
			void motion(int x, int y) {
				int l, r;

				gnd::gl::mouse.left(&l, 0, 0);
				gnd::gl::mouse.right(&r, 0, 0);

				if( l == GLUT_DOWN ) {
					mouse_pointer.wait();
					{
						offset.x += - (double)(x - mouse_pointer.var.x) / gnd::gl::window.field / gnd::gl::window.w;
						offset.y += (double)(y - mouse_pointer.var.y) / gnd::gl::window.field / gnd::gl::window.h;
					}
					mouse_pointer.post();
				}

				mouse_pointer.wait();
				{
					mouse_pointer.var.x = x;
					mouse_pointer.var.y = y;
				}
				mouse_pointer.post();
			}

			inline
			void reshape(int w, int h) {
				//	::view_display();
			}
		}
	}
}

#endif /* OBSERVATION_PROBABILITY_POSITION_TRACKER_VIEW_HPP_ */
