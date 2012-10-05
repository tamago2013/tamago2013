/*
 * ssm-observation-probability-map.hpp
 *
 *  Created on: 2011/08/18
 *      Author: tyamada
 */

#ifndef SSM_OBSERVATION_PROBABILITY_MAP_HPP_
#define SSM_OBSERVATION_PROBABILITY_MAP_HPP_

#include <ssm.hpp>

#define SNAME_OBSERV_PROB_MAP	"ssm_observ_prob_map"

struct SSMObservProbMapDummy {
	int dummy;
};

struct SSMObservProbMapProperty {
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


typedef SSMApi<SSMObservProbMapDummy, SSMObservProbMapProperty>	SSMObservProbMap;

#endif /* SSM_OBSERVATION_PROBABILITY_MAP_HPP_ */
