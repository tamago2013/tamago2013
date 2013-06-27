/*
 * particle-localizer-conf.hpp
 *
 *  Created on: 2011/10/10
 *      Author: tyamada
 */

#ifndef PARTICLE_LOCALIZER_CONF_HPP_
#define PARTICLE_LOCALIZER_CONF_HPP_

#include "ssm-particles.hpp"

#include "gnd-config-file.hpp"
#include "gnd-matrix-base.hpp"
#include "gnd-lib-error.h"

namespace Localizer {

	/*
	 * @brief configure parameter for number of particles
	 */
	static const gnd::conf::parameter_array<char, 512> ConfIni_KFile = {
			"k-param",
			"",
			"kinematics parameter file path"
	};

	/*
	 * @brief configure parameter for number of particles
	 */
	static const gnd::conf::parameter<int> ConfIni_Particles = {
			"particles",
			500,
			"nunber of particles"
	};

	/*
	 * @brief configure parameter remain resampling ratio
	 */
	static const gnd::conf::parameter<double> ConfIni_RemainRatio = {
			"remain-ratio",
			0.35,
			"re-sampling parameter"
	};

	/*
	 * @brief configure parameter random error resampling ratio
	 */
	static const gnd::conf::parameter<double> ConfIni_RandomErrorRatio = {
			"random-error-rate",
			0.1,
			"re-sampling parameter "
	};

	/*
	 * @brief configure parameter random error covariance
	 */
	static const gnd::conf::parameter_array< double , PARTICLE_POS_DIM > ConfIni_RandomErrorCovar = {
			"random-error-covar",
			{ gnd_mm2dist(100), gnd_mm2dist(100), gnd_deg2ang(5) }
	};

	/*
	 * @brief configure parameter random error covariance
	 */
	static const gnd::conf::parameter_array< double, PARTICLE_POS_DIM > ConfIni_RandomErrorStaticCovar = {
			"random-error-static-covar",
			{ gnd_mm2dist(2.5), gnd_mm2dist(2.5), gnd_deg2ang(0.5) }
	};

	/*
	 * @brief configure parameter kinematics ratio
	 */
	static const gnd::conf::parameter<double> ConfIni_KinematicsRatio = {
			"kinematics-ratio",
			0.3
	};

	/*
	 * @brief configure parameter kinematics covariance
	 */
	static const gnd::conf::parameter_array< double, PARTICLE_PROP_DIM > ConfIni_KinematicsCovar = {
			"kinematics-covar",
			{ 1.0e-5, 1.0e-5, 1.0e-5 }
	};

	/*
	 * @brief configure parameter wide kinematics sampling
	 */
	static const gnd::conf::parameter<double> ConfIni_WideKinematicsRatio = {
			"wide-kinematics-ratio",
			0.2
	};

	/*
	 * @brief configure parameter wide kinematics covariance
	 */
	static const gnd::conf::parameter_array< double, PARTICLE_PROP_DIM > ConfIni_WideKinematicsCovar = {
			"wide-kinematics-covar",
			{ 1.0e-3, 1.0e-3, 1.0e-3 }
	};

	/*
	 * @brief configure parameter reset kinematics sampling ratio
	 */
	static const gnd::conf::parameter<double> ConfIni_ResetKinematicsRatio = {
			"reset-kinematics-ratio",
			0.05
	};

	/*
	 * @brief configure parameter reset kinematics sampling covariance
	 */
	static const gnd::conf::parameter_array< double, PARTICLE_PROP_DIM > ConfIni_ResetKinematicsCovar = {
			"reset-kinematics-covar",
			{ 1.0e-5, 1.0e-5, 1.0e-5 }
	};

	/*
	 * @brief configure parameter random sampling
	 */
	static const gnd::conf::parameter<int> ConfIni_RandomSampling = {
			"random-sampling",
			36
	};




	/*
	 * @brief configure parameter random error covariance
	 */
	static const gnd::conf::parameter_array< double, PARTICLE_POS_DIM > ConfIni_PositionCovar_Ini = {
			"ini-pos-covar",
			{ gnd_m2dist(1.0), gnd_m2dist(1.0), gnd_m2dist(1.0) }
	};

	/*
	 * @brief configure parameter random error covariance
	 */
	static const gnd::conf::parameter_array< double, PARTICLE_PROP_DIM > ConfIni_KinematicsCovar_Ini = {
			"ini-kinematics-covar",
			{ 1.0e-5, 1.0e-5, 1.0e-5 }
	};

