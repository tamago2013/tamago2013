/*
 * localizer-conf.hpp
 *
 *  Created on: 2011/10/10
 *      Author: tyamada
 */

#ifndef LOCALIZER_CONF_HPP_
#define LOCALIZER_CONF_HPP_

#include "ssm-particles.hpp"

#include "gnd-configurefile.hpp"
#include "yp-matrix-base.hpp"
#include "yp-lib-error.h"

namespace Localizer {

	/*
	 * @brief configure parameter for number of particles
	 */
	static const gnd_conf::configure_parameter_array<char, 512> ConfIni_KFile = {
			"k-file",
			""
	};

	/*
	 * @brief configure parameter for number of particles
	 */
	static const gnd_conf::configure_parameter<int> ConfIni_Particles = {
			"particles",
			500
	};

	/*
	 * @brief configure parameter remain resampling ratio
	 */
	static const gnd_conf::configure_parameter<double> ConfIni_RemainRatio = {
			"remain-ratio",
			0.35
	};

	/*
	 * @brief configure parameter random error resampling ratio
	 */
	static const gnd_conf::configure_parameter<double> ConfIni_RandomErrorRatio = {
			"random-error-rate",
			0.1
	};

	/*
	 * @brief configure parameter random error covariance
	 */
	static const gnd_conf::configure_parameter_array< double , PARTICLE_POS_DIM > ConfIni_RandomErrorCovar = {
			"random-error-covar",
			{ gnd_mm2dist(100), gnd_mm2dist(100), gnd_deg2ang(5) }
	};

	/*
	 * @brief configure parameter random error covariance
	 */
	static const gnd_conf::configure_parameter_array< double, PARTICLE_POS_DIM > ConfIni_RandomErrorStaticCovar = {
			"random-error-static-covar",
			{ gnd_mm2dist(2.5), gnd_mm2dist(2.5), gnd_deg2ang(0.5) }
	};

	/*
	 * @brief configure parameter kinematics ratio
	 */
	static const gnd_conf::configure_parameter<double> ConfIni_KinematicsRatio = {
			"kinematics-ratio",
			0.3
	};

	/*
	 * @brief configure parameter kinematics covariance
	 */
	static const gnd_conf::configure_parameter_array< double, PARTICLE_PROP_DIM > ConfIni_KinematicsCovar = {
			"kinematics-covar",
			{ 1.0e-5, 1.0e-5, 1.0e-5 }
	};

	/*
	 * @brief configure parameter wide kinematics sampling
	 */
	static const gnd_conf::configure_parameter<double> ConfIni_WideKinematicsRatio = {
			"wide-kinematics-ratio",
			0.2
	};

	/*
	 * @brief configure parameter wide kinematics covariance
	 */
	static const gnd_conf::configure_parameter_array< double, PARTICLE_PROP_DIM > ConfIni_WideKinematicsCovar = {
			"wide-kinematics-covar",
			{ 1.0e-3, 1.0e-3, 1.0e-3 }
	};

	/*
	 * @brief configure parameter reset kinematics sampling ratio
	 */
	static const gnd_conf::configure_parameter<double> ConfIni_ResetKinematicsRatio = {
			"reset-kinematics-ratio",
			0.05
	};

	/*
	 * @brief configure parameter reset kinematics sampling covariance
	 */
	static const gnd_conf::configure_parameter_array< double, PARTICLE_PROP_DIM > ConfIni_ResetKinematicsCovar = {
			"reset-kinematics-covar",
			{ 1.0e-5, 1.0e-5, 1.0e-5 }
	};

	/*
	 * @brief configure parameter random sampling
	 */
	static const gnd_conf::configure_parameter<int> ConfIni_RandomSampling = {
			"random-sampling",
			36
	};




	/*
	 * @brief configure parameter random error covariance
	 */
	static const gnd_conf::configure_parameter_array< double, PARTICLE_POS_DIM > ConfIni_PositionCovar_Ini = {
			"ini-pos-covar",
			{ gnd_m2dist(1.0), gnd_m2dist(1.0), gnd_m2dist(1.0) }
	};

