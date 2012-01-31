/*
 * urg-handler.hpp
 *
 *  Created on: 2012/01/27
 *      Author: tyamada
 */

#ifndef URG_HANDLER_HPP_
#define URG_HANDLER_HPP_

#include <scip2awd.h>
#include "ssm-laser.hpp"

#define GND_DEBUG_LOG	URGProxy
#include "gnd-debug-log.hpp"
#undef	GND_DEBUG_LOG

#ifndef NDEBUG
#define fLog_Debug(f, ...) _debug_log_(Log_Debug, f, __VA_ARGS__)
#define fLog_Verbose(f, ...) _debug_log_(Log_Verbose, f, __VA_ARGS__)
#define Log_Debug(s) _debug_log_(Log_Debug, s)
#define Log_Verbose(s) _debug_log_(Log_Verbose, s)
#else
#define fLog_Debug(f, ...)
#define fLog_Verbose(f, ...)
#define Log_Debug(s)
#define Log_Verbose(s)
#endif


// ---> constant value definition
namespace URGProxy {
	static const long SCIP2TimeMax = (1 << 24) - 1;
}
// <--- constant value definition

// ---> type definition
namespace URGProxy {
	static const int DriftError = 1.0e-3;

	/**
	 * @biref time-adjsut
	 */
	struct TimeAdjust {
		TimeAdjust();
		int poll;		///< polling time parameter
		double device;	///< device clock time
		double host;	///< host clock time
		double drift;	///< host / device
		size_t loop;	///< device clock loop
	};
	typedef struct TimeAdjust TimeAdjust;

	/**
	 * @brief time-adjust property
	 */
	struct TimeAdjustProperty{
		TimeAdjustProperty();
		int min_poll;	///< minimum polling time
		int max_poll;	///< maximum polling time
		double delta;	///< error threshold
		double inv_z;	///< low path filter
	};
	typedef struct TimeAdjustProperty TimeAdjustProperty;


	TimeAdjust::TimeAdjust() {
		poll = 0;
		device = 0;
		host = 0;
		drift = 1.0;
		loop = 0;
	}

	TimeAdjustProperty::TimeAdjustProperty(){
		min_poll = 0;
		max_poll = 0;
		delta = gnd_msec2sec(3);
		inv_z = 0.5;
	}

	/**
	 * @brief scanning property
	 */
	struct ScanningProperty {
		ScanningProperty();
		struct {
			int min;
			int max;
		} step;			///< step
		int decimate;	///< scanning data decimate
		int sheaf;		///< leaser sheaf
		bool intensity;	///< intensity
		int step_front;				///< device front step
		double angle_resolution;	///< resolution of angle
	};
	typedef ScanningProperty Scanning;
	ScanningProperty::ScanningProperty() {
		step.min = 0;
		step.max = 0;
		decimate = 0;
		sheaf = 0;
		intensity = false;
		step_front = 0;
		angle_resolution = 0.0;
	}

	struct ssm_property {
		int id;
		yp_matrix_fixed<4,4> coord;		///<! relation from parent coordinate(robot coordinate)
		ssm_property();
	};

	ssm_property::ssm_property() {
		yp_matrix_set_unit(&coord);
	}

} // <--- type definition


// ---> function declaration
namespace URGProxy {
	int scanning_begin( S2Port *p, ScanningProperty *s, S2Sdd_t* d);
	int scanning_reading( S2Scan_t* d, ScanningProperty *p );

	int timeadjust_initialize( TimeAdjustProperty *pr, TimeAdjust *t, struct timeval *h, struct timeval *d );
	int timeadjust( TimeAdjustProperty *pr, TimeAdjust *t, struct timeval *h, struct timeval *d );
	double timeadjust_polltime( TimeAdjust *t );
	int timeadjust_device2host( double d, TimeAdjust *t, double *h );

	int show_version(FILE* fp, S2Ver_t *v);
	int show_parameter(FILE* fp, S2Param_t *p);
}
// <--- function declaration



// ---> function definition
namespace URGProxy {
	inline
	int scanning_begin( S2Port* p, ScanningProperty *s, S2Sdd_t* d) {
		if( s->intensity){
			if( !::Scip2CMD_StartMS( p,  s->step.min,  s->step.max,  s->sheaf,  s->decimate, 0, d, SCIP2_ENC_3X2BYTE) ) {
				return -1;
			}
		}
		else {
			if( !::Scip2CMD_StartMS( p, s->step.min, s->step.max, s->sheaf, s->decimate, 0, d, SCIP2_ENC_3BYTE) ) {
				return -1;
			}
		}
		return 0;
	}


	inline
	int scanning_reading( S2Scan_t* d, ScanningProperty *p, ssm::ScanPoint2D *s ) {
		int offset = d->start - p->step_front;
		int cnt = 0;
		int stepcnt = p->intensity ? 2 : 1;

		for( int i = 0; i < d->size; i += stepcnt ){
			ssm::MeasuredPoint2DPolar comp;
			// set range
			comp.r = d->data[i] * 1.0e-3;
			// set direction
			comp.th = p->angle_resolution * ( i * d->group / stepcnt + offset );

			// set intensity
			if( p->intensity )
				comp.intensity = static_cast<int>(d->data[i+1]);
			else
				comp.intensity = -1;

			// set status
			if( d->data[i] == 1 ){
				comp.status = ssm::laser::STATUS_NO_REFLECTION;
			}
			else if( d->data[i] < 20 ){
				comp.status = ssm::laser::STATUS_ERROR;
			}
			else {
				comp.status = ssm::laser::STATUS_OK;
			}
			// set
			(*s)[cnt++] = comp;
		}

		return cnt;
	}


