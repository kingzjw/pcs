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
#include "zjw_math.h"


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
	ProbabilityAAC() = default;
	~ProbabilityAAC() = default;

	void setParam(double b ,double u = 0);
	//�����ۼƷֲ��������õ��ӵ��Ա���x�ĸ��� cumulative Distribution Function
	double getCDF(double x) const;

public:
	//������˹�ֲ��еĲ��� 
	//https://zh.wikipedia.org/wiki/%E6%8B%89%E6%99%AE%E6%8B%89%E6%96%AF%E5%88%86%E5%B8%83
	double b_ = 1;
	double u_ = 0;
};

class PCS_Color_AACoder
{
private:
	//���������в��Ե�������4,8.16,32,64
	//pcs�����еĲ������ݣ� 32,64,128,512,1024
	double qround = 4;
	Eigen::SparseMatrix<double> *spLaplacian;
	//�������������,ά�� (3* Nt,1),����x,y,z������Ϣ
	VectorXd *inputSignal;
	vector<Vec3> *m_colorInfo;
	//ѹ��ʱ�õ���motion vector����ɹ���x,y,z������
	VectorXd mvSignalXYZ[3];
	
public:
	PCS_Color_AACoder(VectorXd *signal, Eigen::SparseMatrix<double> *spLaplacian);
	PCS_Color_AACoder(vector<Vec3> *colorInfo, Eigen::SparseMatrix<double> *spLaplacian);
	~PCS_Color_AACoder();
	void setColorInfo(vector<Vec3> *colorInfo);
	//������ѹ������
	
	//�ͻ��˽�ѹ����
	
	//ѹ�������ڶ�color diff ��Ϣ����ѹ��ǰ���д���  
	void colorDiffSolving(vector<double> &dataBeforeCompress_out);
	//��ѹ�����ڴ���֮��GFT�������ݵĽ�ѹ
	void colorDiffSolvingDeCompress(vector<Vec3> & colorInfo_out);

private:
	//VectorXd inputSignal ���뵽 mvXSignal��mvYSignal, mvZSignal;
	bool separateVectorFromVecXd();

	// ��vector<Vec3> color info ��ɵ� VectorXd mvSignalXYZ[3]
	bool separateVectorFromColorInfo();

};