	/*
	 * @brief configure parameter random error covariance
	 */
	static const gnd_conf::configure_parameter_array< double, PARTICLE_PROP_DIM > ConfIni_KinematicsCovar_Ini = {
			"ini-kinematics-covar",
			{ 1.0e-5, 1.0e-5, 1.0e-5 }
	};

	/*
	 * @brief pws ssm id (input)
	 */
	static const gnd_conf::configure_parameter< int > ConfIni_PWSSSM = {
			"pws-ssm-id",
			0
	};

	/*
	 * @brief output ssm id (spur-adjust, particles)
	 */
	static const gnd_conf::configure_parameter< int > ConfIni_SSMID = {
			"ssm-id",
			0
	};



	/*
	 * @brief pws ssm id (input)
	 */
	static const gnd_conf::configure_parameter< bool > ConfIni_Gyro = {
			"gyro",
			0
	};

	/*
	 * @brief gyro voltage
	 */
	static const gnd_conf::configure_parameter< double > ConfIni_GyroVoltage = {
			"gyro-voltage",
			5000
	};

	/*
	 * @brief gyro voltage
	 */
	static const gnd_conf::configure_parameter< int > ConfIni_GyroADBits = {
			"gyro-ad-bits",
			10
	};

	/*
	 * @brief gyro bias
	 */
	static const gnd_conf::configure_parameter< double > ConfIni_GyroBias = {
			"gyro-bias",
			2500
	};

	/*
	 * @brief gyro scale-factor
	 */
	static const gnd_conf::configure_parameter< double > ConfIni_GyroScaleFactor = {
			"gyro-scale-factor",
			1.0 / 1145.9 // rad/sec par mV
	};



	/*
	 * \brief particle localizer configure
	 */
	struct configure_parameters {
		/*
		 * @brief Constructor
		 */
		configure_parameters();

		/*
		 * @brief kinematics parameter file
		 */
		gnd_conf::configure_parameter_array<char, 512>					kfile;

		/*
		 * @brief number of particles
		 */
		gnd_conf::configure_parameter<int>								particles;

		/*
		 * @brief initial position
		 */
		gnd_conf::configure_parameter_array<double, PARTICLE_POS_DIM>	pos_ini;
		/*
		 * @brief initial position variance
		 */
		yp_matrix_fixed<PARTICLE_POS_DIM, PARTICLE_POS_DIM>				pos_ini_covar;

		/*
		 * @brief initial kinematics covariance
		 */
		gnd_conf::configure_parameter_array<double, PARTICLE_PROP_DIM>	knm_ini;
		/*
		 * @brief initial kinematics variance parameter
		 */
		yp_matrix_fixed<PARTICLE_PROP_DIM, PARTICLE_PROP_DIM>			knm_ini_covar;

		/*
		 * @brief remain re-sampling rate
		 */
		gnd_conf::configure_parameter<double>							remain;
		/*
		 * @brief position error re-sampling rate
		 */
		gnd_conf::configure_parameter<double>							poserr;

		/*
		 * @brief position error variance parameter
		 */
		gnd_conf::configure_parameter_array<double, PARTICLE_POS_DIM>	poserr_covar_conf;
		/*
		 * @brief position error covariance
		 */
		yp_matrix_fixed<PARTICLE_POS_DIM, PARTICLE_POS_DIM>				poserr_covar;

		/*
		 * @brief position error static variance parameter
		 */
		gnd_conf::configure_parameter_array<double, PARTICLE_POS_DIM>	poserr_covar_static_conf;
		/*
		 * @brief position error static covariance
		 */
		yp_matrix_fixed<PARTICLE_POS_DIM, PARTICLE_POS_DIM>				poserr_covar_static;

		/*
		 * @brief kinematic re-sampling rate
		 */
		gnd_conf::configure_parameter<double>							knm;

		/*
		 * @brief kinematic variance parameter
		 */
		gnd_conf::configure_parameter_array<double, PARTICLE_PROP_DIM>	knm_covar_conf;
		/*
		 * @brief kinematic covariance
		 */
		yp_matrix_fixed<PARTICLE_PROP_DIM, PARTICLE_PROP_DIM>			knm_covar;

