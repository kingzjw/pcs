#pragma once
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
//#include "zjw_math.h"
#include <GenEigsSolver.h>
#include <MatOp/SparseGenMatProd.h>

#include "zjw_macro.h"
#include "zjw_fminbnd.h"
#include "pcg.h"
#include "signalType.h"


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
	Varargin(string type = "abspline3", double _K = 20, double _a = 2, 
		double _b = 2, double _t1 = 1, double _t2 = 2) :
		designtype(type), K(_K), a(_a), b(_b), t1(_t1), t2(_t2)
	{}
	
	//paper zjw 
	/*Varargin(string type = "abspline3", double _K = 20, double _a = 1, double _b = 1, double _t1 = 1, double _t2 = 2) :
		designtype(type), K(_K), a(_a), b(_b), t1(_t1), t2(_t2)
	{}*/

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


/*
  封装了利用切比雪夫展开式快速求解sgwt的工具接口
*/
class Sgwt {
public:
	//模拟cell  大小  (Nscales+1) * (M+1)
	//c(n,k) 所有的系数
	vector<VectorXd> coeff;

	//Chebyshev的系数，近似的M阶的相数
	const int m;

	//提供默认的g和h的参数设置
	Varargin va;
	//类型为GN的的数组的指针，每个里面藏着scale
	//保存的是scales 列表（t1.t2.t3.,,,,.tl）
	VectorXd t;
	//g函数总共从t1一直到  tn (Nscales),在加上前面还有个h
	//g0 表示的是 scale t1的g函数 ，g1表示 scale t2函数
	GN *g;
	const int Nscales;

	//kernel  h(x)
	HX *h0;

	//稀疏矩阵 拉普拉斯
	SpMat lap;

	//一个表示的是：0表示lmin 和 1表示lmax
	double arange[2];

	//最外面的vector 的大小： 信号的数量 * 象限数  48
	//里面的vector的大小，尺度+h的大小
	//每个向量存储每个尺度下的系数(每个尺度：对应所有的顶点)
	vector<vector<VectorXd>> sgwtCoeff_WS;

	//int signalNum;
	//int quadrantNum;

public:
	//_m 切比雪夫的高阶项模拟  _Nscales
	Sgwt(int _m, int _Nscales, SpMat _lap);

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
		return h0;
	}
	vector<VectorXd>& getCoeff()
	{
		return coeff;
	}

	//利用spectra, 计算得到Laplacian 中最大的特征值 lmax
	double sgwt_rough_lmax();

	//把 Laplace 的最大特征值设置好  Lamda
	void setArange(double lmin, double lmax);

	//设置kernel g 的scales
	VectorXd sgwt_setscales(const double lmin, const double lmax);

	//设置h(x)，并设置好相关的参数
	void sgwt_filter_design(double lmax, Varargin varargin);

	//计算Chebyshev的系数  g函数的近视系数C(k,g)  g 函数(g函数里面包含了尺度信息)
	//参数说明： j表示的第几个尺度，范围从(0 - nscale-1),g表示第j的尺度下的g函数，内含尺度信息。
	//函数作用： 计算在tj尺度下面，从0-m阶的系数  c(j,0), c(j,1), c(j,2)的系数
	template<class T>
	void sgwt_cheby_coeff(int k, T g);		//n = m+1

	//chebushev的不等式求解信号
	//传入的信号 f(f是包括所有节点上的信号的) ,以及计算好的多项式的近似系数
	//返回：vector包含是各个scale和h尺度下面的针对信号f的sgwt系数
	vector<VectorXd> sgwt_cheby_op(VectorXd f, vector<VectorXd> c);

	//chebushev的不等式求解信号
	//传入的信号 f(f是包括所有节点上的信号的) ,以及计算好的多项式的近似系数
	//返回：vector包含是各个scale和h尺度下面的针对信号f的sgwt系数
	bool sgwt_cheby_op(VectorXd f, vector<VectorXd> c, vector<VectorXd>& sgwt_out);

	//input: type:信号的类型， quadrant: 象限 
	bool saveSgwtCoeff(SignalType type, int quadrant,VectorXd *f, vector<VectorXd> *c);

	//通过信号的类型，象限的类型，指定的node idx 返回这个node在的信号(5*1 因为scale 和 h是五维的)
	bool sgwt_cheby_op(int nodeIdx, int signalType, int quadrantType, VectorXd* sgwt_out);

	//给定参数x,返回的是g(x)的值
	static double sgwt_kernel_abspline3(double x);
	//对sgwt_kernel_abspline3接口的返回值取相反数
	static double _sgwt_kernel_abspline3(double x);

	VectorXd sgwt_adjoint(vector<VectorXd> y);
	VectorXd sgwt_cheby_square(VectorXd c);
	//Known L, c, arange
	//VectorXd sgwt_inverse(vector<VectorXd> y);

#ifdef ZJW_DEBUG
	void printVectorVectorXd(vector<VectorXd>& vv);
	void getVectorVectorXdInfo(vector<VectorXd>& vv);
#endif //zjw_debug

};

/*
	利用Sqwt封装好的工具，提供便捷的使用的接口
*/
class SgwtCheby {
public:
	Sgwt *sgwt;

	//尺度的数量+1 就是vector的维数
	vector<VectorXd> chebyCoeff;
public:
	SgwtCheby(int m, int Nscales, SpMat& L, vector<VectorXd>& c, double *arange);

	SgwtCheby(int m, int Nscales, SpMat& L);
	//sgwt 切比雪夫的准备工作，准备计算
	void sgwtDoChebyPrepare();
	//重载操作符"()"，没传入一个信号，俺么返回这个信号对应的系数
	//传入的x是，特定某个信号，在某个象限下的信息（如果是叶子结点是N个，那么就是N*1维度）
	vector<VectorXd> operator()(VectorXd x);

	//isSiglePoint:标记输入的f是一个顶点   f: 1*1维度的
	void operator()(int nodeIdx, int signalType, int quadrantType, VectorXd* sgwt_out);
};
