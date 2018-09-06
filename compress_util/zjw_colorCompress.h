#pragma once

/*
* 对点云序列中的颜色信息进行压缩和解压（基于八叉树)
* //point cloud attribute compression with graph transform 论文
* point cloud sequence  Color Adaptive arithmetic coding
*/

//eigen
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "zjw_macro.h"
#include "zjw_GFT.h"

using namespace std;
using namespace Eigen;

/*
* color压缩，自适应算术编码的概率模型
*/
enum class Terms {
	ACTerms, DCTerms
};

class ProbabilityAAC
{
public:
	ProbabilityAAC(Terms term) {
		term_ = term;
	}
	~ProbabilityAAC() {	}


public:
	Terms term_;

	//拉普拉斯分布中的参数 
	//https://zh.wikipedia.org/wiki/%E6%8B%89%E6%99%AE%E6%8B%89%E6%96%AF%E5%88%86%E5%B8%83
	double b;
	double u = 0;
};

class PCS_Color_AACoder
{
private:
	//引用论文中测试的数据是4,8.16,32,64
	//pcs论文中的测试数据： 32,64,128,512,1024
	double qround = 4;
	Eigen::SparseMatrix<double> *spLaplacian;

	//接受外面的数据,维度 (3* Nt,1),包含x,y,z三种信息
	VectorXd *mvSignal;

	//压缩时用到：motion vector分离成关于x,y,z的特征
	VectorXd mvSignalXYZ[3];
	
public:
	PCS_Color_AACoder(VectorXd *mvSignal, Eigen::SparseMatrix<double> *spLaplacian);
	~PCS_Color_AACoder();

	//服务器压缩部分
	
	//客户端解压部分
	
	//测试部分(解压缩)
	

private:
	//mvSignal 分离到 mvXSignal，mvYSignal, mvZSignal;
	bool separateVector();
};