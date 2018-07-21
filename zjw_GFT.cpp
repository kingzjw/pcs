#include "zjw_GFT.h"

void GFT::computeEigenVector()
{
	eigenVectorMat = es->eigenvectors();
}

void GFT::computeEigenValue()
{
	eigenValueVer = es->eigenvalues();
	//MatrixXcd D = es.eigenvalues().asDiagonal();
}

void GFT::gft(VectorXcd & signal, VectorXcd & signalGFT_out)
{
	//assert ( eigenVertorMat.rows() >0 );
	assert(signal.rows() == eigenVectorMat.rows());
	signalGFT_out.resize(signal.rows());
	
	cout << "得到的特征向量没有根据特征根，从小到大排序...." << endl;

	//遍历所有的特征向量
	for (int col_it = 0; col_it < eigenVectorMat.cols(); col_it++)
	{
		VectorXcd eigenVector = eigenVectorMat.col(col_it);
		//共轭矩阵
		eigenVector.conjugate();
		signalGFT_out(col_it) = signal.dot(eigenVector);
	}
}

void GFT::gft()
{
	gft(signal, signalGFT);
}

void GFT::igft(VectorXcd & signalGFT, VectorXcd & signal_out)
{
	//assert(eigenVectorMat.rows() >0);
	assert(signalGFT.rows() == eigenVectorMat.rows());
	signal_out.resize(signalGFT.rows());
	
	cout << "得到的特征向量没有根据特征根，从小到大排序...." << endl;

	//遍历信号值
	for (int row_it = 0; row_it < signalGFT.rows(); row_it++)
	{
		//row
		VectorXcd eigenVector = eigenVectorMat.row(row_it);
		signal_out(row_it) = signalGFT.dot(eigenVector);
		cout << signal_out(row_it) << endl;
	}
}

void GFT::igft()
{
	igft(signalGFT, signal);
}

GFT::GFT(MatrixXd & lapMat)
{
	this->lapMat = lapMat;
	es = new EigenSolver<MatrixXd>(lapMat);
	computeEigenVector();
	computeEigenValue();
}

GFT::~GFT()
{
	delete es;
}

void GFT::setLapMat(const MatrixXd & lapMat)
{
	this->lapMat = lapMat;
}

void GFT::setSiganlVector(const VectorXcd & signal)
{
	this->signal = signal;
}

void GFT::setSignalGFT(const VectorXcd & signalGFT)
{
	this->signalGFT = signalGFT;
}

void GFT::getLapMat(MatrixXd & lapMat_out)
{
	lapMat_out = this->lapMat;
}

void GFT::getSiganlVector(VectorXcd & siganl_out)
{
	siganl_out = this->signal;
}

void GFT::getSignalGFT(VectorXcd & signalGFT_out)
{
	signalGFT_out = this->signalGFT;
}

MatrixXcd GFT::getEigenVector()
{
	return eigenVectorMat;
}

VectorXcd GFT::getEigenValues()
{
	return eigenValueVer;
}
