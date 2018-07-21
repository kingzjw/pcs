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

	//ѹ��ʱ�õ���motion vector����ɹ���x,y,z������
	VectorXd mvSignalXYZ[3];
	
	//rlgb encoder�л��õ���
	vector<int> inputList;
	vector<uint64_t> inputList_u;
	//rlgb decoder�л��õ���
	vector<int> resList;
	vector<uint64_t> resList_u;


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
	
	//ѹ����������ڸ���������ô�����������codeData_out��
	static void positiveNum(vector<int> & sourceDataNegativeList);
	//��ѹ��������ѹ����ʱ�������positive Num ��ô�ý�ѹ�õ�resData֮�󣬱������restoreNum���ָ��ɺ��и����ġ�
	static void restoreNum(vector<int>& resDecodeData);
	//ѹ����������ڸ���������ô�����������codeData_out��
	static void positiveNum(vector<int> & sourceDataNegativeList, vector<uint64_t>& codeData_out);
	//��ѹ��������ѹ����ʱ�������positive Num ��ô�ý�ѹ�õ�resData֮�󣬱������restoreNum���ָ��ɺ��и����ġ�
	static void restoreNum(vector<uint64_t> & sourceDecodeData, vector<int>& resDecodeData_out);

	//mvSignal ���뵽 mvXSignal��mvYSignal, mvZSignal;
	bool separateMotionVector();

	bool combineMotionVector();
};
