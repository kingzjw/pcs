#include "zjw_colorCompress.h"

PCS_Color_AACoder::PCS_Color_AACoder(VectorXd * signal, Eigen::SparseMatrix<double>* spLaplacian)
{
	//RLGR���������ᵽ  stepSize deta from 0.01 - 1 ��Χ��Խ�ͣ������Խ�ߡ�
	this->inputSignal = signal;
	this->spLaplacian = spLaplacian;
}

PCS_Color_AACoder::PCS_Color_AACoder(vector<Vec3>* colorInfo, Eigen::SparseMatrix<double>* spLaplacian)
{
	this->m_colorInfo = colorInfo;
	this->spLaplacian = spLaplacian;
}

PCS_Color_AACoder::~PCS_Color_AACoder()
{

}

void PCS_Color_AACoder::setColorInfo(vector<Vec3>* colorInfo)
{
	this->m_colorInfo = colorInfo;
}

void PCS_Color_AACoder::colorDiffSolving(vector<double> &dataBeforeCompress)
{
	//�ж�color info
	assert(m_colorInfo);

	//1.����RGBͨ����������ɫ ��0-1�л��� 0-255
	separateVectorFromColorInfo();

	//2.GFT����
	VectorXcd  signalGFTTemp[3];
	MatrixXd lapMat = MatrixXd(*(spLaplacian));
	//�����Ѿ��������������
	GFT g = GFT(lapMat);
	
	for (int signal_type = 0; signal_type < 3; signal_type++)
	{
		VectorXcd  signal(mvSignalXYZ[signal_type]);
		g.gft(signal, signalGFTTemp[signal_type]);
	}

	//3.uniform quantization����
	//round  stepsize�Ĵ��� signalGFT�е����ݣ�ת����numsList�С�
	for (int i = 0; i < signalGFTTemp[0].rows(); i++)
	{
		//ʵ�����鲿������ѹ��
		// 3 * 0
		dataBeforeCompress.push_back((int)(signalGFTTemp[0](i).real() / qround + 0.5));
		dataBeforeCompress.push_back((int)(signalGFTTemp[0](i).imag() / qround + 0.5));
		// 3 * 1
		dataBeforeCompress.push_back((int)(signalGFTTemp[1](i).real() / qround + 0.5));
		dataBeforeCompress.push_back((int)(signalGFTTemp[1](i).imag() / qround + 0.5));
		// 3 * 2
		dataBeforeCompress.push_back((int)(signalGFTTemp[2](i).real() / qround + 0.5));
		dataBeforeCompress.push_back((int)(signalGFTTemp[2](i).imag() / qround + 0.5));
	}

	return;
}

void PCS_Color_AACoder::colorDiffSolvingDeCompress(vector<Vec3> & colorInfo_out)
{
	cout << "==================================" << endl;
	cout << "not finish colorDiffSolvingDeCompress ,need to add something......" << endl;
	cout << "==================================" << endl;
	//4.ѹ��
	//???????????????????????????????????????????

	//5.��ѹ��
	//???????????????????????????????????????????

	//6.inverse uniform quantization

	//7.IGFT����

	//8.�ָ��õ���ɫ
	return;
}

bool PCS_Color_AACoder::separateVectorFromVecXd()
{
	assert(inputSignal);
	//x
	mvSignalXYZ[0].resize(inputSignal->rows() / 3);
	//y
	mvSignalXYZ[1].resize(inputSignal->rows() / 3);
	//z
	mvSignalXYZ[2].resize(inputSignal->rows() / 3);

	for (int i = 0; i < inputSignal->rows() / 3; i++)
	{
		mvSignalXYZ[0](i) = (*inputSignal)(3 * i);
		mvSignalXYZ[1](i) = (*inputSignal)(3 * i + 1);
		mvSignalXYZ[2](i) = (*inputSignal)(3 * i + 2);
	}
	return true;
}

bool PCS_Color_AACoder::separateVectorFromColorInfo()
{
//#define COLOR_0_1
#define COLOR_0_255

	assert(m_colorInfo);
	//r
	mvSignalXYZ[0].resize(m_colorInfo->size());
	//g
	mvSignalXYZ[1].resize(m_colorInfo->size());
	//b
	mvSignalXYZ[2].resize(m_colorInfo->size());

	for (int i = 0; i < m_colorInfo->size(); i++)
	{
#ifdef COLOR_0_255
		mvSignalXYZ[0](i) = (*m_colorInfo)[i].x * 255;
		mvSignalXYZ[1](i) = (*m_colorInfo)[i].y * 255;
		mvSignalXYZ[2](i) = (*m_colorInfo)[i].z * 255;
#endif // COLOR_0_255

#ifdef COLOR_0_1
		mvSignalXYZ[0](i) = (*m_colorInfo)[i].x;
		mvSignalXYZ[1](i) = (*m_colorInfo)[i].y;
		mvSignalXYZ[2](i) = (*m_colorInfo)[i].z;
#endif // COLOR_0_255
	}
	return true;
}

void ProbabilityAAC::setParam(double b, double u )
{
	u_ = u;
	b_ = b;
}

double ProbabilityAAC::getCDF(double x) const
{
	if (x < u_)
	{
		return 1/2 * exp(-(u_ - x) / b_);
	}
	else
	{
		return 1 - 1 / 2 * exp(-(x - u_) / b_);
	}
}
