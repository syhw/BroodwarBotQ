#pragma once
//our defines

#define __DEBUG__
//#define __NON_IMPLEMENTE__
#define BW_POS_MOUSE
//#define __LEARNING_PROB_TABLES__

#define WALK_TILES_SIZE 8

#define __ARBITRATOR_CLEANUP__ // changes Arbitrator behavior, but is safe
#ifndef __ARBITRATOR_CLEANUP__ // XOR between these two
#define __ARBITRATOR_REMOVE_BID_CLEANUP__ // doesn't change Arbitrator behavior, but is not safe
#endif
#define __CONTROL_BO_UNTIL_SECOND_PYLON__
#ifdef __DEBUG__
//#include <vld.h>
#endif

#ifndef __DEBUG__ // Perf
#define __RELEASE_OPTIM__
#ifdef __RELEASE_OPTIM__
#undef _SECURE_SCL
#define _SECURE_SCL 0 // remove iterator runtime checks
#endif
#endif