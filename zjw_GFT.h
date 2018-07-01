#pragma once

#include<Eigen/dense>
#include<Eigen/Sparse>
#include <iostream>

using namespace std;
using namespace Eigen;

/*
* author: JiaweiKing
* email: zjwking258@163.com
* data 2018/6/30
* head file: GFT (graph fourior transform)
* 
*/

class GFT
{
private:
	//eigen�����������
	EigenSolver<MatrixXd> *es;
	VectorXcd  eigenValueVer;
	MatrixXcd eigenVertorMat;
public:
	//������˹����
	MatrixXd  lapMat;
	//�����ź�
	VectorXd  signal;
	//gft��ʽ�µ��ź�
	VectorXd  signalGFT;

public:
	GFT(MatrixXd & lapMat);
	~GFT();
	
	void setLapMat(const MatrixXd &lapMat);
	void setSiganlVector(const VectorXd &siganl);
	void setSignalGFT(const VectorXd &signalGFT);
	void getLapMat( MatrixXd &lapMat_out);
	void getSiganlVector( VectorXd &siganl_out);
	void getSignalGFT( VectorXd &signalGFT_out);

	void computeEigenVector();
	void computeEigenValue();

	/*
	* func: ����������˹�����Լ��źţ������ź��ڸ���Ҷ���еĽ�   
	*/
	void gft(VectorXd & signal , VectorXd & signalGFT_out);
	void gft();
	
	/*
	* func: ����������˹�����Լ��źţ������ź��ڸ���Ҷ���еĽ�
	*/
	void igft(VectorXd & siganlGFT, VectorXd & siganl_out);
	void igft();
};