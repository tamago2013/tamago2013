/*
 * opsm-particle-evaluator.h
 *
 *  Created on: 2012/03/21
 *      Author: tyamada
 */

#ifndef OPSM_PARTICLE_EVALUATOR_H_
#define OPSM_PARTICLE_EVALUATOR_H_

#ifndef OPSM
#define OPSM ObservationProbabilityScamMatching
#endif

#ifndef PEVAL
#define PEVAL ParticleEvaluator
#endif


namespace OPSM {
	namespace PEVAL {
		const double Frame = 0.025;
		const char ProcName[] = "opsm-particle-evaluator";
	}
}

#endif /* OPSM_PARTICLE_EVALUATOR_H_ */
