/*
 * observation-probability-position-tracker-conf.hpp
 *
 *  Created on: 2011/12/22
 *      Author: tyamada
 */

#ifndef OBSERVATION_PROBABILITY_POSITION_TRACKER_CONF_HPP_
#define OBSERVATION_PROBABILITY_POSITION_TRACKER_CONF_HPP_

#include "gnd-observation-probability.hpp"
#include "gnd-configurefile.hpp"
#include "yp-lib-error.h"

#ifndef OPSMPosTrack
#define OPSMPosTrack ObservationProbabilityScanMatching::PositionTracker
#endif

#define __OPTIMIZER_NEWTON__ "newton"
#define __OPTIMIZER_QMC__	 "qmc"

namespace ObservationProbabilityScanMatching {
	namespace PositionTracker {
		// map-file
		static const gnd_conf::configure_parameter_array<char, 512> ConfIni_MapDir = {
				"map-dir",
				"",		// map file directory
		};
		// sokuiki id
		static const gnd_conf::configure_parameter<int> ConfIni_SpurAdjustSSMID = {
				"spur-adjust-ssm-id",
				0,		// [s]
		};
		// sokuiki id
		static const gnd_conf::configure_parameter<int> ConfIni_SokuikiSSMID = {
				"sokuiki-raw-ssm-id",
				1,
		};
		// decimate threshold
		static const gnd_conf::configure_parameter<int> ConfIni_Decimate = {
				"decimate",
				gnd_m2dist( 0.08 ),	// [m]
		};
		// cycle
		static const gnd_conf::configure_parameter<double> ConfIni_Cycle = {
				"cycle",
				gnd_sec2time(0.2),	// [s]
		};
		// rest-cycle
		static const gnd_conf::configure_parameter<double> ConfIni_RestCycle = {
				"rest-cycle",
				gnd_sec2time(3.0),	// [s]
		};
		// rest-threshold-distance
		static const gnd_conf::configure_parameter<double> ConfIni_RestDist = {
				"rest-threshold-distance",
				gnd_m2dist(0.15),	// [m]
		};
		// rest-threshold-orientation
		static const gnd_conf::configure_parameter<double> ConfIni_RestOrient = {
				"rest-threshold-orientation",
				10,	// [deg]
		};

		// rest-threshold-orientation
		static const gnd_conf::configure_parameter<bool> ConfIni_SLAM = {
				"slam",
				false,
		};
		// optimizer

		const char OptNewton[] = __OPTIMIZER_NEWTON__;
		const char OptQMC[] = __OPTIMIZER_QMC__;
		static const gnd_conf::configure_parameter_array<char, 512> ConfIni_Optimizer = {
				"optimizer",
				__OPTIMIZER_NEWTON__,		// map file directory
		};
		// converge dist
		static const gnd_conf::configure_parameter<double> ConfIni_ConvergeDist = {
				"converge-distance",
				gnd_m2dist(0.01),	// [m]
		};
		// converge orient
		static const gnd_conf::configure_parameter<double> ConfIni_ConvergeOrient = {
				"converge-orient",
				0.5,	// [deg]
		};
		// rest-threshold-orientation
		static const gnd_conf::configure_parameter<bool> ConfIni_NDT = {
				"ndt",
				false,
		};


		/*
		 * \brief particle localizer configure
		 */
		struct configure_parameters {
			configure_parameters();

			gnd_conf::configure_parameter_array<char, 512>	mapdir;
			gnd_conf::configure_parameter<int>				spuradjust_ssmid;
			gnd_conf::configure_parameter<int>				sokuiki_ssmid;
			gnd_conf::configure_parameter<double>			decimate;
			gnd_conf::configure_parameter<double>			cycle;
			gnd_conf::configure_parameter<double>			rest_cycle;
			gnd_conf::configure_parameter<double>			rest_dist;
			gnd_conf::configure_parameter<double>			rest_orient;
			gnd_conf::configure_parameter<bool>				slam;
			gnd_conf::configure_parameter_array<char, 512>	optimizer;
			gnd_conf::configure_parameter<double>			converge_dist;
			gnd_conf::configure_parameter<double>			converge_orient;
			gnd_conf::configure_parameter<bool>				ndt;
		};
		typedef struct configure_parameters configure_parameters;

		/**
		 * @brief initialize configure to default parameter
		 */
		int configure_initialize(configure_parameters *conf);

		/**
		 * @brief analyze configure file
		 */
		int analyze_configure(gnd_configure *fconf, configure_parameters *confp);

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