	/*
	 * @brief pws ssm id (input)
	 */
	static const gnd::conf::parameter< int > ConfIni_PWSSSM = {
			"pws-ssm-id",
			0
	};

	/*
	 * @brief output ssm id (spur-adjust, particles)
	 */
	static const gnd::conf::parameter< int > ConfIni_SSMID = {
			"ssm-id",
			0
	};



	/*
	 * @brief pws ssm id (input)
	 */
	static const gnd::conf::parameter< bool > ConfIni_Gyro = {
			"gyro",
			0
	};

	/*
	 * @brief gyro voltage
	 */
	static const gnd::conf::parameter< double > ConfIni_GyroVoltage = {
			"gyro-voltage",
			5000
	};

	/*
	 * @brief gyro voltage
	 */
	static const gnd::conf::parameter< int > ConfIni_GyroADBits = {
			"gyro-ad-bits",
			10
	};

	/*
	 * @brief gyro bias
	 */
	static const gnd::conf::parameter< double > ConfIni_GyroBias = {
			"gyro-bias",
			2500
	};

	/*
	 * @brief gyro scale-factor
	 */
	static const gnd::conf::parameter< double > ConfIni_GyroScaleFactor = {
			"gyro-scale-factor",
			1.0 / 1145.9 // rad/sec par mV
	};



	/*
	 * \brief particle localizer configure
	 */
	struct proc_configuration {
		/*
		 * @brief Constructor
		 */
		proc_configuration();

		/*
		 * @brief kinematics parameter file
		 */
		gnd::conf::parameter_array<char, 512>						kfile;

		/*
		 * @brief number of particles
		 */
		gnd::conf::parameter<int>									particles;

		/*
		 * @brief initial position
		 */
		gnd::conf::parameter_array<double, PARTICLE_POS_DIM>		pos_ini;
		/*
		 * @brief initial position variance
		 */
		gnd::matrix::fixed<PARTICLE_POS_DIM, PARTICLE_POS_DIM>		pos_ini_covar;

		/*
		 * @brief initial kinematics covariance
		 */
		gnd::conf::parameter_array<double, PARTICLE_PROP_DIM>		knm_ini;
		/*
		 * @brief initial kinematics variance parameter
		 */
		gnd::matrix::fixed<PARTICLE_PROP_DIM, PARTICLE_PROP_DIM>	knm_ini_covar;

		/*
		 * @brief remain re-sampling rate
		 */
		gnd::conf::parameter<double>								remain;
		/*
		 * @brief position error re-sampling rate
		 */
		gnd::conf::parameter<double>								poserr;

		/*
		 * @brief position error variance parameter
		 */
		gnd::conf::parameter_array<double, PARTICLE_POS_DIM>		poserr_covar_conf;
		/*
		 * @brief position error covariance
		 */
		gnd::matrix::fixed<PARTICLE_POS_DIM, PARTICLE_POS_DIM>		poserr_covar;

		/*
		 * @brief position error static variance parameter
		 */
		gnd::conf::parameter_array<double, PARTICLE_POS_DIM>		poserr_covar_static_conf;
		/*
		 * @brief position error static covariance
		 */
		gnd::matrix::fixed<PARTICLE_POS_DIM, PARTICLE_POS_DIM>		poserr_covar_static;

		/*
		 * @brief kinematic re-sampling rate
		 */
		gnd::conf::parameter<double>								knm;

		/*
		 * @brief kinematic variance parameter
		 */
		gnd::conf::parameter_array<double, PARTICLE_PROP_DIM>		knm_covar_conf;
		/*
		 * @brief kinematic covariance
		 */
		gnd::matrix::fixed<PARTICLE_PROP_DIM, PARTICLE_PROP_DIM>	knm_covar;

		/*
		 * @brief wide kinematic variance parameter
		 */
		gnd::conf::parameter<double>								wknm;
		/*
		 * @brief wide kinematic variance parameter
		 */
		gnd::conf::parameter_array<double, PARTICLE_PROP_DIM>		wknm_covar_conf;
		/*
		 * @brief wide kinematic covariance
		 */
		gnd::matrix::fixed<PARTICLE_PROP_DIM, PARTICLE_PROP_DIM>	wknm_covar;

