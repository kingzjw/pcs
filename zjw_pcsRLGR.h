#pragma once

//eigen
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "zjw_macro.h"
#include "zjw_GFT.h"
#include "compress_util\zjw_RLGR.h"

using namespace std;
using namespace Eigen;

class PCS_RLGR
{
private:
	//RLGR论文中有提到  stepSize deta from 0.01 - 1 范围。越低，保真度越高。
	double stepSize;
	
	Eigen::SparseMatrix<double> *spLaplacian;
	//接受外面的数据,维度 (3* Nt,1),包含x,y,z三种信息
	VectorXd *mvSignal;

	//motion vector分离成关于x,y,z的特征
	VectorXd mvXSignal;
	VectorXd mvYSignal;
	VectorXd mvZSignal;
	
	//rlgb中会用到的
	vector<uint64_t> inputList;
	vector<uint64_t> resList;

public:
	PCS_RLGR(VectorXd *mvSignal, Eigen::SparseMatrix<double> *spLaplacian);
	~PCS_RLGR();

	//服务器压缩部分
	void rlgr_mv_compress();

	//客户端解压部分
	VectorXcd  rlgr_mv_decompress();

	//测试部分(解压缩)
	VectorXcd  testPCS_RLGR();

private:
	//mvSignal 分离到 mvXSignal，mvYSignal, mvZSignal;
	bool separateMotionVector();

	bool combineMotionVector();
};
