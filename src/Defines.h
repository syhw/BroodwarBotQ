#pragma once

//our defines

#define __DEBUG__
//#define __NON_IMPLEMENTE__
#define BW_POS_MOUSE
//#define __LEARNING_PROB_TABLES__

#define WALK_TILES_SIZE 8

#define __DO_NOT_HARASS_SCOUT__ // will not compile the harassing part in the FirstScoutGoal (to play against the built-in AI)
#define __CONTROL_BO_UNTIL_SECOND_PYLON__ // no supply management before the second pylon, if you don't place it in the BO, you're locked
#define __ETECH_ESTIMATOR__ // use the ETechEstimator (or not if undef)
#define __BENS_LABELS__ // use Ben Weber's openings/strategies labels in the ETechEstimator
#define __WORKERS_FACTOR__ 1.18 // how much more workers we want on top of the optimal count (nb workers = optimal * __WORKERS_FACTOR__)
#define __MAX_WORKERS__ 75
#define __TILES_RADIUS_DEFEND_BASE__ 26
#define __MAX_TRIES_BUILD_SOMETHING__ 1440 // IN FRAMES, 1 minute here

#ifdef __DEBUG__
//#include <vld.h>
#endif

//#define __MICRO_PROJECT__
#ifdef __MICRO_PROJECT__
#undef __ETECH_ESTIMATOR__
#endif

#ifndef __DEBUG__ // Perf
#undef __DO_NOT_HARASS_SCOUT__
#define __RELEASE_OPTIM__
#ifdef __RELEASE_OPTIM__
#undef _SECURE_SCL
#define _SECURE_SCL 0 // remove iterator runtime checks
#define _HAS_ITERATOR_DEBUGGING 0
#endif
#endif