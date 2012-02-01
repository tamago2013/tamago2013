//============================================================================
// Name        : localizer.cpp
// Author      : tyamada
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <signal.h>

#include "gnd-util.h"
#include "gnd-random.hpp"
#include "gnd-mahalanobis.hpp"

#include <ypspur.h>

#include <ssm.h>
#include <ssm.hpp>
#include <ssmtype/spur-odometry.h>
#include <ssmtype/pws-motor.h>
#include <ssmtype/ypspur-ad.h>


#include "ssm-particles.hpp"

#include "particle-localizer-opt.hpp"
#include "gnd-cui.hpp"
#include "gnd-util.h"

bool gShutOff = false;
void ShutOff(int stat);

const char _Debug_Log_[] = "debug.log";


const struct gnd_cui_cmd cui_cmd[] = {
		{"Exit",		'E',	"localizer shut-off"},
		{"help",		'h',	"show help"},
		{"s",			's',	"state show mode"},
		{"show",		's',	"state show mode"},
		{"stand-by",	'B',	"operation stop and wait cui-command"},
		{"start",		'o',	"start operation"},
		{"start-at",	'S',	"initialize on way-point"},
		{"wide",		'w',	"change wide sampling mode on/off"},
		{"debug-log",	'd',	"change debug mode on/off"},
		{"", '\0'}
};



int read_kinematics_config(double* radius_r, double* radius_l, double* tread,
		double* gear, double* count_rev, const char* fname);

/*
 * @brief main
 */