		/*
		 * @brief wide kinematic variance parameter
		 */
		gnd_conf::configure_parameter<double>							wknm;
		/*
		 * @brief wide kinematic variance parameter
		 */
		gnd_conf::configure_parameter_array<double, PARTICLE_PROP_DIM>	wknm_covar_conf;
		/*
		 * @brief wide kinematic covariance
		 */
		yp_matrix_fixed<PARTICLE_PROP_DIM, PARTICLE_PROP_DIM>			wknm_covar;

		/*
		 * @brief reset kinematic variance parameter
		 */
		gnd_conf::configure_parameter<double>							reset_knm;
		/*
		 * @brief reset kinematic variance parameter
		 */
		gnd_conf::configure_parameter_array<double, PARTICLE_PROP_DIM>	reset_knm_covar_conf;
		/*
		 * @brief reset kinematic variance parameter
		 */
		yp_matrix_fixed<PARTICLE_PROP_DIM, PARTICLE_PROP_DIM>			reset_knm_covar;

		/*
		 * @brief number of random sampling particle
		 */
		gnd_conf::configure_parameter<int>								random_sampling;

		/*
		 * @brief pws ssm-id (input)
		 */
		gnd_conf::configure_parameter<int>								pws_id;

		/*
		 * @brief output ssm-id
		 */
		gnd_conf::configure_parameter<int>								ssm_id;

		/*
		 * @brief gyro
		 */
		gnd_conf::configure_parameter<bool>								gyro;

		/*
		 * @brief gyro voltage
		 */
		gnd_conf::configure_parameter<double>							gyro_vol;

		/*
		 * @brief gyro-bits
		 */
		gnd_conf::configure_parameter<int>								gyro_bits;

		/*
		 * @brief gyro-bias
		 */
		gnd_conf::configure_parameter<double>							gyro_bias;


		/*
		 * @brief gyro-scalefactor
		 */
		gnd_conf::configure_parameter<double>							gyro_sf;

};
	typedef struct configure_parameters configure_parameters;


	/**
	 * @brief initialize configure to default parameter
	 */
	int configure_initialize(configure_parameters *conf);
	/**
	 * @brief sampling ratio parameter normalize
	 */
	int configure_sampling_ratio_normalize(configure_parameters *conf);
	/**
	 * @brief kinematics parameter materialize
	 */
	int configure_kinematics_parameter_materialize(configure_parameters *conf, double wr, double wl, double tread);

	/**
	 * @brief analyze configure file
	 */
	int analyze_configure(gnd_configure *fconf, configure_parameters *confp);


	// constructor
	configure_parameters::configure_parameters(){
		configure_initialize(this);
	}