		/*
		 * @brief reset kinematic variance parameter
		 */
		gnd::conf::parameter<double>								reset_knm;
		/*
		 * @brief reset kinematic variance parameter
		 */
		gnd::conf::parameter_array<double, PARTICLE_PROP_DIM>		reset_knm_covar_conf;
		/*
		 * @brief reset kinematic variance parameter
		 */
		gnd::matrix::fixed<PARTICLE_PROP_DIM, PARTICLE_PROP_DIM>	reset_knm_covar;

		/*
		 * @brief number of random sampling particle
		 */
		gnd::conf::parameter<int>									random_sampling;

		/*
		 * @brief pws ssm-id (input)
		 */
		gnd::conf::parameter<int>									pws_id;

		/*
		 * @brief output ssm-id
		 */
		gnd::conf::parameter<int>									ssm_id;

		/*
		 * @brief gyro
		 */
		gnd::conf::parameter<bool>									gyro;

		/*
		 * @brief gyro voltage
		 */
		gnd::conf::parameter<double>								gyro_vol;

		/*
		 * @brief gyro-bits
		 */
		gnd::conf::parameter<int>									gyro_bits;

		/*
		 * @brief gyro-bias
		 */
		gnd::conf::parameter<double>								gyro_bias;


		/*
		 * @brief gyro-scalefactor
		 */
		gnd::conf::parameter<double>								gyro_sf;

	};
	typedef struct proc_configuration configure_parameters;


	/**
	 * @brief initialize configure to default parameter
	 */
	int proc_conf_initialize(proc_configuration *conf);
	/**
	 * @brief sampling ratio parameter normalize
	 */
	int proc_conf_sampling_ratio_normalize(proc_configuration *conf);
	/**
	 * @brief kinematics parameter materialize
	 */
	int proc_conf_set_kinematics_parameter(proc_configuration *conf, double wr, double wl, double tread);

	/**
	 * @brief analyze configure file
	 */
	int proc_conf_analyze(gnd::conf::configuration *fconf, proc_configuration *confp);


	// constructor
	proc_configuration::proc_configuration(){
		proc_conf_initialize(this);
	}


	inline
	int proc_conf_sampling_ratio_normalize(proc_configuration *conf)
	{
		gnd_assert(!conf, -1, "invalid null pointer");

		{
			double sum = 0;

			sum += conf->remain.value;
			sum += conf->poserr.value;
			sum += conf->knm.value;
			sum += conf->wknm.value;
			sum += conf->reset_knm.value;

			if( sum <= 0 ){
				return -1;
			}
			else {
				conf->remain.value /= sum;
				conf->poserr.value /= sum;
				conf->knm.value /= sum;
				conf->wknm.value /= sum;
				conf->reset_knm.value /= sum;
			}
		}
		return 0;
	}



	inline
	int configure_get_covariance(proc_configuration *conf)
	{
		gnd_assert(!conf, -1, "invalid null pointer");

		gnd::matrix::set_unit( &conf->pos_ini_covar );
		for(size_t i = 0; i < 2; i++)
			gnd::matrix::set(&conf->pos_ini_covar, i, i, gnd_square( conf->pos_ini.value[i] ) );
		gnd::matrix::set(&conf->pos_ini_covar, 2, 2, gnd_square( gnd_deg2ang( conf->pos_ini.value[2] ) ) );

		gnd::matrix::set_unit( &conf->knm_ini_covar );
		for(size_t i = 0; i < 3; i++)
			gnd::matrix::set(&conf->knm_ini_covar, i, i, gnd_square( conf->knm_ini.value[i] ) );

		gnd::matrix::set_unit( &conf->poserr_covar );
		for(size_t i = 0; i < 2; i++)
			gnd::matrix::set(&conf->poserr_covar, i, i, gnd_square( conf->poserr_covar_conf.value[i] ) );
		gnd::matrix::set(&conf->poserr_covar, 2, 2, gnd_square( gnd_deg2ang( conf->poserr_covar_conf.value[2] ) ) );

		gnd::matrix::set_unit( &conf->poserr_covar_static );
		for(size_t i = 0; i < 2; i++)
			gnd::matrix::set(&conf->poserr_covar_static, i, i, gnd_square( conf->poserr_covar_static_conf.value[i] ) );
		gnd::matrix::set(&conf->poserr_covar_static, 2, 2, gnd_square( gnd_deg2ang( conf->poserr_covar_static_conf.value[2] ) ) );

		return 0;
	}


