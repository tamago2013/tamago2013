/*
 * observation-probability-position-tracker-conf.hpp
 *
 *  Created on: 2011/12/22
 *      Author: tyamada
 */

#ifndef OBSERVATION_PROBABILITY_POSITION_TRACKER_CONF_HPP_
#define OBSERVATION_PROBABILITY_POSITION_TRACKER_CONF_HPP_

#include <ssmtype/spur-odometry.h>
#include "ssm-laser.hpp"

#include "gnd-observation-probability.hpp"
#include "gnd-configuration.hpp"
#include "yp-lib-error.h"

#ifndef OPSMPosTrack
#define OPSMPosTrack ObservationProbabilityScanMatching::PositionTracker
#endif

#define __OPTIMIZER_NEWTON__		"newton"
#define __OPTIMIZER_QMC__	 		"qmc"
#define __OPTIMIZER_QMC2NEWTON__	"qmc2newton"

namespace ObservationProbabilityScanMatching {
	namespace PositionTracker {
		static const char proc_name[] = "opsm-position-tracker";

		// map-file
		static const gnd::Conf::parameter_array<char, 512> ConfIni_MapDir = {
				"map-dir",
				"",		// map file directory
		};
		// ssm-name
		static const gnd::Conf::parameter_array<char, 512> ConfIni_SSMName = {
				"ssm-name",
				SNAME_ADJUST,
		};
		// ssm-id
		static const gnd::Conf::parameter<int> ConfIni_SSMID = {
				"ssm-id",
				0,		// [s]
		};

		// ssm-name
		static const gnd::Conf::parameter_array<char, 512> ConfIni_OdometrySSMName = {
				"odm-ssm-name",
				SNAME_ODOMETRY,
		};
		// ssm-id
		static const gnd::Conf::parameter<int> ConfIni_OdometrySSMID = {
				"odm-ssm-id",
				0,		// [s]
		};

		// laser scanner ssm-name
		static const gnd::Conf::parameter_array<char, 512> ConfIni_LaserScannerSSMName = {
				"laser-scanner-ssm-name",
				SSM_NAME_SCAN_POINT_2D,
		};
		// laser scanner ssm-id
		static const gnd::Conf::parameter<int> ConfIni_LaserScannerSSMID = {
				"laser-scanner-ssm-id",
				0,
		};
		// decimate threshold
		static const gnd::Conf::parameter<double> ConfIni_Decimate = {
				"decimate",
				gnd_m2dist( 0.08 ),	// [m]
		};

		// cycle
		static const gnd::Conf::parameter<double> ConfIni_Cycle = {
				"cycle",
				gnd_sec2time(0.2),	// [s]
		};

		// rest-cycle
		static const gnd::Conf::parameter<double> ConfIni_RestCycle = {
				"rest-cycle",
				gnd_sec2time(10.0),	// [s]
		};
		// rest-threshold-distance
		static const gnd::Conf::parameter<double> ConfIni_RestDist = {
				"rest-threshold-distance",
				gnd_m2dist(0.05),	// [m]
		};
		// rest-threshold-orientation
		static const gnd::Conf::parameter<double> ConfIni_RestOrient = {
				"rest-threshold-orientation",
				2.5,	// [deg]
		};

		// rest-threshold-orientation
		static const gnd::Conf::parameter<bool> ConfIni_SLAM = {
				"slam",
				false,
		};
		// optimizer

		static const char OptNewton[]		= __OPTIMIZER_NEWTON__;
		static const char OptQMC[]			= __OPTIMIZER_QMC__;
		static const char OptQMC2Newton[]	= __OPTIMIZER_QMC2NEWTON__;
		static const gnd::Conf::parameter_array<char, 512> ConfIni_Optimizer = {
				"optimizer",
				__OPTIMIZER_NEWTON__,		// map file directory
		};
		// converge dist
		static const gnd::Conf::parameter<double> ConfIni_ConvergeDist = {
				"converge-distance",
				gnd_m2dist(0.01),	// [m]
		};
		// converge orient
		static const gnd::Conf::parameter<double> ConfIni_ConvergeOrient = {
				"converge-orient",
				0.5,	// [deg]
		};

