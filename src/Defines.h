#pragma once
//our defines

#define __DEBUG__
//#define __NON_IMPLEMENTE__
#define BW_POS_MOUSE
//#define __LEARNING_PROB_TABLES__

#define WALK_TILES_SIZE 8

#ifdef __DEBUG__
//#include <vld.h>
#endif

#ifndef __DEBUG__ // Perf
#define _SECURE_SCL 0 // remove iterator runtime checks
#endif