/*
 * opsm-map-maker-opt.hpp
 *
 *  Created on: 2013/09/27
 *      Author: tyamada
 */

#ifndef OPSM_MAP_MAKER_OPT_HPP_
#define OPSM_MAP_MAKER_OPT_HPP_



#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <ssmtype/spur-odometry.h>
#include <ssmtype/pws-motor.h>

#include "opsm-map-maker-conf.hpp"

#ifndef opsm_mm
#define opsm_mm opsm::map_maker
#endif

namespace opsm {
	namespace map_maker {

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
			options(): pconf(0) { init(); }
			options(proc_configuration *p) : pconf(p) { init(); }
			~options(){}
		private:
			void init();
		public:
			proc_configuration *pconf;

		public:
			int set(proc_configuration *p){
				pconf = p;
				return 0;
			}
			// <--- constructor

			// ---> operation
		public:
			bool get_option(int aArgc, char **aArgv);
			// <--- operation
		};


		const char ShortOpt[] = "hg:G::m:S:O:d:";

		const struct option LongOpt[] = {
				{"help", 							no_argument,		0,	'h'},
				{"config",							required_argument,	0,	'g'},
				{"write-config",					required_argument,	0,	'G'},
				{ConfIni_InitialMapDir.item,		required_argument,	0,	'm'},
				{ConfIni_LaserScannerSSMName.item,	required_argument,	0,	'S'},
				{ConfIni_PositionSSMName.item,		required_argument,	0,	'O'},
				{ConfIni_Culling.item,				required_argument,	0,	'd'},
				{0, 0, 0, 0}	// end of array
		};



		inline void options::init()
		{
		}

		inline bool options::get_option(int aArgc, char **aArgv)
		{
			gnd_assert(!pconf, -1, "parameter storage is null.");

			while(1){
				int opt;
				optarg = 0;
				opt = ::getopt_long(aArgc, aArgv, ShortOpt, LongOpt, 0);
				if(opt < 0)	break;

				switch(opt){

				// read configure
				case 'g':
				{
					if( proc_conf_read(optarg, pconf) < 0 ){
						::fprintf(stderr, " ... [\x1b[1m\x1b[31mERROR\x1b[30m\x1b[0m]: -g option, Fail to read configure file\n");
						return RFail;
					}
				} break;

				// write configure
				case 'G': {
					proc_conf_write( optarg ? optarg : "opsm-map-maker.conf", pconf);
					::fprintf(stderr, " ... output configuration file \"\x1b[4m%s\x1b[0m\"\n", optarg ? optarg : "opsm-map-maker.conf");
				} return RWriteConf;

				// entry map file directory
				case 'm': ::strcpy(pconf->init_map.value, optarg);			break;
				// entry sokuiki ssm-data id
				case 'S': ::strcpy( pconf->ls_name.value, optarg);		break;
				// entry sokuiki ssm-data id
				case 'O': ::strcpy( pconf->pos_name.value, optarg);		break;

				// data decimate threshold
				case 'd': pconf->culling.value = ::atoi(optarg);			break;


				// show help
				case 'h':
				{
					int i = 0;
					fprintf(stderr, "\t\x1b[1mNAME\x1b[0m\n");
					fprintf(stderr, "\t\t\x1b[1m%s\x1b[0m - position tracking using probabilistic scan matching\n", proc_name);
					fprintf(stderr, "\n");

					fprintf(stderr, "\t\x1b[1mSYNAPSIS\x1b[0m\n");
					fprintf(stderr, "\t\t\x1b[1m%s\x1b[0m [\x1b[4mOPTIONS\x1b[0m]\n", proc_name);
					fprintf(stderr, "\n");

					fprintf(stderr, "\t\x1b[1mDISCRIPTION\x1b[0m\n");
					fprintf(stderr, "\t\t\x1b[1m%s\x1b[0m estimate the robot position and optimize it with probabilistic scan matching.\n", proc_name);

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
					fprintf(stderr, "\t\t\tinput directory containing map files.\n");
					fprintf(stderr, "\n");
					i++;

					fprintf(stderr, "\t\t\x1b[1m-%c\x1b[0m, \x1b[1m--%s\x1b[0m\n", LongOpt[i].val, LongOpt[i].name);
					fprintf(stderr, "\t\t\tchenge tracking option\n");
					fprintf(stderr, "\n");
					i++;

					fprintf(stderr, "\t\t\x1b[1m-%c\x1b[0m, \x1b[1m--%s\x1b[0m\n", LongOpt[i].val, LongOpt[i].name);
					fprintf(stderr, "\t\t\tscan matching with Normal Distribution Transform algorithm.\n");
					fprintf(stderr, "\n");
					i++;

					fprintf(stderr, "\t\t\x1b[1m-%c\x1b[0m, \x1b[1m--%s\x1b[0m\n", LongOpt[i].val, LongOpt[i].name);
					fprintf(stderr, "\t\t\tset newton optimization\n");
					fprintf(stderr, "\n");
					i++;

					fprintf(stderr, "\t\t\x1b[1m-%c\x1b[0m, \x1b[1m--%s\x1b[0m\n", LongOpt[i].val, LongOpt[i].name);
					fprintf(stderr, "\t\t\tset quasi monte calro optimization\n");
					fprintf(stderr, "\n");
					i++;

					return RHelp;
				}break;
				}
			}
			return 0;
		}
	}; // namespace Optimizer
}; // namespace opsm





#endif /* OPSM_MAP_MAKER_OPT_HPP_ */
