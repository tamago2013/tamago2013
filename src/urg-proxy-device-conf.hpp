/*
 * urg-proxy-device-conf.hpp
 *
 *  Created on: 2012/01/28
 *      Author: tyamada
 */

#ifndef URG_PROXY_DEVICE_CONF_HPP_
#define URG_PROXY_DEVICE_CONF_HPP_


#include <time.h>
#include "gnd-matrix-base.hpp"
#include "gnd-configuration.hpp"

// ---> constant value definition
namespace URGProxy {
	// id
	static const gnd::Conf::param_int ConfIni_ID = {
			"id",
			10,
	};
	// time-adjust
	static const gnd::Conf::parameter_array<int, 2> ConfIni_TimeAdjust = {
			"timeadjust",
			{ 1, 8, }
	};
	// reflection
	static const gnd::Conf::param_bool ConfIni_Reflect = {
			"reflect",
			false,
	};
	// scan range
	static const gnd::Conf::parameter_array<double, 2> ConfIni_AngleRange = {
			"angle-range",
			{ -180, 180, },
	};
	// position
	static const gnd::Conf::parameter_array<double, 3> ConfIni_Position = {
			"position",
			{ 0, 0, 0, },
	};
	// orient
	static const gnd::Conf::parameter_array<double, 3> ConfIni_Orient = {
			"orient",
			{ 1.00, 0.00, 0.00, },
	};
	// upside
	static const gnd::Conf::parameter_array<double, 3> ConfIni_Upside = {
			"upside",
			{ 0.00, 0.00, 1.00, },
	};
}
// <--- constant value definition


// ---> type declaration
namespace URGProxy {
	struct device_configuration;
	typedef struct device_configuration configuration;
}
// <--- type declaration


// ---> function declaration
namespace URGProxy {
	/**
	 * @brief initialize configure to default parameter
	 */
	int initialize_configuration(device_configuration *conf);

	/**
	 * @brief analyze configure file
	 */
	int get_configuration_parameter(gnd::Conf::Configuration *fconf, device_configuration *confp);

	/**
	 * @brief file out  configure file
	 */
	int write_configuration_parameter( const char* fname, device_configuration *confp );
}
// <--- function declaration





// ---> type definition
namespace URGProxy {
	/*
	 * @parameter device configuration
	 */
	struct device_configuration {
		gnd::Conf::param_int					id;
		gnd::Conf::param_bool					reflect;
		gnd::Conf::parameter_array<int, 2>		timeadjust;
		gnd::Conf::parameter_array<double, 2>	angle_range;
		gnd::Conf::parameter_array<double, 3>	position;
		gnd::Conf::parameter_array<double, 3>	orient;
		gnd::Conf::parameter_array<double, 3>	upside;
		device_configuration();
	};

	device_configuration::device_configuration() {
		initialize_configuration(this);
	}
}
// <--- type definition


// ---> function definition
namespace URGProxy {

	/*!
	 * @brief initialize configure
	 */
	inline
	int initialize_configuration(device_configuration *conf){
		gnd_assert(!conf, -1, "invalid null pointer");

		::memcpy(&conf->id,				&ConfIni_ID,			sizeof(ConfIni_ID) );
		::memcpy(&conf->timeadjust,		&ConfIni_TimeAdjust,	sizeof(ConfIni_TimeAdjust) );
		::memcpy(&conf->reflect,		&ConfIni_Reflect,		sizeof(ConfIni_Reflect) );
		::memcpy(&conf->angle_range,	&ConfIni_AngleRange,	sizeof(ConfIni_AngleRange) );
		::memcpy(&conf->position,		&ConfIni_Position,		sizeof(ConfIni_Position) );
		::memcpy(&conf->orient,			&ConfIni_Orient,		sizeof(ConfIni_Orient) );
		::memcpy(&conf->upside,			&ConfIni_Upside,		sizeof(ConfIni_Upside) );
		return 0;
	}

	/*
	 * @brief analyze
	 */
	inline
	int get_configuration_parameter(gnd::Conf::Configuration *conf, device_configuration *confp)
	{
		gnd_assert(!conf, -1, "invalid null pointer");
		gnd_assert(!confp, -1, "invalid null pointer");

		gnd::Conf::get_parameter(conf, &confp->id);
		gnd::Conf::get_parameter(conf, &confp->timeadjust);
		gnd::Conf::get_parameter(conf, &confp->reflect);
		gnd::Conf::get_parameter(conf, &confp->angle_range);
		gnd::Conf::get_parameter(conf, &confp->position);
		gnd::Conf::get_parameter(conf, &confp->orient);
		gnd::Conf::get_parameter(conf, &confp->upside);
		return 0;
	}

	/**
	 * @brief file out  configure file
	 */
	inline
	int write_configuration_parameter( const char* fname, device_configuration *confp ){

		gnd_assert(!fname, -1, "invalid null pointer");
		gnd_assert(!confp, -1, "invalid null pointer");

		{ // ---> operation
			gnd::Conf::FileStream conf;
			gnd::Conf::set_parameter(&conf, &confp->id);
			gnd::Conf::set_parameter(&conf, &confp->timeadjust);
			gnd::Conf::set_parameter(&conf, &confp->reflect);
			gnd::Conf::set_parameter(&conf, &confp->angle_range);
			gnd::Conf::set_parameter(&conf, &confp->position);
			gnd::Conf::set_parameter(&conf, &confp->orient);
			gnd::Conf::set_parameter(&conf, &confp->upside);

			return conf.write(fname);
		} // <--- operation
	}

}
// <--- function definition

#endif /* URG_PROXY_DEVICE_CONF_HPP_ */
