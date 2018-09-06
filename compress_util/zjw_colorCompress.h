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

/*
* colorѹ��������Ӧ��������ĸ���ģ��
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

	//������˹�ֲ��еĲ��� 
	//https://zh.wikipedia.org/wiki/%E6%8B%89%E6%99%AE%E6%8B%89%E6%96%AF%E5%88%86%E5%B8%83
	double b;
	double u = 0;
};

class PCS_Color_AACoder
{
private:
	//���������в��Ե�������4,8.16,32,64
	//pcs�����еĲ������ݣ� 32,64,128,512,1024
	double qround = 4;
	Eigen::SparseMatrix<double> *spLaplacian;

	//�������������,ά�� (3* Nt,1),����x,y,z������Ϣ
	VectorXd *mvSignal;

	//ѹ��ʱ�õ���motion vector����ɹ���x,y,z������
	VectorXd mvSignalXYZ[3];
	
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