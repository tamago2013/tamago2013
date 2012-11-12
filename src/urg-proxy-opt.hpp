/*
 * urg-proxy-opt.hpp
 *
 *  Created on: 2012/01/27
 *      Author: tyamada
 */

#ifndef URG_PROXY_OPT_HPP_
#define URG_PROXY_OPT_HPP_

#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "urg-proxy-conf.hpp"
#include "urg-proxy-device-conf.hpp"

namespace gnd {
	namespace urg_proxy {
		class options
		{
		// ---> return value define
		public:
			static const int RFail = -1;		///<! return value Failure
			static const int RHelp = 1;			///<! return value Help
			static const int RWriteConf = 2;	///<! return value Write Configure File
		// <--- return value define

		// ---> constructor
		public:
			options(): param(0) { init(); }
			options(proc_configuration *p) : param(p) { init(); }
			~options(){}
		private:
			void init();
		public:
			proc_configuration *param;

		public:
			int set(proc_configuration *p){
				param = p;
				return 0;
			}
			// <--- constructor

			// ---> operation
		public:
			bool get_option(int aArgc, char **aArgv);
			// <--- operation
		};


		const char proc_name[] = "urg-proxy";


		const char ShortOpt[] = "hg:G::p:d:D::";

		const struct option LongOpt[] = {
				{"help", 						no_argument,		0,	'h'},
				{"config",						required_argument,	0,	'g'},
				{"write-config",				optional_argument,	0,	'G'},
				{ConfIni_DevicePort.token,		required_argument,	0,	'p'},
				{ConfIni_DeviceConf.token,		required_argument,	0,	'd'},
				{"write-dev-conf",				optional_argument,	0,	'D'},
				{0, 0, 0, 0}	// end of array
		};



		inline void options::init()
		{
		}

		inline bool options::get_option(int aArgc, char **aArgv)
		{
			gnd_assert(!param, -1, "parameter storage is null.");

			while(1){
				int opt;
				optarg = 0;
				opt = ::getopt_long(aArgc, aArgv, ShortOpt, LongOpt, 0);
				if(opt < 0)	break;

				switch(opt){

				// read configure
				case 'g':
				{
					gnd::Conf::FileStream conf_fs;
					if( conf_fs.read(optarg) < 0 ){
						::fprintf(stderr, " ... [\x1b[1m\x1b[31mERROR\x1b[30m\x1b[0m]: -g option, Fail to read configure file\n");
						return RFail;
					}
					if( proc_conf_get(&conf_fs, param) < 0){
						::fprintf(stderr, " ... [\x1b[1m\x1b[31mERROR\x1b[30m\x1b[0m]: -g option, configure file syntax error\n");
						return RFail;
					}
				} break;

				// write configure
				case 'G': {
					proc_conf_write( optarg ? optarg : "urg-proxy.conf", param);
					::fprintf(stdout, " ... output configuration file \"\x1b[4m%s\x1b[0m\"\n", optarg ? optarg : "urg-proxy.conf");
				} return RWriteConf;
				// write configure


				// entry device port path
				case 'p': ::strcpy(param->dev_port.value, optarg);			break;
				// entry device configuration file
				case 'd': ::strcpy(param->dev_conf.value, optarg);			break;

				case 'D': {
					device_configuration devparam;
					dev_conf_write( optarg ? optarg : "urg-dev.conf", &devparam);
					::fprintf(stdout, " ... output device configuration file \"\x1b[4m%s\x1b[0m\"\n", optarg ? optarg : "urg-dev.conf");
				} return RWriteConf;

				// show help
				case 'h':
				{
					int i = 0;
					fprintf(stderr, "\t\x1b[1mNAME\x1b[0m\n");
					fprintf(stderr, "\t\t\x1b[1m%s\x1b[0m - observation probability scan matching optimizer\n", proc_name);
					fprintf(stderr, "\n");

					fprintf(stderr, "\t\x1b[1mSYNAPSIS\x1b[0m\n");
					fprintf(stderr, "\t\t\x1b[1m%s\x1b[0m [\x1b[4mOPTIONS\x1b[0m]\n", proc_name);
					fprintf(stderr, "\n");

					fprintf(stderr, "\t\x1b[1mDISCRIPTION\x1b[0m\n");
					fprintf(stderr, "\t\t\x1b[1m%s\x1b[0m is estimate the robot position and optimize it with newton method.\n", proc_name);

					fprintf(stderr, "\n");
					fprintf(stderr, "\t\x1b[1mOPTIONS\x1b[0m\n");
					fprintf(stderr, "\t\t\x1b[1m-%c\x1b[0m, \x1b[1m--%s\x1b[0m\n", LongOpt[i].val, LongOpt[i].name);
					fprintf(stderr, "\t\t\tprint help\n");
					fprintf(stderr, "\n");
					i++;

					fprintf(stderr, "\t\t\x1b[1m-%c\x1b[0m, \x1b[1m--%s\x1b[0m\n", LongOpt[i].val,  LongOpt[i].name);
					fprintf(stderr, "\t\t\tread configure file\n");
					fprintf(stderr, "\n");
					i++;

					fprintf(stderr, "\t\t\x1b[1m-%c\x1b[0m, \x1b[1m--%s\x1b[0m\n", LongOpt[i].val,  LongOpt[i].name);
					fprintf(stderr, "\t\t\twirte configure file\n");
					fprintf(stderr, "\n");
					i++;

					fprintf(stderr, "\t\t\x1b[1m-%c\x1b[0m, \x1b[1m--%s\x1b[0m\n", LongOpt[i].val, LongOpt[i].name);
					fprintf(stderr, "\t\t\tdevice port path\n");
					fprintf(stderr, "\n");
					i++;

					fprintf(stderr, "\t\t\x1b[1m-%c\x1b[0m, \x1b[1m--%s\x1b[0m\n", LongOpt[i].val, LongOpt[i].name);
					fprintf(stderr, "\t\t\tdevice configuration file\n");
					fprintf(stderr, "\n");
					i++;

					return RHelp;
				}break;
				}
			}
			return 0;
		}
	} // namespace urg
}; // namespace gnd

#endif /* URG_PROXY_OPT_HPP_ */
