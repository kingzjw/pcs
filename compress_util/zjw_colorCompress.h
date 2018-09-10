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
#include "zjw_math.h"


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
	ProbabilityAAC() = default;
	~ProbabilityAAC() = default;

	void setParam(double b ,double u = 0);
	//根据累计分布函数，得到从到自变量x的概率 cumulative Distribution Function
	double getCDF(double x) const;

public:
	//拉普拉斯分布中的参数 
	//https://zh.wikipedia.org/wiki/%E6%8B%89%E6%99%AE%E6%8B%89%E6%96%AF%E5%88%86%E5%B8%83
	double b_ = 1;
	double u_ = 0;
};

class PCS_Color_AACoder
{
private:
	//引用论文中测试的数据是4,8.16,32,64
	//pcs论文中的测试数据： 32,64,128,512,1024
	double qround = 4;
	Eigen::SparseMatrix<double> *spLaplacian;
	//接受外面的数据,维度 (3* Nt,1),包含x,y,z三种信息
	VectorXd *inputSignal;
	vector<Vec3> *m_colorInfo;
	//压缩时用到：motion vector分离成关于x,y,z的特征
	VectorXd mvSignalXYZ[3];
	
public:
	PCS_Color_AACoder(VectorXd *signal, Eigen::SparseMatrix<double> *spLaplacian);
	PCS_Color_AACoder(vector<Vec3> *colorInfo, Eigen::SparseMatrix<double> *spLaplacian);
	~PCS_Color_AACoder();
	void setColorInfo(vector<Vec3> *colorInfo);
	//服务器压缩部分
	
	//客户端解压部分
	
	//压缩：用于对color diff 信息，在压缩前进行处理。  
	void colorDiffSolving(vector<double> &dataBeforeCompress_out);
	//解压：用于传输之后，GFT传输内容的解压
	void colorDiffSolvingDeCompress(vector<Vec3> & colorInfo_out);

private:
	//VectorXd inputSignal 分离到 mvXSignal，mvYSignal, mvZSignal;
	bool separateVectorFromVecXd();

	// 从vector<Vec3> color info 变成到 VectorXd mvSignalXYZ[3]
	bool separateVectorFromColorInfo();

};