	/*!
	 * @brief initialize configure
	 */
	inline
	int proc_conf_initialize(proc_configuration *conf){
		gnd_assert(!conf, -1, "invalid null pointer");

		::memcpy(&conf->kfile,						&ConfIni_KFile,						sizeof(ConfIni_KFile));

		::memcpy(&conf->particles,					&ConfIni_Particles,					sizeof(ConfIni_Particles));
		::memcpy(&conf->remain,						&ConfIni_RemainRatio,				sizeof(ConfIni_RemainRatio));

		::memcpy(&conf->pos_ini,					&ConfIni_PositionCovar_Ini,			sizeof(ConfIni_PositionCovar_Ini));
		::memcpy(&conf->knm_ini,					&ConfIni_KinematicsCovar_Ini,		sizeof(ConfIni_KinematicsCovar_Ini));

		::memcpy(&conf->poserr_covar_conf,			&ConfIni_RandomErrorCovar,			sizeof(ConfIni_RandomErrorCovar));
		::memcpy(&conf->poserr,						&ConfIni_RandomErrorRatio,			sizeof(ConfIni_RandomErrorRatio));
		::memcpy(&conf->poserr_covar_static_conf,	&ConfIni_RandomErrorStaticCovar,	sizeof(ConfIni_RandomErrorStaticCovar));
		::memcpy(&conf->knm,						&ConfIni_KinematicsRatio,			sizeof(ConfIni_KinematicsRatio));
		::memcpy(&conf->knm_covar_conf,				&ConfIni_KinematicsCovar,			sizeof(ConfIni_KinematicsCovar));
		::memcpy(&conf->wknm,						&ConfIni_WideKinematicsRatio,		sizeof(ConfIni_WideKinematicsRatio));
		::memcpy(&conf->wknm_covar_conf,			&ConfIni_WideKinematicsCovar,		sizeof(ConfIni_WideKinematicsCovar));
		::memcpy(&conf->reset_knm,					&ConfIni_ResetKinematicsRatio,		sizeof(ConfIni_ResetKinematicsRatio));
		::memcpy(&conf->reset_knm_covar_conf,		&ConfIni_ResetKinematicsCovar,		sizeof(ConfIni_ResetKinematicsCovar));

		::memcpy(&conf->random_sampling,			&ConfIni_RandomSampling,			sizeof(ConfIni_RandomSampling));

		::memcpy(&conf->pws_id,						&ConfIni_PWSSSM,					sizeof(ConfIni_PWSSSM));
		::memcpy(&conf->ssm_id,						&ConfIni_SSMID,						sizeof(ConfIni_SSMID));

		::memcpy(&conf->gyro,						&ConfIni_Gyro,						sizeof(ConfIni_Gyro));
		::memcpy(&conf->gyro_vol,					&ConfIni_GyroVoltage,				sizeof(ConfIni_GyroVoltage));
		::memcpy(&conf->gyro_bits,					&ConfIni_GyroADBits,				sizeof(ConfIni_GyroADBits));
		::memcpy(&conf->gyro_bias,					&ConfIni_GyroBias,					sizeof(ConfIni_GyroBias));
		::memcpy(&conf->gyro_sf,					&ConfIni_GyroScaleFactor,			sizeof(ConfIni_GyroScaleFactor));

		proc_conf_sampling_ratio_normalize(conf);
		configure_get_covariance(conf);
		return 0;
	}