		// rest-threshold-orientation
		static const gnd::Conf::parameter<bool> ConfIni_NDT = {
				"ndt",
				false,
		};


		/*
		 * \brief particle localizer configure
		 */
		struct configure_parameters {
			configure_parameters();

			gnd::Conf::parameter_array<char, 512>	mapdir;				///< map data directory
			gnd::Conf::parameter_array<char, 512>	ssmname;			///< output(potision estimate) ssm-name
			gnd::Conf::parameter<int>				ssmid;				///< output(potision estimate) ssm-id
			gnd::Conf::parameter_array<char, 512>	odm_ssmname;		///< odometry position estimation ssm-name
			gnd::Conf::parameter<int>				odm_ssmid;			///< odometry position estimation ssm-id
			gnd::Conf::parameter_array<char, 512>	ls_ssmname;			///< laser scanner data ssm-name
			gnd::Conf::parameter<int>				ls_ssmid;			///< laser scanner data ssm-id
			gnd::Conf::parameter<double>			decimate;			///< laser scanner data decimate parameter [m]
			gnd::Conf::parameter<double>			cycle;				///< operation cycle
			gnd::Conf::parameter<double>			rest_cycle;			///< resting mode cycle
			gnd::Conf::parameter<double>			rest_dist;			///< resting threshold (position distance) [m]
			gnd::Conf::parameter<double>			rest_orient;		///< resting threshold (position orientation) [deg]
			gnd::Conf::parameter<bool>				slam;				///< slam mode flag
			gnd::Conf::parameter_array<char, 512>	optimizer;			///< kind of optimizer
			gnd::Conf::parameter<double>			converge_dist;		///< convergence test threshold (position distance) [m]
			gnd::Conf::parameter<double>			converge_orient;	///< convergence test threshold (position orientation) [deg]
			gnd::Conf::parameter<bool>				ndt;				///< ndt mode
		};
		typedef struct configure_parameters configure_parameters;

		/**
		 * @brief initialize configure to default parameter
		 */
		int configure_initialize(configure_parameters *conf);

		/**
		 * @brief analyze configure file
		 */
		int analyze_configure(gnd::Conf::Configuration *fconf, configure_parameters *confp);

		/**
		 * @brief constructor
		 */
		inline
		configure_parameters::configure_parameters(){
			configure_initialize(this);
		}

		/*!
		 * @brief initialize configure
		 */
		inline
		int configure_initialize(configure_parameters *conf){
			yp_assert(!conf, -1, "invalid null pointer");

			::memcpy(&conf->mapdir,				&ConfIni_MapDir,				sizeof(ConfIni_MapDir) );
			::memcpy(&conf->ssmname,			&ConfIni_SSMName,				sizeof(ConfIni_SSMName) );
			::memcpy(&conf->ssmid,				&ConfIni_SSMID,					sizeof(ConfIni_SSMID) );
			::memcpy(&conf->odm_ssmname,		&ConfIni_OdometrySSMName,		sizeof(ConfIni_OdometrySSMName) );
			::memcpy(&conf->odm_ssmid,			&ConfIni_OdometrySSMID,			sizeof(ConfIni_OdometrySSMID) );
			::memcpy(&conf->ls_ssmname,			&ConfIni_LaserScannerSSMName,	sizeof(ConfIni_LaserScannerSSMName) );
			::memcpy(&conf->ls_ssmid,			&ConfIni_LaserScannerSSMID,		sizeof(ConfIni_LaserScannerSSMID) );
			::memcpy(&conf->decimate,			&ConfIni_Decimate,				sizeof(ConfIni_Decimate) );
			::memcpy(&conf->cycle,				&ConfIni_Cycle,					sizeof(ConfIni_Cycle) );
			::memcpy(&conf->rest_cycle,			&ConfIni_RestCycle,				sizeof(ConfIni_RestCycle) );
			::memcpy(&conf->rest_dist,			&ConfIni_RestDist,				sizeof(ConfIni_RestDist) );
			::memcpy(&conf->rest_orient,		&ConfIni_RestOrient,			sizeof(ConfIni_RestOrient) );
			::memcpy(&conf->slam,				&ConfIni_SLAM,					sizeof(ConfIni_SLAM) );
			::memcpy(&conf->optimizer,			&ConfIni_Optimizer,				sizeof(ConfIni_Optimizer) );
			::memcpy(&conf->converge_dist,		&ConfIni_ConvergeDist,			sizeof(ConfIni_ConvergeDist) );
			::memcpy(&conf->converge_orient,	&ConfIni_ConvergeOrient,		sizeof(ConfIni_ConvergeOrient) );
			::memcpy(&conf->ndt,				&ConfIni_NDT,					sizeof(ConfIni_NDT) );

			return 0;
		}

