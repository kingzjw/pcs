#pragma once

/*
* �Ե��������е���ɫ��Ϣ����ѹ���ͽ�ѹ�����ڰ˲���)
* //point cloud attribute compression with graph transform ����
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

class PCS_Color_AACoder
{
private:
	//�����в��Ե�������4,8.16,32,64
	double qround = 4;

	Eigen::SparseMatrix<double> *spLaplacian;
	//�������������,ά�� (3* Nt,1),����x,y,z������Ϣ
	VectorXd *mvSignal;

	//ѹ��ʱ�õ���motion vector����ɹ���x,y,z������
	VectorXd mvSignalXYZ[3];

	//rlgb encoder�л��õ���
	/*vector<int> inputList;
	vector<uint64_t> inputList_u;*/

	//rlgb decoder�л��õ���
	/*vector<int> resList;
	vector<uint64_t> resList_u;*/


public:
	PCS_Color_AACoder(VectorXd *mvSignal, Eigen::SparseMatrix<double> *spLaplacian);
	~PCS_Color_AACoder();

	//������ѹ������
	
	//�ͻ��˽�ѹ����
	
	//���Բ���(��ѹ��)
	

private:
	//mvSignal ���뵽 mvXSignal��mvYSignal, mvZSignal;
	bool separateVector();
};