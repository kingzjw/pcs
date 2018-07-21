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

	//压缩时用到：motion vector分离成关于x,y,z的特征
	VectorXd mvSignalXYZ[3];
	
	//rlgb encoder中会用到的
	vector<int> inputList;
	vector<uint64_t> inputList_u;
	//rlgb decoder中会用到的
	vector<int> resList;
	vector<uint64_t> resList_u;


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
	
	//压缩：如果存在负整数，那么处理成正整数codeData_out。
	static void positiveNum(vector<int> & sourceDataNegativeList);
	//解压：若果在压缩的时候调用了positive Num 那么久解压得到resData之后，必须调用restoreNum，恢复成含有负数的。
	static void restoreNum(vector<int>& resDecodeData);
	//压缩：如果存在负整数，那么处理成正整数codeData_out。
	static void positiveNum(vector<int> & sourceDataNegativeList, vector<uint64_t>& codeData_out);
	//解压：若果在压缩的时候调用了positive Num 那么久解压得到resData之后，必须调用restoreNum，恢复成含有负数的。
	static void restoreNum(vector<uint64_t> & sourceDecodeData, vector<int>& resDecodeData_out);

	//mvSignal 分离到 mvXSignal，mvYSignal, mvZSignal;
	bool separateMotionVector();

	bool combineMotionVector();
};
