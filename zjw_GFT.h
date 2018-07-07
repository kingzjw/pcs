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

	//�õ�����������û�и�������������С�������򡣣���������������������������������
	MatrixXcd eigenVectorMat;
public:
	//������˹����
	MatrixXd  lapMat;
	//�����ź�
	VectorXcd  signal;
	//gft��ʽ�µ��ź�
	VectorXcd  signalGFT;

private:
	void computeEigenVector();
	void computeEigenValue();

public:
	GFT(MatrixXd & lapMat);
	~GFT();
	
	void setLapMat(const MatrixXd &lapMat);
	void setSiganlVector(const VectorXcd &siganl);
	void setSignalGFT(const VectorXcd &signalGFT);

	void getLapMat( MatrixXd &lapMat_out);
	void getSiganlVector(VectorXcd &siganl_out);
	void getSignalGFT(VectorXcd &signalGFT_out);
	MatrixXcd getEigenVector();
	VectorXcd getEigenValues();


	/*
	* func: ����������˹�����Լ��źţ������ź��ڸ���Ҷ���еĽ�   
	*/
	void gft(VectorXcd & signal , VectorXcd & signalGFT_out);
	void gft();
	
	/*
	* func: ����������˹�����Լ��źţ������ź��ڸ���Ҷ���еĽ�
	*/
	void igft(VectorXcd & signalGFT, VectorXcd & signal_out);
	void igft();
};