	inline
	int proc_conf_set_kinematics_parameter(proc_configuration *conf, double wr, double wl, double tread)
	{
		double wheel_mean = (wl + wr) / 2.0;
		double wheel_ratio = wl / wheel_mean;
		double tread_ratio = tread / wheel_mean;

		{ // ---> initlaize kinematics error
			gnd::matrix::set_unit(&conf->knm_ini_covar);
			if( !conf->gyro.value ){
				gnd::matrix::set(&conf->knm_ini_covar, 0, 0,
						gnd_square( conf->knm_ini.value[0] * gnd_square(wheel_mean) )  );
				gnd::matrix::set(&conf->knm_ini_covar, 1, 1,
						gnd_square( conf->knm_ini.value[1] * gnd_square(wheel_ratio) )  );
				gnd::matrix::set(&conf->knm_ini_covar, 2, 2,
						gnd_square( conf->knm_ini.value[2] * gnd_square(tread_ratio) )  );
			}
			else {
				gnd::matrix::set(&conf->knm_ini_covar, 0, 0,
						gnd_square( conf->knm_ini.value[0] * gnd_square(wheel_mean) )  );
				gnd::matrix::set(&conf->knm_ini_covar, 1, 1,
						gnd_square( conf->knm_ini.value[1] * conf->gyro_bias.value )  );
				gnd::matrix::set(&conf->knm_ini_covar, 2, 2,
						gnd_square( conf->knm_ini.value[2] * conf->gyro_sf.value )  );
			}
		} // <--- initlaize kinematics error

		{ // ---> kinematic sampling
			gnd::matrix::set_unit(&conf->knm_covar);
			if( !conf->gyro.value ){
				gnd::matrix::set(&conf->knm_covar, 0, 0,
						gnd_square( conf->knm_covar_conf.value[0] * gnd_square(wheel_mean) )  );
				gnd::matrix::set(&conf->knm_covar, 1, 1,
						gnd_square( conf->knm_covar_conf.value[1] * gnd_square(wheel_ratio) )  );
				gnd::matrix::set(&conf->knm_covar, 2, 2,
						gnd_square( conf->knm_covar_conf.value[2] * gnd_square(tread_ratio) )  );
			}
			else {
				gnd::matrix::set(&conf->knm_covar, 0, 0,
						gnd_square( conf->knm_covar_conf.value[0] * gnd_square(wheel_mean) )  );
				gnd::matrix::set(&conf->knm_covar, 1, 1,
						gnd_square( conf->knm_covar_conf.value[1] * conf->gyro_bias.value )  );
				gnd::matrix::set(&conf->knm_covar, 2, 2,
						gnd_square( conf->knm_covar_conf.value[2] * conf->gyro_sf.value )  );
			}
		} // <--- kinematic sampling

		{ // ---> kinematic wide sampling
			gnd::matrix::set_unit(&conf->wknm_covar);
			if( !conf->gyro.value ){
				gnd::matrix::set(&conf->wknm_covar, 0, 0,
						gnd_square( conf->wknm_covar_conf.value[0] * gnd_square(wheel_mean) )  );
				gnd::matrix::set(&conf->wknm_covar, 1, 1,
						gnd_square( conf->wknm_covar_conf.value[1] * gnd_square(wheel_ratio) )  );
				gnd::matrix::set(&conf->wknm_covar, 2, 2,
						gnd_square( conf->wknm_covar_conf.value[2] * gnd_square(tread_ratio) )  );
			}
			else {
				gnd::matrix::set(&conf->wknm_covar, 0, 0,
						gnd_square( conf->wknm_covar_conf.value[0] * gnd_square(wheel_mean) )  );
				gnd::matrix::set(&conf->wknm_covar, 1, 1,
						gnd_square( conf->wknm_covar_conf.value[1] * conf->gyro_bias.value )  );
				gnd::matrix::set(&conf->wknm_covar, 2, 2,
						gnd_square( conf->wknm_covar_conf.value[2] * conf->gyro_sf.value )  );
			}
		} // <--- kinematic wide sampling

		{ // ---> reset kinematic sampling
			gnd::matrix::set_unit(&conf->reset_knm_covar);
			if( !conf->gyro.value ){
				gnd::matrix::set(&conf->reset_knm_covar, 0, 0,
						gnd_square( conf->reset_knm_covar_conf.value[0] * gnd_square(wheel_mean) )  );
				gnd::matrix::set(&conf->reset_knm_covar, 1, 1,
						gnd_square( conf->reset_knm_covar_conf.value[1] * gnd_square(wheel_ratio) )  );
				gnd::matrix::set(&conf->reset_knm_covar, 2, 2,
						gnd_square( conf->reset_knm_covar_conf.value[2] * gnd_square(tread_ratio) )  );
			}
			else {
				gnd::matrix::set(&conf->reset_knm_covar, 0, 0,
						gnd_square( conf->reset_knm_covar_conf.value[0] * gnd_square(wheel_mean) )  );
				gnd::matrix::set(&conf->reset_knm_covar, 1, 1,
						gnd_square( conf->reset_knm_covar_conf.value[1] * conf->gyro_bias.value )  );
				gnd::matrix::set(&conf->reset_knm_covar, 2, 2,
						gnd_square( conf->reset_knm_covar_conf.value[2] * conf->gyro_sf.value )  );
			}
		} // <--- reset kinematic sampling
		return 0;
	}