		/*
		 * @brief analyze
		 */
		inline
		int get_config_param(gnd::Conf::Configuration *conf, configure_parameters *confp)
		{
			yp_assert(!conf, -1, "invalid null pointer");
			yp_assert(!confp, -1, "invalid null pointer");

			gnd::Conf::get_parameter(conf, &confp->mapdir);
			gnd::Conf::get_parameter(conf, &confp->ssmname);
			gnd::Conf::get_parameter(conf, &confp->ssmid);
			gnd::Conf::get_parameter(conf, &confp->odm_ssmname);
			gnd::Conf::get_parameter(conf, &confp->odm_ssmid);
			gnd::Conf::get_parameter(conf, &confp->ls_ssmname);
			gnd::Conf::get_parameter(conf, &confp->ls_ssmid);
			gnd::Conf::get_parameter(conf, &confp->decimate);
			gnd::Conf::get_parameter(conf, &confp->cycle);
			gnd::Conf::get_parameter(conf, &confp->rest_cycle);
			gnd::Conf::get_parameter(conf, &confp->rest_dist);
			gnd::Conf::get_parameter(conf, &confp->rest_orient);
			gnd::Conf::get_parameter(conf, &confp->slam);
			gnd::Conf::get_parameter(conf, &confp->optimizer);
			gnd::Conf::get_parameter(conf, &confp->converge_dist);
			gnd::Conf::get_parameter(conf, &confp->converge_orient);
			gnd::Conf::get_parameter(conf, &confp->ndt);

			confp->rest_orient.value = gnd_deg2rad(confp->rest_orient.value);
			return 0;
		}

		/**
		 * @brief file out  configure file
		 */
		inline
		int write_configuration( const char* fname, configure_parameters *confp ){

			yp_assert(!fname, -1, "invalid null pointer");
			yp_assert(!confp, -1, "invalid null pointer");

			{ // ---> operation
				gnd::Conf::FileStream conf;
				gnd::Conf::set_parameter(&conf, &confp->mapdir);
				gnd::Conf::set_parameter(&conf, &confp->ssmname);
				gnd::Conf::set_parameter(&conf, &confp->ssmid);
				gnd::Conf::set_parameter(&conf, &confp->odm_ssmname);
				gnd::Conf::set_parameter(&conf, &confp->odm_ssmid);
				gnd::Conf::set_parameter(&conf, &confp->ls_ssmname);
				gnd::Conf::set_parameter(&conf, &confp->ls_ssmid);
				gnd::Conf::set_parameter(&conf, &confp->decimate);
				gnd::Conf::set_parameter(&conf, &confp->cycle);
				gnd::Conf::set_parameter(&conf, &confp->rest_cycle);
				gnd::Conf::set_parameter(&conf, &confp->rest_dist);
				gnd::Conf::set_parameter(&conf, &confp->rest_orient);
				gnd::Conf::set_parameter(&conf, &confp->slam);
				gnd::Conf::set_parameter(&conf, &confp->optimizer);
				gnd::Conf::set_parameter(&conf, &confp->converge_dist);
				gnd::Conf::set_parameter(&conf, &confp->converge_orient);
				gnd::Conf::set_parameter(&conf, &confp->ndt);

				return conf.wirte(fname);
			} // <--- operation
		}

	} // namespace PositionTracker
}; // namespace ObservationProbabilityScanMatching

#undef __OPTIMIZER_NEWTON__
#undef __OPTIMIZER_QMC__
#undef __OPTIMIZER_QMC2NEWTON__

#endif /* OBSERVATION_PROBABILITY_POSITION_TRACKER_CONF_HPP_ */
