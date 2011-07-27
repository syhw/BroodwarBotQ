#pragma once
//our defines

#define __DEBUG__
//#define __NON_IMPLEMENTE__
#define BW_POS_MOUSE
//#define __LEARNING_PROB_TABLES__

#define WALK_TILES_SIZE 8

//#define __ARBITRATOR_CLEANUP__ // changes Arbitrator behavior
#define __ARBITRATOR_REMOVE_BID_CLEANUP__
#ifdef __DEBUG__
//#include <vld.h>
#endif

#ifndef __DEBUG__ // Perf
#undef _SECURE_SCL
#define _SECURE_SCL 0 // remove iterator runtime checks
#endif