/*
 * urg-coordinate-converter-conf.hpp
 *
 *  Created on: 2013/05/23
 *      Author: tyamada
 */

#ifndef URG_COORDINATE_CONVERTER_CONF_HPP_
#define URG_COORDINATE_CONVERTER_CONF_HPP_

#include "gnd-configuration.hpp"

// ---> type declaration
namespace gnd {
	namespace urg_cc {
		struct proc_configuration;
		typedef struct proc_configuration configuration;
	}
} // <--- type declaration


// ---> constant value definition
namespace gnd {
	namespace urg_cc {
		// sokuiki raw data ssm name
		static const gnd::Conf::parameter_array<char, 256> ConfIni_RawSSMName = {
				"ssm-name-raw",
				SSM_NAME_SCAN_POINT_2D,		// device port path
				"sokuiki raw data ssm name"
		};

		// sokuiki raw data ssm id
		static const gnd::Conf::parameter<int> ConfIni_RawSSMID = {
				"ssm-id-raw",
				0,		// device port path
				"sokuiki raw data ssm id"
		};

		// sokuiki fs data ssm name
		static const gnd::Conf::parameter_array<char, 256> ConfIni_FSSSMName = {
				"ssm-name-fs",
				SSM_NAME_SOKUIKI_3D_FS,		// device port path
				"sokuiki fs data ssm name"
		};

		// sokuiki fs data ssm id
		static const gnd::Conf::parameter<int> ConfIni_FSSSMID = {
				"ssm-id-fs",
				-1,
				"sokuiki fs data ssm id. if this value is less than 0, fs id is same to raw"
		};

		// sokuiki fs storage life
		static const gnd::Conf::parameter<double> ConfIni_FSStorageLife = {
				"storage-life-fs",
				5,	 // [sec]
				"sokuiki fs data storage life on ssm"
		};

	}
} // <--- constant value definition



// ---> function declaration
namespace gnd {
	namespace urg_cc {
		/**
		 * @brief initialize configure to default parameter
		 */
		int proc_conf_initialize(proc_configuration *conf);

		/**
		 * @brief analyze configure file
		 */
		int analyze_configure(gnd::Conf::Configuration *fconf, proc_configuration *confp);

		/**
		 * @brief file out  configure file
		 */
		int proc_conf_write( const char* fname, proc_configuration *confp );
	}
} // <--- function declaration



// ---> type definition
namespace gnd {
	namespace urg_cc {
		/**
		 * \brief particle localizer configure
		 */
		struct proc_configuration {
			proc_configuration();

			gnd::Conf::parameter_array<char, 256>	raw_name;
			gnd::Conf::parameter<int>				raw_id;
			gnd::Conf::parameter_array<char, 256>	fs_name;
			gnd::Conf::parameter<int>				fs_id;
			gnd::Conf::parameter<double>			fs_storage;
		};

		typedef struct proc_configuration proc_configuration;

		/**
		 * @brief constructor
		 */
		inline
		proc_configuration::proc_configuration(){
			proc_conf_initialize(this);
		}
	}
} // <--- type definition



// ---> function definition
namespace gnd {
	namespace urg_cc {

		/*!
		 * @brief initialize configure
		 */
		inline
		int proc_conf_initialize(proc_configuration *conf){
			gnd_assert(!conf, -1, "invalid null pointer");

			::memcpy(&conf->raw_name,	&ConfIni_RawSSMName,	sizeof(ConfIni_RawSSMName) );
			::memcpy(&conf->raw_id,		&ConfIni_RawSSMID,		sizeof(ConfIni_RawSSMID) );
			::memcpy(&conf->fs_name,	&ConfIni_FSSSMName,		sizeof(ConfIni_FSSSMName) );
			::memcpy(&conf->fs_id,		&ConfIni_FSSSMID,		sizeof(ConfIni_FSSSMID) );
			::memcpy(&conf->fs_storage,	&ConfIni_FSStorageLife,	sizeof(ConfIni_FSStorageLife) );

			return 0;
		}

		/*
		 * @brief analyze
		 */
		inline
		int proc_conf_get(gnd::Conf::Configuration *src, proc_configuration *dest)
		{
			gnd_assert(!src, -1, "invalid null pointer");
			gnd_assert(!dest, -1, "invalid null pointer");

			gnd::Conf::get_parameter(src, &dest->raw_name);
			gnd::Conf::get_parameter(src, &dest->raw_id);
			gnd::Conf::get_parameter(src, &dest->fs_name);
			gnd::Conf::get_parameter(src, &dest->fs_id);
			gnd::Conf::get_parameter(src, &dest->fs_storage);
			return 0;
		}


		/*
		 * @brief set configuration parameter
		 */
		inline
		int proc_conf_set(gnd::Conf::Configuration *dest, proc_configuration *src)
		{
			gnd_assert(!dest, -1, "invalid null pointer");
			gnd_assert(!src, -1, "invalid null pointer");

			gnd::Conf::set_parameter(dest, &src->raw_name);
			gnd::Conf::set_parameter(dest, &src->raw_id);
			gnd::Conf::set_parameter(dest, &src->fs_name);
			gnd::Conf::set_parameter(dest, &src->fs_id);
			gnd::Conf::set_parameter(dest, &src->fs_storage);
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
				gnd::Conf::FileStream fs;
				// configuration file read
				if( (ret = fs.read(f)) < 0 )    return ret;

				return proc_conf_get(&fs, dest);
			} // <--- operation
		}

		/**
		 * @brief file out  configure file
		 */
		inline
		int proc_conf_write( const char* f, proc_configuration *src ){
			gnd_assert(!f, -1, "invalid null pointer");
			gnd_assert(!src, -1, "invalid null pointer");

			{ // ---> operation
				int ret;
				gnd::Conf::FileStream fs;
				if( (ret = proc_conf_set(&fs, src)) < 0 )	return ret;

				return fs.write(f);
			} // <--- operation
		}

	}
}; // <--- function definition


#endif /* URG_HANDLER_CONF_HPP_ */
