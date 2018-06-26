#pragma once

/*
* author: Jiawei Zhou
* email: zjwking258@163.com
* headfile : c++ version of algorithm minresQLP
* time: 2018/6/26
*   
*/

#include <iostream>
#include<Eigen/dense>
#include<Eigen/Sparse>
#include <assert.h>

using namespace std;
using namespace Eigen;

/*
* A : symmtrix matrix
*
*
* maxit : specifies the maximum number of iterations.Default MAXIT = N
* M:  uses a matrix M as preconditioner. M must be positive definite and symmetric or Hermitian
*
*
*
* rnormvec: ��ʾ�Ƿ���Ҫresvec��Aresvec���������ز���
*/

class MinresQLP
{
private:
	static VectorXd minresxxxA(MatrixXd &A, VectorXd &x);
	//����������ٶȿ��ܻ��һ��
	static void minresxxxA(MatrixXd &A, VectorXd &x, VectorXd &res_out);
	static int sign(double num);
	static void SymGivens2(double &a, double &b, double& c, double& s, double& d);

public :
	//������Ľ����׼ȷ�������������������ɴΡ�
	static void zjw_minres_QLP(VectorXd &x_out, MatrixXd A, VectorXd b, double rtol = 1e-6, int maxit = 0, bool  M = false,
		double shift = 0, double maxxnorm = 1e7,	double Acondlim = 1e15, double TranCond = 1e7, bool show = false, bool  rnormvec = false);

	static int example1();

};
