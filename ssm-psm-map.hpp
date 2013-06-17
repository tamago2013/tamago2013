/*
 * ssm-psm-map.hpp
 *
 *  Created on: 2011/08/18
 *      Author: tyamada
 */

#ifndef SSM_PSM_MAP_HPP_
#define SSM_PSM_MAP_HPP_

#include <ssm.hpp>

#define SNAME_PSM_MAP	"ssm_psm_map"

struct SSMPSMMapDummy {
	int dummy;
};

struct SSMPSMMapProperty {
	char fname[256];
	// 画像の角の座標
	// 画像左上から反時計回り
	struct {
		double x;
		double y;
	} point[4];
	double scale;	// [m/pixel]
	double offset;
};


typedef SSMApi<SSMPSMMapDummy, SSMPSMMapProperty>	SSMPSMMap;

#endif /* SSM_PSM_MAP_HPP_ */
