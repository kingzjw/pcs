#pragma once
#include <stdlib.h>
//setteing the macro for zjw use

//debug 时候的专用
#define ZJW_DEBUG

//打印信息专用
//#define ZJW_PRINT_INFO
#define ZJW_INFO(a) printf("%s\n",a);
//打印信息
//#define PRINT_CHEBY_COEFF

//是否打开计时器
#define ZJW_TIMER


//------------------针对这个应用--------------------------

//信号的类型。 不定义就是绝对距离。定义了就是相对距离
//#define RELATIVE_DIS_SIGNAL


//下面三个只能选一个，因为CallTGetGraph 的构造函数问题

//求解出拉普拉斯矩阵的特征值，
//1
//#define USE_EIGEN

//2
//#define USE_ARPACK

//3
//原来用稠密存储拉普拉斯矩阵，现在用稀疏的。
#define USE_SPARSE
#define SGWT_DEBUG