			::memcpy(&conf->mapdir,				&ConfIni_MapDir,			sizeof(ConfIni_MapDir) );
			::memcpy(&conf->sokuiki_ssmid,		&ConfIni_SokuikiSSMID,		sizeof(ConfIni_SokuikiSSMID) );
			::memcpy(&conf->spuradjust_ssmid,	&ConfIni_SpurAdjustSSMID,	sizeof(ConfIni_SpurAdjustSSMID) );
			::memcpy(&conf->decimate,			&ConfIni_Decimate,			sizeof(ConfIni_Decimate) );
			::memcpy(&conf->cycle,				&ConfIni_Cycle,				sizeof(ConfIni_Cycle) );
			::memcpy(&conf->rest_cycle,			&ConfIni_RestCycle,			sizeof(ConfIni_RestCycle) );
			::memcpy(&conf->rest_dist,			&ConfIni_RestDist,			sizeof(ConfIni_RestDist) );
			::memcpy(&conf->rest_orient,		&ConfIni_RestOrient,		sizeof(ConfIni_RestOrient) );
			::memcpy(&conf->slam,				&ConfIni_SLAM,				sizeof(ConfIni_SLAM) );
			::memcpy(&conf->optimizer,			&ConfIni_Optimizer,			sizeof(ConfIni_Optimizer) );
			::memcpy(&conf->converge_dist,		&ConfIni_ConvergeDist,		sizeof(ConfIni_ConvergeDist) );
			::memcpy(&conf->converge_orient,	&ConfIni_ConvergeOrient,	sizeof(ConfIni_ConvergeOrient) );
			::memcpy(&conf->ndt,				&ConfIni_NDT,				sizeof(ConfIni_NDT) );

			return 0;
		}

		/*
		 * @brief analyze
		 */
		inline
		int get_config_param(gnd_configure *conf, configure_parameters *confp)
		{
			yp_assert(!conf, -1, "invalid null pointer");
			yp_assert(!confp, -1, "invalid null pointer");

			gnd_conf::get_configure_parameter(conf, &confp->mapdir);
			gnd_conf::get_configure_parameter(conf, &confp->sokuiki_ssmid);
			gnd_conf::get_configure_parameter(conf, &confp->spuradjust_ssmid);
			gnd_conf::get_configure_parameter(conf, &confp->decimate);
			gnd_conf::get_configure_parameter(conf, &confp->cycle);
			gnd_conf::get_configure_parameter(conf, &confp->rest_cycle);
			gnd_conf::get_configure_parameter(conf, &confp->rest_dist);
			gnd_conf::get_configure_parameter(conf, &confp->rest_orient);
			gnd_conf::get_configure_parameter(conf, &confp->slam);
			gnd_conf::get_configure_parameter(conf, &confp->optimizer);
			gnd_conf::get_configure_parameter(conf, &confp->converge_dist);
			gnd_conf::get_configure_parameter(conf, &confp->converge_orient);
			gnd_conf::get_configure_parameter(conf, &confp->ndt);

			confp->rest_orient.value = gnd_deg2rad(confp->rest_orient.value);
			return 0;
		}

		/**
		 * @brief file out  configure file
		 */
		inline
		int write_configure( const char* fname, configure_parameters *confp ){

			yp_assert(!fname, -1, "invalid null pointer");
			yp_assert(!confp, -1, "invalid null pointer");

			{ // ---> operation
				gnd_configure_filestream conf;
				gnd_conf::set_configure_parameter(&conf, &confp->mapdir);
				gnd_conf::set_configure_parameter(&conf, &confp->sokuiki_ssmid);
				gnd_conf::set_configure_parameter(&conf, &confp->spuradjust_ssmid);
				gnd_conf::set_configure_parameter(&conf, &confp->decimate);
				gnd_conf::set_configure_parameter(&conf, &confp->cycle);
				gnd_conf::set_configure_parameter(&conf, &confp->rest_cycle);
				gnd_conf::set_configure_parameter(&conf, &confp->rest_dist);
				gnd_conf::set_configure_parameter(&conf, &confp->rest_orient);
				gnd_conf::set_configure_parameter(&conf, &confp->slam);
				gnd_conf::set_configure_parameter(&conf, &confp->optimizer);
				gnd_conf::set_configure_parameter(&conf, &confp->converge_dist);
				gnd_conf::set_configure_parameter(&conf, &confp->converge_orient);
				gnd_conf::set_configure_parameter(&conf, &confp->ndt);

				return conf.wirte(fname);
			} // <--- operation
		}

	} // namespace PositionTracker
}; // namespace ObservationProbabilityScanMatching

#undef __OPTIMIZER_NEWTON__
#undef __OPTIMIZER_QMC__

#endif /* OBSERVATION_PROBABILITY_POSITION_TRACKER_CONF_HPP_ */
