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
	//eigen求解特征向量
	EigenSolver<MatrixXd> *es;
	VectorXcd  eigenValueVer;
	MatrixXcd eigenVertorMat;
public:
	//拉普拉斯矩阵
	MatrixXd  lapMat;
	//常规信号
	VectorXd  signal;
	//gft形式下的信号
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
	* func: 利用拉普拉斯矩阵，以及信号，求解出信号在傅里叶域中的解   
	*/
	void gft(VectorXd & signal , VectorXd & signalGFT_out);
	void gft();
	
	/*
	* func: 利用拉普拉斯矩阵，以及信号，求解出信号在傅里叶域中的解
	*/
	void igft(VectorXd & siganlGFT, VectorXd & siganl_out);
	void igft();
};