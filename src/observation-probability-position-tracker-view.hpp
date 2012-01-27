/*
 * observation-probability-position-tracker-view.hpp
 *
 *  Created on: 2012/01/10
 *      Author: tyamada
 */

#ifndef OBSERVATION_PROBABILITY_POSITION_TRACKER_VIEW_HPP_
#define OBSERVATION_PROBABILITY_POSITION_TRACKER_VIEW_HPP_

#include <time.h>
#include <gnd-thread.hpp>
#include <gnd-semaphore.hpp>
#include <ssm-laser.hpp>
#include <yp-lib-error.h>

#include <GL/gl.h>
#include <GL/glut.h>

struct point {
	double x;
	double y;
};


class matching_view;

class matching_view_data
{
	friend class matching_view;
private:
	matching_view_data();
public:
	~matching_view_data();

private:
	double x, y;
	struct point cur[1081];
	size_t ncur;
	struct point prev[1081];
	size_t nprev;
	struct point pos;
	gnd_semaphore	sem;

public:
	int begin();
	int set(double x, double y);
	int end();
	int set_pos(double x, double y);
};

inline
matching_view_data::matching_view_data(){
	x = 0;
	y = 0;
	ncur = 0;
	nprev = 0;
}

inline
matching_view_data::~matching_view_data(){
}

inline
int matching_view_data::begin()
{
	sem.wait();
	::memcpy(prev, cur, sizeof(cur));
	nprev = ncur;
	ncur = 0;

	return 0;
}

inline
int matching_view_data::set(double x, double y){
	cur[ncur].x = x;
	cur[ncur].y = y;
	ncur++;

	return 0;
}

inline
int matching_view_data::end(){
	sem.post();
	return 0;
}

inline
int matching_view_data::set_pos(double x, double y){
	pos.x = x;
	pos.y = y;

	return 0;
}



class matching_view
: public gnd_basic_thread
{
public:
	static matching_view view;
	static matching_view_data data;

// ---> constructor, destructor
protected:
	matching_view();
public:
	~matching_view();

public:
	int init(int *argcp, char** argv);
public:
	int create();
private:
	static void __display__();
	static void __redisplay__(int v);

public:
	void* thread_routine();
};
// instance view object
matching_view matching_view::view;
matching_view_data matching_view::data;



matching_view::matching_view(){

}

matching_view::~matching_view(){

}

inline
int matching_view::init(int *argcp, char** argv)
{
	yp_assert(!argcp, -1, "invalid null pointer" );
	yp_assert(!argv, -1, "invalid null pointer" );

	glutInit(argcp, argv);
	return 0;
}


inline
int matching_view::create()
{
	int ret;
	if(!thread_is_free()){
		return ETHFREE;
	}

	// detach
	thread_attr_set_detachstate(PTHREAD_CREATE_DETACHED);
	// thread create
	if((ret = thread_create()) < 0){
		return ret;
	}

	thread_join();
	return 0;
}




inline
void* matching_view::thread_routine()
{
	// initialize
	::glutInitWindowPosition(0, 0);
	::glutInitWindowSize(400, 400);
	::glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

	// create window
	::glutCreateWindow("view");

	::glutDisplayFunc( __display__ );
	::glutTimerFunc((int)(1.0 / 30), __redisplay__, 0);

	::glutMainLoop();
	return 0;
}



inline
void matching_view::__display__()
{
	struct point cur[1081], prev[1081];
	size_t ncur, nprev;
	struct point pos;

	{ // ---> copy
		data.sem.wait();
		pos = data.pos;
		::memcpy(&cur, &data.cur, sizeof(cur));
		ncur = data.ncur;
		::memcpy(&prev, &data.prev, sizeof(prev));
		nprev = data.nprev;
		data.sem.post();
	} // <--- copy

	::glClear(GL_COLOR_BUFFER_BIT);
	::glColor3d(1.0, 0, 0);
	::glPointSize(1);
	::glBegin(GL_POINTS);
	for( size_t i = 0; i < ncur; i++ ){
		::glVertex2d( (cur[i].x - pos.x) / 5.0 , (cur[i].y - pos.y) / 5.0);
	}
	::glEnd();

	::glColor3d(0, 1.0, 0);
	::glBegin(GL_POINTS);
	::glPointSize(1);
	for( size_t i = 0; i < nprev; i++ ){
		::glVertex2d( (prev[i].x - pos.x) / 5.0 , (prev[i].y - pos.y) / 5.0);
	}
	::glEnd();
	// drow
	::glFlush();
}




inline
void matching_view::__redisplay__(int v)
{
	::glutPostRedisplay();
	::glutTimerFunc( (int)(1.0 / 30), __redisplay__, 0);
}



#endif /* OBSERVATION_PROBABILITY_POSITION_TRACKER_VIEW_HPP_ */
