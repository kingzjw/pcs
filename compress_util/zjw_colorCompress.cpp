#include "zjw_colorCompress.h"

PCS_Color_AACoder::PCS_Color_AACoder(VectorXd * mvSignal, Eigen::SparseMatrix<double>* spLaplacian)
{
	//RLGR论文中有提到  stepSize deta from 0.01 - 1 范围。越低，保真度越高。
	this->mvSignal = mvSignal;
	this->spLaplacian = spLaplacian;
}

PCS_Color_AACoder::~PCS_Color_AACoder()
{

}

bool PCS_Color_AACoder::separateVector()
{
	//x
	mvSignalXYZ[0].resize(mvSignal->rows() / 3);
	//y
	mvSignalXYZ[1].resize(mvSignal->rows() / 3);
	//z
	mvSignalXYZ[2].resize(mvSignal->rows() / 3);

	for (int i = 0; i < mvSignal->rows() / 3; i++)
	{
		mvSignalXYZ[0](i) = (*mvSignal)(3 * i);
		mvSignalXYZ[1](i) = (*mvSignal)(3 * i + 1);
		mvSignalXYZ[2](i) = (*mvSignal)(3 * i + 2);
	}
	return true;
}

