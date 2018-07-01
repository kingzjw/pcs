#include "zjw_GFT.h"

void GFT::computeEigenVector()
{
	eigenVertorMat = es->eigenvectors();
}

void GFT::computeEigenValue()
{
	eigenValueVer = es->eigenvalues();
	//MatrixXcd D = es.eigenvalues().asDiagonal();
}

void GFT::gft(VectorXd & signal, VectorXd & signalGFT_out)
{
	assert ( eigenVertorMat.rows() >0 );
	signalGFT_out.resize(signal.rows());

}

void GFT::igft(VectorXd & siganlGFT, VectorXd & siganl_out)
{
	assert(eigenVertorMat.rows() >0);
	siganl_out.resize(siganlGFT.rows());

}

GFT::GFT(MatrixXd & lapMat)
{
	this->lapMat = lapMat;
	es = new EigenSolver<MatrixXd>(lapMat);
	computeEigenVector();
}

GFT::~GFT()
{
	delete es;
}

void GFT::setLapMat(const MatrixXd & lapMat)
{
	this->lapMat = lapMat;
}

void GFT::setSiganlVector(const VectorXd & signal)
{
	this->signal = signal;
}

void GFT::setSignalGFT(const VectorXd & signalGFT)
{
	this->signalGFT = signalGFT;
}

void GFT::getLapMat(MatrixXd & lapMat_out)
{
	lapMat_out = this->lapMat;
}

void GFT::getSiganlVector(VectorXd & siganl_out)
{
	siganl_out = this->signal;
}

void GFT::getSignalGFT(VectorXd & signalGFT_out)
{
	signalGFT_out = this->signalGFT;
}