	inline
	int timeadjust_initialize( TimeAdjustProperty *pr, TimeAdjust *t, struct timeval *d, struct timeval *h ) {
		t->loop = 0;
		t->poll = pr->min_poll;
		// drift
		t->drift = 1.0;
		// host
		t->host = gnd_timeval2time(h);
		// device
		t->device = gnd_timeval2time(d);
		return 0;
	}

	inline
	int timeadjust(	TimeAdjustProperty* pr, TimeAdjust *t, struct timeval *d, struct timeval *h ) {
		yp_assert(!pr, -1, "invalid null argument");
		yp_assert(!t, -1, "invalid null argument");
		yp_assert(!d, -1, "invalid null argument");
		yp_assert(!h, -1, "invalid null argument");
		yp_error(pr->min_poll == 0, -1, "invalid property");
		yp_error(pr->min_poll > pr->max_poll, -1, "invalid property");

		fLog_Debug("Begin:int timeadjust(%p, %p, %p, %p)\n", pr, t, d, h);

		{ // ---> operation
			double dtime_d, htime_d;

			// delta on host time
			htime_d = gnd_timeval2time(h);
			// delta on device time
			dtime_d = gnd_timeval2time(d);

			{ // ---> set poll
				double d2h, diff;
				timeadjust_device2host(dtime_d, t, &d2h);

				diff = ::fabs(d2h - htime_d);
				if( diff < pr->delta / 2 ) {
					if(t->poll < pr->max_poll)	t->poll++;
				}
				else if( diff > pr->delta ) {
					if(t->poll > pr->max_poll)	t->poll--;
				}
				fLog_Debug("     : poll %d\n", t->poll );
			} // <--- set poll

			{ // --->  drift
				double ddelta, hdelta, drift;
				hdelta = htime_d - t->host;
				ddelta = dtime_d - t->device;
				while(ddelta < 0) {
					t->loop++;
					ddelta += SCIP2TimeMax;
				}
				drift = (hdelta / ddelta);
				if( ::fabs( 1.0 - drift ) > DriftError )	t->drift = 1.0;
				else	t->drift += (drift - t->drift) * ( 1 - pr->inv_z);

				fLog_Debug("     : drift %lf\n", t->drift );
			} // <--- drift
			// host
			t->host = htime_d;
			// device
			t->device = dtime_d;
		} // <--- operation

		fLog_Debug("  End:int timeadjust(%p, %p, %p, %p)\n", pr, t, d, h);
		return 0;
	}

	inline
	double timeadjust_polltime( TimeAdjust *t ) {
		return (double) ( 1 << t->poll );
	}

	inline
	int timeadjust_device2host( double d, TimeAdjust *t, double *h ) {
		*h =  t->host + ((d - t->device) * t->drift);
		return 0;
	}


	inline
	int show_version(FILE* fp, S2Ver_t *v) {
		yp_assert(!v, -1, "invalid null argument");

		if(fp){
			::fprintf(fp, "VEND:%s\n", v->vender);
			::fprintf(fp, "PROD:%s\n", v->product);
			::fprintf(fp, "FIRM:%s\n", v->firmware);
			::fprintf(fp, "PROT:%s\n", v->protocol);
			::fprintf(fp, "SERI:%s\n", v->serialno);
		}
		fLog_Debug("VEND:%s\n", v->vender);
		fLog_Debug("PROD:%s\n", v->product);
		fLog_Debug("FIRM:%s\n", v->firmware);
		fLog_Debug("PROT:%s\n", v->protocol);
		fLog_Debug("SERI:%s\n", v->serialno);

		return 0;
	}

	inline
	int show_parameter(FILE* fp, S2Param_t *p) {
		yp_assert(!p, -1, "invalid null argument");

		if(fp){
			::fprintf(fp, "MODL:%s\n", p->model);
			::fprintf(fp, "DMIN:%d\n", p->dist_min);
			::fprintf(fp, "DMAX:%d\n", p->dist_max);
			::fprintf(fp, "ARES:%d\n", p->revolution);
			::fprintf(fp, "AMIN:%d\n", p->step_min);
			::fprintf(fp, "AMAX:%d\n", p->step_max);
			::fprintf(fp, "AFRT:%d\n", p->step_front);
			::fprintf(fp, "SCAN:%d\n", p->step_resolution);
		}
		fLog_Debug("MODL:%s\n", p->model);
		fLog_Debug("DMIN:%d\n", p->dist_min);
		fLog_Debug("DMAX:%d\n", p->dist_max);
		fLog_Debug("ARES:%d\n", p->revolution);
		fLog_Debug("AMIN:%d\n", p->step_min);
		fLog_Debug("AMAX:%d\n", p->step_max);
		fLog_Debug("AFRT:%d\n", p->step_front);
		fLog_Debug("SCAN:%d\n", p->step_resolution);

		return 0;
	}

} // <--- function definition



#undef fLog_Debug
#undef fLog_Verbose
#undef Log_Debug
#undef Log_Verbose


#endif /* URG_HANDLER_HPP_ */