	inline
	int configure_sampling_ratio_normalize(configure_parameters *conf)
	{
		yp_assert(!conf, -1, "invalid null pointer");

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
	int configure_get_covariance(configure_parameters *conf)
	{
		yp_assert(!conf, -1, "invalid null pointer");

		yp_matrix_set_unit( &conf->pos_ini_covar );
		for(size_t i = 0; i < 2; i++)
			yp_matrix_set(&conf->pos_ini_covar, i, i, gnd_square( conf->pos_ini.value[i] ) );
		yp_matrix_set(&conf->pos_ini_covar, 2, 2, gnd_square( gnd_deg2ang( conf->pos_ini.value[2] ) ) );

		yp_matrix_set_unit( &conf->knm_ini_covar );
		for(size_t i = 0; i < 3; i++)
			yp_matrix_set(&conf->knm_ini_covar, i, i, gnd_square( conf->knm_ini.value[i] ) );

		yp_matrix_set_unit( &conf->poserr_covar );
		for(size_t i = 0; i < 2; i++)
			yp_matrix_set(&conf->poserr_covar, i, i, gnd_square( conf->poserr_covar_conf.value[i] ) );
		yp_matrix_set(&conf->poserr_covar, 2, 2, gnd_square( gnd_deg2ang( conf->poserr_covar_conf.value[2] ) ) );

		yp_matrix_set_unit( &conf->poserr_covar_static );
		for(size_t i = 0; i < 2; i++)
			yp_matrix_set(&conf->poserr_covar_static, i, i, gnd_square( conf->poserr_covar_static_conf.value[i] ) );
		yp_matrix_set(&conf->poserr_covar_static, 2, 2, gnd_square( gnd_deg2ang( conf->poserr_covar_static_conf.value[2] ) ) );

		return 0;
	}


	/*!
	 * @brief initialize configure
	 */
	inline
	int configure_initialize(configure_parameters *conf){
		yp_assert(!conf, -1, "invalid null pointer");

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

		configure_sampling_ratio_normalize(conf);
		configure_get_covariance(conf);
		return 0;
	}

	inline
	int configure_kinematics_parameter_materialize(configure_parameters *conf, double wr, double wl, double tread)
	{
		double wheel_mean = (wl + wr) / 2.0;
		double wheel_ratio = wl / wheel_mean;
		double tread_ratio = tread / wheel_mean;

		{ // ---> initlaize kinematics error
			yp_matrix_set_unit(&conf->knm_ini_covar);
			if( !conf->gyro.value ){
				yp_matrix_set(&conf->knm_ini_covar, 0, 0,
						gnd_square( conf->knm_ini.value[0] * gnd_square(wheel_mean) )  );
				yp_matrix_set(&conf->knm_ini_covar, 1, 1,
						gnd_square( conf->knm_ini.value[1] * gnd_square(wheel_ratio) )  );
				yp_matrix_set(&conf->knm_ini_covar, 2, 2,
						gnd_square( conf->knm_ini.value[2] * gnd_square(tread_ratio) )  );
			}
			else {
				yp_matrix_set(&conf->knm_ini_covar, 0, 0,
						gnd_square( conf->knm_ini.value[0] * gnd_square(wheel_mean) )  );
				yp_matrix_set(&conf->knm_ini_covar, 1, 1,
						gnd_square( conf->knm_ini.value[1] * conf->gyro_bias.value )  );
				yp_matrix_set(&conf->knm_ini_covar, 2, 2,
						gnd_square( conf->knm_ini.value[2] * conf->gyro_sf.value )  );
			}
		} // <--- initlaize kinematics error

		{ // ---> kinematic sampling
			yp_matrix_set_unit(&conf->knm_covar);
			if( !conf->gyro.value ){
				yp_matrix_set(&conf->knm_covar, 0, 0,
						gnd_square( conf->knm_covar_conf.value[0] * gnd_square(wheel_mean) )  );
				yp_matrix_set(&conf->knm_covar, 1, 1,
						gnd_square( conf->knm_covar_conf.value[1] * gnd_square(wheel_ratio) )  );
				yp_matrix_set(&conf->knm_covar, 2, 2,
						gnd_square( conf->knm_covar_conf.value[2] * gnd_square(tread_ratio) )  );
			}
			else {
				yp_matrix_set(&conf->knm_covar, 0, 0,
						gnd_square( conf->knm_covar_conf.value[0] * gnd_square(wheel_mean) )  );
				yp_matrix_set(&conf->knm_covar, 1, 1,
						gnd_square( conf->knm_covar_conf.value[1] * conf->gyro_bias.value )  );
				yp_matrix_set(&conf->knm_covar, 2, 2,
						gnd_square( conf->knm_covar_conf.value[2] * conf->gyro_sf.value )  );
			}
		} // <--- kinematic sampling

		{ // ---> kinematic wide sampling
			yp_matrix_set_unit(&conf->wknm_covar);
			if( !conf->gyro.value ){
				yp_matrix_set(&conf->wknm_covar, 0, 0,
						gnd_square( conf->wknm_covar_conf.value[0] * gnd_square(wheel_mean) )  );
				yp_matrix_set(&conf->wknm_covar, 1, 1,
						gnd_square( conf->wknm_covar_conf.value[1] * gnd_square(wheel_ratio) )  );
				yp_matrix_set(&conf->wknm_covar, 2, 2,
						gnd_square( conf->wknm_covar_conf.value[2] * gnd_square(tread_ratio) )  );
			}
			else {
				yp_matrix_set(&conf->wknm_covar, 0, 0,
						gnd_square( conf->wknm_covar_conf.value[0] * gnd_square(wheel_mean) )  );
				yp_matrix_set(&conf->wknm_covar, 1, 1,
						gnd_square( conf->wknm_covar_conf.value[1] * conf->gyro_bias.value )  );
				yp_matrix_set(&conf->wknm_covar, 2, 2,
						gnd_square( conf->wknm_covar_conf.value[2] * conf->gyro_sf.value )  );
			}
		} // <--- kinematic wide sampling

		{ // ---> reset kinematic sampling
			yp_matrix_set_unit(&conf->reset_knm_covar);
			if( !conf->gyro.value ){
				yp_matrix_set(&conf->reset_knm_covar, 0, 0,
						gnd_square( conf->reset_knm_covar_conf.value[0] * gnd_square(wheel_mean) )  );
				yp_matrix_set(&conf->reset_knm_covar, 1, 1,
						gnd_square( conf->reset_knm_covar_conf.value[1] * gnd_square(wheel_ratio) )  );
				yp_matrix_set(&conf->reset_knm_covar, 2, 2,
						gnd_square( conf->reset_knm_covar_conf.value[2] * gnd_square(tread_ratio) )  );
			}
			else {
				yp_matrix_set(&conf->reset_knm_covar, 0, 0,
						gnd_square( conf->reset_knm_covar_conf.value[0] * gnd_square(wheel_mean) )  );
				yp_matrix_set(&conf->reset_knm_covar, 1, 1,
						gnd_square( conf->reset_knm_covar_conf.value[1] * conf->gyro_bias.value )  );
				yp_matrix_set(&conf->reset_knm_covar, 2, 2,
						gnd_square( conf->reset_knm_covar_conf.value[2] * conf->gyro_sf.value )  );
			}
		} // <--- reset kinematic sampling
		return 0;
	}





	// configure file data analyze
	inline
	int get_config_param(gnd_configure *conf, configure_parameters *confp)
	{
		yp_assert(!conf, -1, "invalid null pointer");
		yp_assert(!confp, -1, "invalid null pointer");

		gnd_conf::get_configure_parameter(conf, &confp->kfile);
		gnd_conf::get_configure_parameter(conf, &confp->particles);
		gnd_conf::get_configure_parameter(conf, &confp->pos_ini);
		gnd_conf::get_configure_parameter(conf, &confp->knm_ini);
		gnd_conf::get_configure_parameter(conf, &confp->remain);
		gnd_conf::get_configure_parameter(conf, &confp->poserr);
		gnd_conf::get_configure_parameter(conf, &confp->poserr_covar_conf);
		gnd_conf::get_configure_parameter(conf, &confp->knm);
		gnd_conf::get_configure_parameter(conf, &confp->knm_covar_conf);
		gnd_conf::get_configure_parameter(conf, &confp->wknm);
		gnd_conf::get_configure_parameter(conf, &confp->wknm_covar_conf);
		gnd_conf::get_configure_parameter(conf, &confp->reset_knm);
		gnd_conf::get_configure_parameter(conf, &confp->reset_knm_covar_conf);
		gnd_conf::get_configure_parameter(conf, &confp->random_sampling);
		gnd_conf::get_configure_parameter(conf, &confp->pws_id);
		gnd_conf::get_configure_parameter(conf, &confp->ssm_id);
		gnd_conf::get_configure_parameter(conf, &confp->gyro);
		gnd_conf::get_configure_parameter(conf, &confp->gyro_vol);
		gnd_conf::get_configure_parameter(conf, &confp->gyro_bits);
		gnd_conf::get_configure_parameter(conf, &confp->gyro_bias);
		gnd_conf::get_configure_parameter(conf, &confp->gyro_sf);

		configure_sampling_ratio_normalize(confp);
		configure_get_covariance(confp);
		return 0;
	}


};

#endif /* LOCALIZER_CONF_HPP_ */
