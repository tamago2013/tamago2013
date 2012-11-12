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
#include "gnd-lib-error.h"

// ---> constant value definition
namespace gnd {
	namespace urg_proxy {

		const char Token_Serial[] = "<Serial>";
		// id
		static const gnd::Conf::param_int ConfIni_ID = {
				"id",
				10,
		};
		// time-adjust
		static const gnd::Conf::parameter_array<int, 2> ConfIni_TimeAdjust = {
				"timeadjust",
				{ 1, 8, },
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
}
// <--- constant value definition


// ---> type declaration
namespace gnd {
	namespace urg_proxy {
		struct device_configuration;
		typedef struct device_configuration device_configuration;
	}
}
// <--- type declaration


// ---> function declaration
namespace gnd {
	namespace urg_proxy {
		/**
		 * @brief initialize configure to default parameter
		 */
		int dev_conf_init(device_configuration *conf);

		/**
		 * @brief analyze configure file
		 */
		int dev_conf_get(gnd::Conf::Configuration *fconf, device_configuration *dest);

		/**
		 * @brief file out  configure file
		 */
		int dev_conf_write( const char* fname, device_configuration *confp );
	}
}
// <--- function declaration





// ---> type definition
namespace gnd {
	namespace urg_proxy {
		/*
		 * @parameter device configuration
		 */
		struct device_configuration {
			char									serial[64];
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
			dev_conf_init(this);
		}
	}
}
// <--- type definition


// ---> function definition
namespace gnd {
	namespace urg_proxy {

		/*!
		 * @brief initialize configure
		 */
		inline
		int dev_conf_init(device_configuration *conf){
			gnd_assert(!conf, -1, "invalid null pointer");

			::memset(conf->serial,			0,						sizeof(conf->serial));
			::strcpy(conf->serial,			Token_Serial);

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
		int dev_conf_get(gnd::Conf::Configuration *src, device_configuration *dest)
		{
			gnd_assert(!src, -1, "invalid null pointer");
			gnd_assert(!dest, -1, "invalid null pointer");

			gnd::Conf::get_parameter(src, &dest->id);
			gnd::Conf::get_parameter(src, &dest->timeadjust);
			gnd::Conf::get_parameter(src, &dest->reflect);
			gnd::Conf::get_parameter(src, &dest->angle_range);
			gnd::Conf::get_parameter(src, &dest->position);
			gnd::Conf::get_parameter(src, &dest->orient);
			gnd::Conf::get_parameter(src, &dest->upside);
			return 0;
		}

		/*
		 * @brief analyze
		 */
		inline
		int dev_conf_set(gnd::Conf::Configuration *dest, device_configuration *src)
		{
			gnd::Conf::Configuration *ch;
			gnd_assert(!dest, -1, "invalid null pointer");
			gnd_assert(!src, -1, "invalid null pointer");

			dest->child_push_back(src->serial, 0, 0);
			if( (ch = dest->child_find(src->serial) ) ) {

				gnd::Conf::set_parameter(ch, &src->id);
				gnd::Conf::set_parameter(ch, &src->timeadjust);
				gnd::Conf::set_parameter(ch, &src->reflect);
				gnd::Conf::set_parameter(ch, &src->angle_range);
				gnd::Conf::set_parameter(ch, &src->position);
				gnd::Conf::set_parameter(ch, &src->orient);
				gnd::Conf::set_parameter(ch, &src->upside);
			}

			return 0;
		}

		/**
		 * @brief read configuration parameter file
		 * @param [in]  f    : configuration file name
		 * @param [out] dest : configuration parameter
		 */
		inline
		int dev_conf_read(const char* f, device_configuration* dest) {
			gnd_assert(!f, -1, "invalid null pointer");
			gnd_assert(!dest, -1, "invalid null pointer");

			{ // ---> operation
				int ret;
				gnd::Conf::FileStream fs;
				// configuration file read
				if( (ret = fs.read(f)) < 0 )    return ret;

				return dev_conf_get(&fs, dest);
			} // <--- operation
		}


		/**
		 * @brief file out  configure file
		 */
		inline
		int dev_conf_write( const char* f, device_configuration *src ){
			gnd_assert(!f, -1, "invalid null pointer");
			gnd_assert(!src, -1, "invalid null pointer");

			{ // ---> operation
				int ret;
				gnd::Conf::FileStream fs;
				if( (ret = dev_conf_set(&fs, src)) < 0 )	return ret;

				return fs.write(f);
			} // <--- operation
		}

	}
}
// <--- function definition

#endif /* URG_PROXY_DEVICE_CONF_HPP_ */
