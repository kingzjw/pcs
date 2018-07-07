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

	//得到的特征向量没有根据特征根，从小到大排序。？？？？？？？？？？？？？？？？？
	MatrixXcd eigenVectorMat;
public:
	//拉普拉斯矩阵
	MatrixXd  lapMat;
	//常规信号
	VectorXcd  signal;
	//gft形式下的信号
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
	* func: 利用拉普拉斯矩阵，以及信号，求解出信号在傅里叶域中的解   
	*/
	void gft(VectorXcd & signal , VectorXcd & signalGFT_out);
	void gft();
	
	/*
	* func: 利用拉普拉斯矩阵，以及信号，求解出信号在傅里叶域中的解
	*/
	void igft(VectorXcd & signalGFT, VectorXcd & signal_out);
	void igft();
};