#ifndef _CAL_H_202001_09
#define _CAL_H_202001_09
#include <math.h>
#include <limits>
#ifdef _WIN32
#include <Windows.h>

#	define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & (1<<(sizeof(SHORT)*8-1))) ? 1 : 0)  
#	define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & (1<<(sizeof(SHORT)*8-1))) ? 0 : 1)
#endif

const float FLOAT_EPS = 1e-6f;
const double DOUBLE_EPS = 1e-15; 
       
#define FIS_EQUAL(f1, f2)	(fabs((f1) - (f2)) <= numeric_limits<float>::epsilon())
#define FIS_GREATER(f1, f2) (((f1) - (f2)) > numeric_limits<float>::epsilon())
#define FIS_LESSER(f1, f2)	(((f1) - (f2)) < -numeric_limits<float>::epsilon())
#define FIS_LIMIT(f1)	    (fabs(f1) >= FLT_MIN && fabs(f1) <= FLT_MAX || f1 == 0.0f) 

#define DIS_EQUAL(d1, d2)	(fabs((d1) - (d2)) <= numeric_limits<double>::epsilon())

#endif