int main(int argc, char* argv[], char *envp[]) {
	SSMApi<PWSMotor>  mtr;
	SSMApi<YP_ad> ad;
	SSMApi<Spur_Odometry> ssm_position;
	SSMParticles ssm_particle;
	SSMParticleEvaluation ssm_estimation;

	gnd_cui gcui;
	Localizer::proc_configuration param;
	Localizer::proc_option opt(&param);
	yp_matrix_fixed_c<1, PARTICLE_DIM> myu_ini;

	double wheel_mean = 0,
			wheel_ratio = 0,
			tread_ratio = 0;
	double gear = 0,
			count_rev = 0,
			revl_ratio = 0;
	Spur_Odometry_Property odm_prop;




	{ // ---> Initialize
		size_t phase = 1;
		double inittime = -1;
		// todo make parameter file
		myu_ini.set_zero();

		// get option
		if( !opt.get_option(argc, argv) ){
			return 0;
		}



		::fprintf(stderr, "========== Initialize ==========\n");
		::fprintf(stderr, " %d. allocate signal \"\x1b[4mSIGINT\x1b[0m\" to shut-off\n", phase++);
		::fprintf(stderr, " %d. get kinematics parameter\n", phase++);
		if(opt.op_mode.start_at)
			::fprintf(stderr, " %d. load start way-point\n", phase++);
		::fprintf(stderr, " %d. initialize \x1b[4mssm\x1b[0m\n", phase++);
		::fprintf(stderr, " %d. create ssm-data \"\x1b[4m%s\x1b[0m\"\n", phase++, SNAME_ADJUST );
		::fprintf(stderr, " %d. create initial particles and wirte initial ssm-data \"\x1b[4m%s\x1b[0m\"\n", phase++ , SNAME_ADJUST);
		::fprintf(stderr, " %d. create ssm-data \"\x1b[4m%s\x1b[0m\"\n", phase++, SNAME_PARTICLES );
		::fprintf(stderr, " %d. create ssm-data \"\x1b[4m%s\x1b[0m\"\n", phase++, SNAME_PARTICLES_EVALUATION );
		::fprintf(stderr, " %d. open ssm-data \"\x1b[4m%s\x1b[0m\"\n", phase++, SNAME_PWS_MOTOR );
		if( param.gyro.value)
			::fprintf(stderr, " %d. open ssm-data \"\x1b[4m%s\x1b[0m\"\n", phase++, SNAME_YPSPUR_AD );
		::fprintf(stderr, "\n");


		gShutOff = false;
		{ // ---> allocate SIGINT to shutoff
			struct sigaction sigact;

			::memset(&sigact, 0, sizeof(sigact));

			::fprintf(stderr, "\n");
			::fprintf(stderr, " => allocate signal \"\x1b[4mSIGINT\x1b[0m\" to shut-off\n");
			sigact.sa_handler = ShutOff;
			sigact.sa_flags |= SA_RESETHAND | SA_RESTART;

			if( sigaction(SIGINT, &sigact, 0) < 0){
				::fprintf(stderr, "  \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to allocate \"\x1b[4mSIGINT\x1b[0m\" to shut-off\n" );
				gShutOff = true;
			}
			else {
				::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m\n");
			}
		} // <--- allocate SIGINT to shutoff


		{ // ---> set initial kinematics
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => get initial kinematics parameter\n");

			{ // ---> set-zero kinematics property
				odm_prop.radius_l = 0;
				odm_prop.radius_r = 0;
				odm_prop.tread = 0;
				gear = 0;
				count_rev = 0;
			} // <--- set-zero kinematics property

			// read kinematics configure file
			if(*param.kfile.value != '\0'){
				::fprintf(stderr, "    load kinematics parameter file \"\x1b[4m%s\x1b[0m\"\n", param.kfile.value);
				read_kinematics_config(&odm_prop.radius_r, &odm_prop.radius_l, &odm_prop.tread,
						&gear, &count_rev, param.kfile.value);
			}

			// check kinematic parameter
			if( odm_prop.radius_r > 0 && odm_prop.radius_l > 0 && odm_prop.tread > 0 && gear > 0 && count_rev > 0){
				::fprintf(stderr, "    load lacking parameter from \x1b[4mypsypur-coordinator\x1b[0m\n");
			}
			else {
				// initialize for using ypspur-coordinator
				if( Spur_init() < 0){
					::fprintf(stderr, "  ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to connect ypspur-coordinator.\n");
					gShutOff = true;
				}

				// get kinmeatics from ypspur-coordinator
				if( !gShutOff && YP_get_parameter( YP_PARAM_RADIUS_R, &odm_prop.radius_r ) < 0 ){
					gShutOff = true;
				}
				if( !gShutOff && YP_get_parameter( YP_PARAM_RADIUS_L, &odm_prop.radius_l ) < 0 ){
					gShutOff = true;
				}
				if( !gShutOff && YP_get_parameter( YP_PARAM_TREAD, &odm_prop.tread ) < 0 ){
					gShutOff = true;
				}

				if( !gShutOff && YP_get_parameter(YP_PARAM_COUNT_REV, &count_rev) < 0 ){
					gShutOff = true;
				}
				if( !gShutOff && YP_get_parameter(YP_PARAM_GEAR, &gear) < 0 ){
					gShutOff = true;
				}
			}
			// <--- get parameter from ypspur-coordinater

			// check kinematic parameter
			if( odm_prop.radius_r <= 0 || odm_prop.radius_l <= 0 || odm_prop.tread <= 0 || gear <= 0 || count_rev <= 0){
				::fprintf(stderr, "  => \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m : Incomplete loading kinematics parameter.\n");
				gShutOff = true;
			}

			// set kinematics
			if( !gShutOff ){
				// wheel mean
				wheel_mean = (odm_prop.radius_l + odm_prop.radius_r) / 2.0;
				wheel_ratio = odm_prop.radius_l / wheel_mean;
				tread_ratio = odm_prop.tread / wheel_mean;

				if( !param.gyro.value ){
					myu_ini.set(0, PARTICLE_WHEEL_MEAN,  wheel_mean);
					myu_ini.set(0, PARTICLE_WHEEL_RATIO, wheel_ratio);
					myu_ini.set(0, PARTICLE_TREAD_RATIO, tread_ratio);
				}
				else {
					myu_ini.set(0, PARTICLE_WHEEL_MEAN,  wheel_mean);
					myu_ini.set(0, PARTICLE_GYRO_BIAS, param.gyro_bias.value);
					myu_ini.set(0, PARTICLE_GYRO_SF, param.gyro_sf.value);
				}
				ssm_particle.data.init_kinematics(count_rev, gear);

				revl_ratio = count_rev * gear;
			}

		} // <--- set initial kinematics



		 // ---> get configure
		if( !gShutOff ){

			Localizer::proc_conf_sampling_ratio_normalize(&param);
			Localizer::proc_conf_kinematics_parameter_materialize(&param, wheel_mean, wheel_ratio, tread_ratio);

			{ // ---> initlaize min cov
				yp_matrix_fixed<3, 3> min_cov;

				yp_matrix_set_zero(&min_cov);

				yp_matrix_set(&min_cov, 0, 0, 1.0e-20);
				yp_matrix_set(&min_cov, 1, 1, 1.0e-20);
				yp_matrix_set(&min_cov, 2, 2, 1.0e-20);

				yp_matrix_add(&param.poserr_covar,			&min_cov,		&param.poserr_covar);
				yp_matrix_add(&param.knm_covar,				&min_cov,		&param.knm_covar);
				yp_matrix_add(&param.poserr_covar_static,	&min_cov,		&param.poserr_covar_static);
				yp_matrix_add(&param.knm_covar,				&min_cov,		&param.knm_covar);
				yp_matrix_add(&param.wknm_covar, 			&min_cov, 		&param.wknm_covar);

			} // <--- initlaize min cov
		} // <--- get configure


		// ---> set initialize position covariance matrix
		if( !gShutOff ){
			ssm_estimation.property.n = param.particles.value + param.random_sampling.value;
			ssm_estimation.data.n = param.particles.value + param.random_sampling.value;
			ssm_estimation.data.value = new double [ssm_estimation.data.n];
		} // <--- set initialize position covariance matrix


		// ---> initialize ssm
		if( !gShutOff ) {
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => initialize SSM\n");
			if( !initSSM() ){
				::fprintf(stderr, "  \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: SSM is not available.\n");
				gShutOff = true;
			}
			else {
				::fprintf(stderr, "   ... \x1b[1mOK\x1b[0m\n");
			}
		} // <--- initialize ssm


		// ---> ssm open
		if( !gShutOff ) {
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => create ssm-data \"\x1b[4m%s\x1b[0m\"\n", SNAME_ADJUST );
			if( !ssm_position.create( SNAME_ADJUST, 0, 1, 0.005) ){
				::fprintf(stderr, "  \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to create \"\x1b[4m%s\x1b[0m\"\n", SNAME_ADJUST );
				gShutOff = true;
			}
			else {
				::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m\n");
			}
		} // <--- ssm open



		if( !gShutOff ){ // ---> write initial position
			::fprintf(stderr, " => create initial particles and write into ssm\n" );

			// create initialize position
			ssm_particle.data.init_particle(myu_ini.base(),
					&param.pos_ini_covar, &param.knm_ini_covar, param.particles.value + param.random_sampling.value);

			// set property
			ssm_particle.property.n = param.particles.value + param.random_sampling.value;

			// set position
			ssm_position.data.x = ssm_particle.data.pos.odo.x;
			ssm_position.data.y = ssm_particle.data.pos.odo.y;
			ssm_position.data.theta = ssm_particle.data.pos.odo.theta;
			ssm_position.data.v = 0;
			ssm_position.data.w = 0;

			// show
			::fprintf(stderr, "  x  - %lf\n", ssm_position.data.x);
			::fprintf(stderr, "  y  - %lf\n", ssm_position.data.y);
			::fprintf(stderr, "  th - %lf\n", ssm_position.data.theta);

			// ---> write position to ssm
			if( inittime > 0){
				if( !ssm_position.write( inittime ) && !ssm_position.write( ) ){
					gShutOff = true;
					::fprintf(stderr, "  \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to write \"\x1b[4m%s\x1b[0m\"\n", SNAME_ADJUST );
				}
				else {
					::fprintf(stderr, "   ... \x1b[1mOK\x1b[0m\n");
				}
			}
			else if( !gShutOff ){
				if( !ssm_position.write( ) ){
					gShutOff = true;
					::fprintf(stderr, "  \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to write \"\x1b[4m%s\x1b[0m\"\n", SNAME_ADJUST );
				}
				else {
					::fprintf(stderr, "   ... \x1b[1mOK\x1b[0m\n");
				}
			} // <--- write position to ssm
		} // <--- write initial position


		// ---> ssm perticles open
		if( !gShutOff ){
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => create ssm-data \"\x1b[4m%s\x1b[0m\"\n", SNAME_PARTICLES );
			if( !ssm_particle.create( SNAME_PARTICLES, 0, 5, 0.005 ) ){
				::fprintf(stderr, "  \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to create \"\x1b[4m%s\x1b[0m\"\n", SNAME_PARTICLES );
				gShutOff = true;
			}
			else {
				ssm_particle.setProperty();
				::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m\n");
			}
		} // <--- ssm perticles open


		// ---> ssm perticles evaluation open
		if( !gShutOff ){
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => create ssm-data \"\x1b[4m%s\x1b[0m\"\n", SNAME_PARTICLES_EVALUATION );
			if( !ssm_estimation.create( SNAME_PARTICLES_EVALUATION, 0, 5, 0.1 ) ){
				::fprintf(stderr, "  \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to open \"\x1b[4m%s\x1b[0m\"\n", SNAME_PARTICLES_EVALUATION );
				gShutOff = true;
			}
			else {
				ssm_estimation.setProperty();
				::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m\n");
			}
		}// <--- ssm perticles evaluation open

		// ---> ssm pws motor open
		if( !gShutOff ){
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => open ssm-data \"\x1b[4m%s\x1b[0m\"\n", SNAME_PWS_MOTOR );
			if( !gShutOff && !mtr.openWait( SNAME_PWS_MOTOR, 0, 0.0) ){
				::fprintf(stderr, "  \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to create \"\x1b[4m%s\x1b[0m\"\n", SNAME_PWS_MOTOR );
				gShutOff = true;
			}
			else {
				// next read new data
				mtr.readLast();
				// blocking
				mtr.setBlocking(true);
				::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m\n");
			}
		} // <--- ssm pws motor open


		// ---> ssm ad open
		if( param.gyro.value && !gShutOff ){
			::fprintf(stderr, "\n");
			::fprintf(stderr, " => open ssm-data \"\x1b[4m%s\x1b[0m\"\n", SNAME_YPSPUR_AD );
			if( !gShutOff && !ad.openWait( SNAME_YPSPUR_AD, 0, 0.0) ){
				::fprintf(stderr, "  \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to create \"\x1b[4m%s\x1b[0m\"\n", SNAME_YPSPUR_AD );
				gShutOff = true;
			}
			else {
				// next read new data
				ad.readLast();
				// blocking
				ad.setBlocking(false);
				::fprintf(stderr, "  ... \x1b[1mOK\x1b[0m\n");
			}
		} // <--- ssm ad open

		// initialize cui
		gcui.set_command(cui_cmd, sizeof(cui_cmd) / sizeof(cui_cmd[0]));

	} // <--- Initialize







	if( !gShutOff ){ // ---> operation
		int rsmpl_cnt = 0,
			reject_cnt = 0;
		ssmTimeT rsmp_time = 0;
		ssmTimeT prev_time = mtr.time;
		bool show_st = true;

		double eval_ave_slow = 0;
		static const double eval_alpha_slow = 1.0 / 180;
		double eval_ave_fast = 0;
		static const double eval_alpha_fast = 1.0 / 60;
		double wide_sampling_rate = 0;
		size_t nparticle_remain = param.particles.value * param.remain.value;
		size_t nparticle_pos = param.particles.value * param.poserr.value;
		size_t nparticle_knm = param.particles.value * param.knm.value;
		size_t nparticle_wknm = param.particles.value * param.wknm.value;
		size_t nparticle_knm_reset = param.particles.value - nparticle_remain - nparticle_pos
				- nparticle_knm - nparticle_wknm;

		int enc_cnt_pos = 0;
		int enc_cnt_knm = 0;
		int enc_cnt_wknm = 0;
		FILE *dbgfp = 0;
		double cuito = 0;;

		{ // ---> show
			::fprintf(stderr, "\n\n");
			::fprintf(stderr, "========== Oeration ==========\n");
			::fprintf(stderr, " => start\n");
			::fprintf(stderr, " \x1b[1m%s\x1b[0m > ", Localizer::particle_filter);
		} // <--- show

		// ---> debug log open
		if(opt.op_mode.debug){
			::fprintf(stderr, " => debug-log mode\n");
			if( !(dbgfp = fopen(_Debug_Log_, "w")) ) {
				::fprintf(stderr, "  ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open \"\x1b[4m%s\x1b[0m\"", _Debug_Log_);
				opt.op_mode.debug = false;
			}
			else {
				fprintf(dbgfp, "# 1.[time] 2.[x] 3.[y] 4.[theta] 5.[v] 6.[w] 7.[wh mean] 8.[wh ratio] 9.[trd ratio] 10.[pwm1] 11.[pwm2] 12.[counter1] 13.[counter2]\n");
			}
		} // <--- debug log open


		while(!gShutOff){

			{ // ---> cui
				int cuival = 0;
				char cuiarg[512];

				::memset(cuiarg, 0, sizeof(cuiarg));
				if( gcui.poll(&cuival, cuiarg, sizeof(cuiarg), cuito) > 0 ){
					if( show_st ){
						// if show status mode, quit show status mode
						show_st = false;
						::fprintf(stderr, "-------------------- cui mode --------------------\n");
					}
					else {
						switch(cuival) {
						// exit
						case 'e': gShutOff = true; break;
						// help
						case 'h': gcui.show(stderr, "   "); break;
						// debug log-mode
						case 'd': {
							::fprintf(stderr, "   => debug-log mode\n");
							if( ::strncmp("on", cuiarg, 2) == 0){
								bool flg = dbgfp ? true : false;

								if( !dbgfp && !(dbgfp = fopen(_Debug_Log_, "w")) ) {
									::fprintf(stderr, "    ... \x1b[1m\x1b[31mError\x1b[39m\x1b[0m: fail to open \"\x1b[4m%s\x1b[0m\"", _Debug_Log_);
									opt.op_mode.debug = false;
								}
								else if( flg ){
									fprintf(dbgfp, "# 1.[time] 2.[x] 3.[y] 4.[theta] 5.[v] 6.[w] 7.[wh mean] 8.[wh ratio] 9.[trd ratio] 10.[pwm1] 11.[pwm2] 12.[counter1] 13.[counter2]\n");
									opt.op_mode.debug = true;
									::fprintf(stderr, "    ... \x1b[1mon\x1b[0m\n");
								}
								else {
									opt.op_mode.debug = true;
									::fprintf(stderr, "    ... \x1b[1mon\x1b[0m\n");
								}
							}
							else if( ::strncmp("off", cuiarg, 3) == 0){
								opt.op_mode.debug = false;
								::fprintf(stderr, "    ... \x1b[1moff\x1b[0m\n");
							}
							else {
								::fprintf(stderr, "   ... %s\n", opt.op_mode.debug ? "on": "off");
								::fprintf(stderr, "   if you want to change mode, input \"on/off\"\n");
							}
						}
						break;
						// show status
						case 's': show_st = true;	break;
						// stand-by mode
						case 'B': cuito = -1;		break;
						// start
						case 'o': cuito = 0;		break;

						case '\0':
							break;
						default:
							::fprintf(stderr, "   ... \x1b[31m\x1b[1mError\x1b[0m\x1b[39m: invalid command\n");
							::fprintf(stderr, "       Please input \x1b[4mhelp\x1b[0m/\x1b[4mh\x1b[0m to show command\n");
							break;

						}
					}
					::fprintf(stderr, "  \x1b[33m\x1b[1m%s\x1b[0m\x1b[39m > ", Localizer::particle_filter);
					gcui.poll(&cuival, cuiarg, sizeof( cuiarg ), 0);
				}
			}// ---> cui


			if( show_st ){ // ---> show status
				struct timespec cur;
				static struct timespec next;
				clock_gettime(CLOCK_REALTIME, &cur);

				if( cur.tv_sec > next.tv_sec ||
						( cur.tv_sec == next.tv_sec && cur.tv_nsec > next.tv_nsec )){

					::fprintf(stderr, "\x1b[0;0H\x1b[2J");	// display clear
					::fprintf(stderr, "-------------------- \x1b[33m\x1b[1m%s\x1b[0m\x1b[39m --------------------\n", Localizer::particle_filter);
					::fprintf(stderr, "    particle : %d\n", ssm_particle.data.row() );
					::fprintf(stderr, "             : r %d, p %d, k %d, wk %d rk %d\n", nparticle_remain, nparticle_pos, nparticle_knm, nparticle_wknm, nparticle_knm_reset);
					::fprintf(stderr, "    resample : %d,   reject %d\n", rsmpl_cnt, reject_cnt );
					::fprintf(stderr, "    position : %.02lf %.02lf %.01lf\n",  ssm_particle.data.pos.odo.x,  ssm_particle.data.pos.odo.y,  gnd_ang2deg(ssm_particle.data.pos.odo.theta) );
					::fprintf(stderr, "    velocity : v %.02lf  w %.03lf\n", ssm_position.data.v, gnd_ang2deg(ssm_position.data.w) );
					::fprintf(stderr, "  kinematics : %.05lf %.05lf %.05lf\n",
							ssm_particle.data.pos.prop.wheel_odm.wheel_mean, ssm_particle.data.pos.prop.wheel_odm.wheel_ratio,
							param.gyro.value ? 1 / gnd_rad2deg( ssm_particle.data.pos.prop.wheel_odm.tread_ratio ) : ssm_particle.data.pos.prop.wheel_odm.tread_ratio );
					::fprintf(stderr, "    odometry : %s-odometry\n", param.gyro.value ? "gyro" : "wheel" );

					::fprintf(stderr, "\n");
					::fprintf(stderr, " Push \x1b[1mEnter\x1b[0m to change CUI Mode\n");
					next = cur;
					next.tv_sec++;
				}
			} // <--- show status



			// stand-by mode
			if( cuito < 0)	continue;


			// read ssm motor
			if( mtr.readNext() ){
				// compute pertilecs motion
				if( !param.gyro.value ) {
					ssm_particle.data.odometry_motion(mtr.data.counter1, mtr.data.counter2);
				}
				else {
					if( !ad.readTime(mtr.time) ) continue;
					ssm_particle.data.gyro_odometry_motion(mtr.data.counter1, mtr.data.counter2, ad.data.ad[0] * param.gyro_vol.value / (1 << param.gyro_bits.value), mtr.time - prev_time);
				}

				ssm_particle.write( mtr.time );

				// set position
				ssm_position.data.x = ssm_particle.data.pos.odo.x;
				ssm_position.data.y = ssm_particle.data.pos.odo.y;
				ssm_position.data.theta = ssm_particle.data.pos.odo.theta;


				{ // ---> compute velocity and angular velocity
					double wr = ( 2.0 * M_PI * ( (double) mtr.data.counter1 ) ) / ( revl_ratio ),
							wl = ( 2.0 * M_PI * ( (double) mtr.data.counter2 ) ) / ( revl_ratio );
					double wr2 = wr * ( 2.0 - ssm_particle.data.pos.prop.wheel_odm.wheel_ratio );
					double wl2 = wl * ssm_particle.data.pos.prop.wheel_odm.wheel_ratio;
					// robot translation quantity
					double tq = ( ssm_particle.data.pos.prop.wheel_odm.wheel_mean * ( wr2 + wl2 ) ) / 2.0;
					// robot rotation quantity
					double rq = ( wr2 - wl2 ) / ssm_particle.data.pos.prop.wheel_odm.tread_ratio ;

					ssm_position.data.v = tq / (mtr.time - prev_time);
					if( !param.gyro.value ){
						ssm_position.data.w = rq / (mtr.time - prev_time);
					}
					else {
						ssm_position.data.w = -(ad.data.ad[0] * param.gyro_vol.value / (1 << param.gyro_bits.value)
								- ssm_particle.data.pos.prop.gyro_odm.bias) * ssm_particle.data.pos.prop.gyro_odm.sf;
					}
				} // <--- compute velocity and angular velocity


				// write position to ssm
				ssm_position.write( mtr.time );
				prev_time = mtr.time;


				// log file out
				if( opt.op_mode.debug ){
					if( dbgfp ) {
						fprintf(dbgfp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %d %d %d %d %lf %lf %lf\n",
								mtr.time,
								ssm_particle.data.pos.odo.x, ssm_particle.data.pos.odo.y, ssm_particle.data.pos.odo.theta,
								ssm_position.data.v, ssm_position.data.w,
								ssm_particle.data.pos.prop.wheel_odm.wheel_mean,
								ssm_particle.data.pos.prop.wheel_odm.wheel_ratio,
								ssm_particle.data.pos.prop.wheel_odm.tread_ratio,
								mtr.data.pwm1, mtr.data.pwm2,
								mtr.data.counter1, mtr.data.counter2,
								eval_ave_slow, eval_ave_fast,
								eval_ave_slow == 0 ? 0 : 1.0 - eval_ave_fast / eval_ave_slow );
					}
				}

				// increment encoder count
				enc_cnt_pos += abs(mtr.data.counter1) + abs(mtr.data.counter2);
				enc_cnt_knm += abs(mtr.data.counter1) + abs(mtr.data.counter2);
				enc_cnt_wknm += abs(mtr.data.counter1) + abs(mtr.data.counter2);
			}

			// ---> resampling
			if( ssm_estimation.readNext() ){
				int ret;
				size_t remain = nparticle_remain;
				size_t rnoise = nparticle_pos;			// local sampling
				size_t wknm = nparticle_wknm;			// wide kinematics sampling
				size_t lknm = nparticle_knm;			// local kinematics sampling
				size_t knm_reset = nparticle_knm_reset;	// kinematics reset
				double eval_ave;


				// ---> check time
				if( ssm_estimation.time < rsmp_time ){
					reject_cnt++;
					continue;
				} // <--- check time

				if(enc_cnt_knm < 0.25 * count_rev * gear){
					remain += lknm;
					lknm = 0;
				}
				else {
					enc_cnt_knm = 0;
				}


				if(enc_cnt_wknm < 10 * count_rev * gear){
					remain += wknm;
					wknm = 0;
				}
				else {
					enc_cnt_wknm = 0;
				}


				// select remaining particle
				ret = ssm_particle.data.resampling_remain(ssm_estimation.data.value, remain, &eval_ave);


				{ // ---> deternimation wide sampling num
					eval_ave_fast += eval_alpha_fast * (eval_ave - eval_ave_fast);
					eval_ave_slow += eval_alpha_slow * (eval_ave - eval_ave_slow);
					wide_sampling_rate = 1.0 - (eval_ave_fast / eval_ave_slow);
				} // <--- deternimation wide sampling num

				// count resampling
				rsmpl_cnt++;

				if(opt.op_mode.debug) { // ---> open log file
					yp_matrix_fixed<1,PARTICLE_DIM> mean;
					char fname[128];
					static int logcnt = 0;
					FILE *fp;
					sprintf( fname, "log/%04d.log", logcnt++ );
					ssm_particle.data.mean(&mean);

					fp = fopen(fname, "w");
					if(fp){
						fprintf(fp, "#");
						yp_matrix_show(fp, &mean, "%lf");
						fprintf(fp, "#%lf %lf %lf %lf %lf %lf %lf\n",
								ssm_particle.data.pos.odo.x, ssm_particle.data.pos.odo.y, ssm_particle.data.pos.odo.theta,
								ssm_particle.data.pos.prop.wheel_odm.wheel_mean, ssm_particle.data.pos.prop.wheel_odm.wheel_ratio, ssm_particle.data.pos.prop.wheel_odm.tread_ratio,
								ssm_particle.data.pos.eval);
						ssm_particle.data.show(fp);

						fclose(fp);
					}
				} // <--- open log file

				if(ret >= 0){
					{ // ---> resampling position change particle
						yp_matrix_fixed<PARTICLE_POS_DIM, PARTICLE_POS_DIM> covp;

						yp_matrix_scalar_prod( &param.poserr_covar, enc_cnt_pos / (count_rev * gear), &covp);
						yp_matrix_add( &covp, &param.poserr_covar_static, &covp );
						ssm_particle.data.resampling_position(&covp, rnoise);
						enc_cnt_pos = 0;
					} // <--- resampling position change particle

					// resampling kinematics change particle (global)
					if(wknm > 0){
						ssm_particle.data.resampling_kinematics(&param.wknm_covar, wknm);
					}
					// resampling kinematics change particle (local)
					if(lknm > 0){
						ssm_particle.data.resampling_kinematics(&param.knm_covar, lknm);
					}
					// reset kinematics change particle
					if(knm_reset > 0){
						ssm_particle.data.resampling_kinematics_reset(myu_ini.base(), &param.reset_knm_covar , knm_reset);
					}
				}

				// write ssm
				ssm_particle.write( mtr.time + 1.0e-6);

				// save resampling time
				rsmp_time = ssm_particle.time;
			} // <--- resampling

		}

		// close debug log
		if(opt.op_mode.debug && dbgfp){
			::fclose(dbgfp);
		}

	} // <--- operation


	{ // ---> finalize
		::endSSM();

		::fprintf(stderr, "Finish.\x1b[49m\n");
	} // <--- finalize

	return 0;
}



