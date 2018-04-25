#pragma once
//setteing the macro for zjw use

//debug 时候的专用
#define ZJW_DEUG

//打印信息专用
//#define ZJW_PRINT_INFO

//是否打开计时器
#define ZJW_TIMER

//计时器
/*
用法：
	TIME_START
	TIME_END("mesh  load time")
*/
//static innoreal::InnoRealTimer timer_zjw;
//#define TIME_START timer_zjw.TimeRestart();timer_zjw.TimeStart();
//#define TIME_END(str) 		 timer_zjw.TimeEnd();timer_zjw.TimeGap_in_ms();\
//		std::printf("%s: %f ms\n", str,timer_zjw.Time_in_ms());

//------------------针对这个应用--------------------------

//下面三个只能选一个，因为CallTGetGraph 的构造函数问题

//求解出拉普拉斯矩阵的特征值，
//#define USE_EIGEN
//#define USE_ARPACK

//原来用稠密存储拉普拉斯矩阵，现在用稀疏的。
#define USE_SPARSE