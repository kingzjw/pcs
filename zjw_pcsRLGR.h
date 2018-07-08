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
	//RLGR���������ᵽ  stepSize deta from 0.01 - 1 ��Χ��Խ�ͣ������Խ�ߡ�
	double stepSize;
	
	Eigen::SparseMatrix<double> *spLaplacian;
	//�������������,ά�� (3* Nt,1),����x,y,z������Ϣ
	VectorXd *mvSignal;

	//motion vector����ɹ���x,y,z������
	VectorXd mvXSignal;
	VectorXd mvYSignal;
	VectorXd mvZSignal;
	
	//rlgb�л��õ���
	vector<uint64_t> inputList;
	vector<uint64_t> resList;

public:
	PCS_RLGR(VectorXd *mvSignal, Eigen::SparseMatrix<double> *spLaplacian);
	~PCS_RLGR();

	//������ѹ������
	void rlgr_mv_compress();

	//�ͻ��˽�ѹ����
	VectorXcd  rlgr_mv_decompress();

	//���Բ���(��ѹ��)
	VectorXcd  testPCS_RLGR();

private:
	//mvSignal ���뵽 mvXSignal��mvYSignal, mvZSignal;
	bool separateMotionVector();

	bool combineMotionVector();
};