void ShutOff(int stat)
{
	// set shut-off flag
	gShutOff = true;
}



int read_kinematics_config(double* radius_r, double* radius_l, double* tread,
		double* gear, double* count_rev, const char* fname)
{
	FILE* fp;
	char buf[256];
	char s, e;
	char *p;
	int l;

	// ---> initialize
	if((fp = fopen(fname, "r")) == 0){
		return -1;
	}
	// <--- initialize

	while(1){
		::memset(buf, 0, sizeof(buf));
		if(::fgets(buf, sizeof(buf), fp) == 0)	break;

		// check comment out
		for(e = 0; *(buf+e) != '\0' && *(buf+e) != '#' && *(buf+e) != '\r' && *(buf+e) != '\n'; e++ );
		*(buf+e) = '\0';
		// delete space head
		for(s = 0; s < e && ::isspace(*(buf+s)); s++);

		p = 0;
		if(      (l = ::strlen("RADIUS_R"))	&& ::strncmp(buf + s, "RADIUS_R", l) == 0)	*radius_r = ::strtod( buf + s + l, &p);
		else if( (l = ::strlen("RADIUS_L"))	&& ::strncmp(buf + s, "RADIUS_L", l) == 0)	*radius_l = ::strtod( buf + s + l, &p);
		else if( (l = ::strlen("TREAD"))	&& ::strncmp(buf + s, "TREAD", l) == 0)		*tread = ::strtod( buf + s + l, &p);
		else if( (l = ::strlen("GEAR"))		&& ::strncmp(buf + s, "GEAR", l) == 0)		*gear = ::strtod( buf + s + l, &p);
		else if( (l = ::strlen("COUNT_REV"))&& ::strncmp(buf + s, "COUNT_REV", l) == 0)	*count_rev = ::strtod( buf + s + l, &p);

		if(!p){
			// syntax error
		}
	}

	{ // ---> finalize
		fclose(fp);
	} // <--- finalize

	return 0;
}





