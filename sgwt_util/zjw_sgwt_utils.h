﻿#pragma once
#include <iostream>
#include <cstdio>
#include <vector>
#include <cmath>
#include <string>
#include <ctime>

//eigen
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/Core>
#include <Eigen/SparseCore>
//spect
#include <GenEigsSolver.h>
#include <MatOp/SparseGenMatProd.h>

#include "zjw_fminbnd.h"
#include "pcg.h"


using namespace std;
using namespace Eigen;
using namespace Spectra;

// 声明一个列优先的双精度稀疏矩阵类型
typedef Eigen::SparseMatrix<double> SpMat;
typedef double(*G)(double x);
class Sgwt;

//g和h实际使用的参数，可以在这里进行设置
struct Varargin
{
	string designtype;
	//K值  论文中设置成20
	double K;

	//alph
	double a;
	//beta
	double b;
	//x1
	double t1;
	//x2
	double t2;

	//abspline3： 实现的三次样条
	Varargin(string type = "abspline3", double _K = 20, double _a = 2, double _b = 2, double _t1 = 1, double _t2 = 2) :
		designtype(type), K(_K), a(_a), b(_b), t1(_t1), t2(_t2)
	{}
};

//kernel g functino   分装了scalue，需要其他辅助函数
class GN {
	//g函数的尺度
	double tj;
public:
	GN();
	double operator()(double x);
	void setTj(const double tj);
};

//function h(x)
class HX {
	//表示: λmin * 0.6
	double lminfac;
	//表示: γ
	double gamma_1;

	//hx_ptr是指向 Hx函数的指针，用于表示h（x）
	G hx_ptr;
public:

	//h(x) = γ exp(−(  x/ (0.6λ min))^4)
	double operator()(double x)
	{
		x = x / lminfac;
		return gamma_1*hx_ptr(x);
	}

	HX(G g1, double lminfac, double gamma_1)
	{
		this->lminfac = lminfac;
		this->hx_ptr = g1;
		this->gamma_1 = gamma_1;
	}
};

class Sgwt {
	vector<VectorXd> c;		//模拟cell

	//Chebyshev的系数，近似的M阶的相数
	const int m;

	//一个表示的是：0表示lmin 和 1表示lmax
	double arange[2];

	//稀疏矩阵 拉普拉斯
	SpMat lap;

	//提供默认的g和h的参数设置
	Varargin va;
	//类型为GN的的数组的指针，每个里面藏着scale
	GN *g;
	//num =	Nscales
	const int Nscales;

	//保存的是scales 列表（t1.t2.t3.,,,,.tl）
	VectorXd t;

	//kernel  h(x)
	HX *g0;

public:
	Sgwt(int _m, int _Nscales, SpMat _L);
	int getNscales()
	{
		return Nscales;
	}
	GN* getGN()
	{
		return g;
	}
	HX* getG0()
	{
		return g0;
	}
	vector<VectorXd>& getc()
	{
		return c;
	}

	//把 Laplace 的最大特征值设置好  Lamda
	void setArange(double lmin, double lmax);

	//利用spectra, 计算得到Laplacian 中最大的特征值
	double sgwt_rough_lmax();

	//设置kernel g 的scales
	VectorXd sgwt_setscales(const double lmin, const double lmax);

	//设置h(x)，并设置好相关的参数
	void sgwt_filter_design(double lmax, Varargin varargin);

	//计算Chebyshev的系数  g函数的近视系数C(k,g)  g 函数
	template<class T>
	void sgwt_cheby_coeff(int k, T g);		//n = m+1
	

	//chebushev的不等式求解信号
	vector<VectorXd> sgwt_cheby_op(VectorXd f, vector<VectorXd> c);

	//给定参数x,返回的是g(x)的值
	static double sgwt_kernel_abspline3(double x);
	//对sgwt_kernel_abspline3接口的返回值取相反数
	static double _sgwt_kernel_abspline3(double x);

	VectorXd sgwt_adjoint(vector<VectorXd> y);
	VectorXd sgwt_cheby_square(VectorXd c);
	//Known L, c, arange
	//VectorXd sgwt_inverse(vector<VectorXd> y);
};

typedef Eigen::SparseMatrix<double> SpMat;
class Sgwt;

class Handle_sgwt_cheby_op {
private:
	Sgwt *tmp_sgwt;
	vector<VectorXd> d;
public:
	Handle_sgwt_cheby_op(int m, int Nscales, SpMat L, vector<VectorXd> c, double *arange);

	vector<VectorXd> operator()(VectorXd x);
};
