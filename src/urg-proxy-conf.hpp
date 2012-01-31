/*
 * urg-handler-conf.hpp
 *
 *  Created on: 2012/01/27
 *      Author: tyamada
 */

#ifndef URG_HANDLER_CONF_HPP_
#define URG_HANDLER_CONF_HPP_

#include "gnd-configuration.hpp"

// ---> type declaration
namespace gnd {
	namespace urg_proxy {
		struct configuration;
		typedef struct configuration configuration;
	}
} // <--- type declaration


// ---> constant value definition
namespace gnd {
	namespace urg_proxy {
		// device port
		static const gnd::Conf::parameter_array<char, 512> ConfIni_DevicePort = {
				"device-port",
				"/dev/ttyACM0",		// device port path
		};

		// device configuration file
		static const gnd::Conf::parameter_array<char, 512> ConfIni_DeviceConf = {
				"dev-conf",
				"",		// map file directory
		};

	}
} // <--- constant value definition



// ---> function declaration
namespace gnd {
	namespace urg_proxy {
		/**
		 * @brief initialize configure to default parameter
		 */
		int configure_initialize(configuration *conf);

		/**
		 * @brief analyze configure file
		 */
		int analyze_configure(gnd::Conf::Configuration *fconf, configuration *confp);

		/**
		 * @brief file out  configure file
		 */
		int write_configure( const char* fname, configuration *confp );
	}
} // <--- function declaration



// ---> type definition
namespace gnd {
	namespace urg_proxy {
		/**
		 * \brief particle localizer configure
		 */
		struct configuration {
			configuration();

			gnd::Conf::parameter_array<char, 512>	dev_port;
			gnd::Conf::parameter_array<char, 512>	dev_conf;
		};

		/**
		 * @brief constructor
		 */
		inline
		configuration::configuration(){
			configure_initialize(this);
		}
	}
} // <--- type definition



// ---> function definition
namespace gnd {
	namespace urg_proxy {

		/*!
		 * @brief initialize configure
		 */
		inline
		int configure_initialize(configuration *conf){
			yp_assert(!conf, -1, "invalid null pointer");

			::memcpy(&conf->dev_port,	&ConfIni_DevicePort,	sizeof(ConfIni_DevicePort) );
			::memcpy(&conf->dev_conf,	&ConfIni_DeviceConf,	sizeof(ConfIni_DeviceConf) );

			return 0;
		}

		/*
		 * @brief analyze
		 */
		inline
		int get_config_param(gnd::Conf::Configuration *conf, configuration *confp)
		{
			yp_assert(!conf, -1, "invalid null pointer");
			yp_assert(!confp, -1, "invalid null pointer");

			gnd::Conf::get_parameter(conf, &confp->dev_port);
			gnd::Conf::get_parameter(conf, &confp->dev_conf);
			return 0;
		}

		/**
		 * @brief file out  configure file
		 */
		inline
		int write_configure( const char* fname, configuration *confp ){

			yp_assert(!fname, -1, "invalid null pointer");
			yp_assert(!confp, -1, "invalid null pointer");

			{ // ---> operation
				gnd::Conf::FileStream conf;
				gnd::Conf::set_parameter(&conf, &confp->dev_port);
				gnd::Conf::set_parameter(&conf, &confp->dev_conf);

				return conf.wirte(fname);
			} // <--- operation
		}

	}
}; // <--- function definition


#endif /* URG_HANDLER_CONF_HPP_ */