	// configure file data analyze
	inline
	int proc_conf_get(gnd::conf::configuration *src, proc_configuration* dest) {
		gnd_assert(!src, -1, "invalid null pointer");
		gnd_assert(!dest, -1, "invalid null pointer");

		gnd::conf::get_parameter(src, &dest->kfile);
		gnd::conf::get_parameter(src, &dest->particles);
		gnd::conf::get_parameter(src, &dest->pos_ini);
		gnd::conf::get_parameter(src, &dest->knm_ini);
		gnd::conf::get_parameter(src, &dest->remain);
		gnd::conf::get_parameter(src, &dest->poserr);
		gnd::conf::get_parameter(src, &dest->poserr_covar_conf);
		gnd::conf::get_parameter(src, &dest->knm);
		gnd::conf::get_parameter(src, &dest->knm_covar_conf);
		gnd::conf::get_parameter(src, &dest->wknm);
		gnd::conf::get_parameter(src, &dest->wknm_covar_conf);
		gnd::conf::get_parameter(src, &dest->reset_knm);
		gnd::conf::get_parameter(src, &dest->reset_knm_covar_conf);
		gnd::conf::get_parameter(src, &dest->random_sampling);
		gnd::conf::get_parameter(src, &dest->pws_id);
		gnd::conf::get_parameter(src, &dest->ssm_id);
		gnd::conf::get_parameter(src, &dest->gyro);
		gnd::conf::get_parameter(src, &dest->gyro_vol);
		gnd::conf::get_parameter(src, &dest->gyro_bits);
		gnd::conf::get_parameter(src, &dest->gyro_bias);
		gnd::conf::get_parameter(src, &dest->gyro_sf);

		proc_conf_sampling_ratio_normalize(dest);
		configure_get_covariance(dest);
		return 0;
	}


	// configure file data analyze
	inline
	int proc_conf_set(gnd::conf::configuration *dest, proc_configuration* src) {
		gnd_assert(!src, -1, "invalid null pointer");
		gnd_assert(!dest, -1, "invalid null pointer");

		gnd::conf::set_parameter(dest, &src->kfile);
		gnd::conf::set_parameter(dest, &src->particles);
		gnd::conf::set_parameter(dest, &src->pos_ini);
		gnd::conf::set_parameter(dest, &src->knm_ini);
		gnd::conf::set_parameter(dest, &src->remain);
		gnd::conf::set_parameter(dest, &src->poserr);
		gnd::conf::set_parameter(dest, &src->poserr_covar_conf);
		gnd::conf::set_parameter(dest, &src->knm);
		gnd::conf::set_parameter(dest, &src->knm_covar_conf);
		gnd::conf::set_parameter(dest, &src->wknm);
		gnd::conf::set_parameter(dest, &src->wknm_covar_conf);
		gnd::conf::set_parameter(dest, &src->reset_knm);
		gnd::conf::set_parameter(dest, &src->reset_knm_covar_conf);
		gnd::conf::set_parameter(dest, &src->random_sampling);
		gnd::conf::set_parameter(dest, &src->pws_id);
		gnd::conf::set_parameter(dest, &src->ssm_id);
		gnd::conf::set_parameter(dest, &src->gyro);
		gnd::conf::set_parameter(dest, &src->gyro_vol);
		gnd::conf::set_parameter(dest, &src->gyro_bits);
		gnd::conf::set_parameter(dest, &src->gyro_bias);
		gnd::conf::set_parameter(dest, &src->gyro_sf);

		return 0;
	}


	/**
	 * @brief read configuration parameter file
	 * @param [in]  f    : configuration file name
	 * @param [out] dest : configuration parameter
	 */
	inline
	int proc_conf_read(const char* f, proc_configuration* dest) {
		gnd_assert(!f, -1, "invalid null pointer");
		gnd_assert(!dest, -1, "invalid null pointer");

		{ // ---> operation
			int ret;
			gnd::conf::file_stream fs;
			// configuration file read
			if( (ret = fs.read(f)) < 0 )	return ret;

			return proc_conf_get(&fs, dest);
		} // <--- operation
	}

	/**
	 * @brief write configuration parameter file
	 * @param [in]  f  : configuration file name
	 * @param [in] src : configuration parameter
	 */
	inline
	int proc_conf_write(const char* f, proc_configuration* src){
		gnd_assert(!f, -1, "invalid null pointer");
		gnd_assert(!src, -1, "invalid null pointer");

		{ // ---> operation
			int ret;
			gnd::conf::file_stream fs;
			// convert configuration declaration
			if( (ret = proc_conf_set(&fs, src)) < 0 ) return ret;

			return fs.write(f);
		} // <--- operation
	}
};

#endif /* PARTICLE_LOCALIZER_CONF_HPP_ */
