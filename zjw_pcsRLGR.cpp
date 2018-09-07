#include "zjw_pcsRLGR.h"

PCS_RLGR::PCS_RLGR(VectorXd * mvSignal, Eigen::SparseMatrix<double>* lapMat)
{
	//RLGR���������ᵽ  stepSize deta from 0.01 - 1 ��Χ��Խ�ͣ������Խ�ߡ�
	stepSize = 0.001;

	this->mvSignal = mvSignal;
	this->spLaplacian = lapMat;
}

PCS_RLGR::~PCS_RLGR()
{

}

void PCS_RLGR::rlgr_mv_compress()
{
	inputList.clear();
	separateMotionVector();

#ifdef  ZJW_DEBUG
	cout << "=========================================" << endl;
	cout << "start RLGR motion vector compress ...." << endl;
	cout << "start gft (include the compute eigen vector )..." << endl;
#endif //  ZJW_DEBUG

	//mv�źŵ�x,y,z�����źţ��ֱ�ͨ��gft�����gft�źţ�Ȼ��ϲ�
	MatrixXd lapMat = MatrixXd(*(spLaplacian));
	GFT g = GFT(lapMat);
	//---�����źŵĽ��
	//VectorXcd  signalGFTTotal;
	VectorXcd  signalGFTTemp[3];
	for (int signal_type = 0; signal_type < 3; signal_type++)
	{
		VectorXcd  signal(mvSignalXYZ[signal_type]);
		g.gft(signal, signalGFTTemp[signal_type]);
	}

	//round  stepsize�Ĵ��� signalGFT�е����ݣ�ת����numsList�С�
	for (int i = 0; i < mvSignal->rows() / 3; i++)
	{
		/*signalGFTTotal(i * 3 + 0) = signalGFTTemp[0](i);
		signalGFTTotal(i * 3 + 1) = signalGFTTemp[1](i);
		signalGFTTotal(i * 3 + 2) = signalGFTTemp[2](i);*/


		//ʵ�����鲿������ѹ��
		// 3 * 0
		inputList.push_back((int)(signalGFTTemp[0](i).real() / stepSize + 0.5));
		inputList.push_back((int)(signalGFTTemp[0](i).imag() / stepSize + 0.5));
		// 3 * 1
		inputList.push_back((int)(signalGFTTemp[1](i).real() / stepSize + 0.5));
		inputList.push_back((int)(signalGFTTemp[1](i).imag() / stepSize + 0.5));
		// 3 * 2
		inputList.push_back((int)(signalGFTTemp[2](i).real() / stepSize + 0.5));
		inputList.push_back((int)(signalGFTTemp[2](i).imag() / stepSize + 0.5));
	}

	//������
	PCS_RLGR::positiveNum(inputList, inputList_u);

#ifdef  ZJW_DEBUG
	cout << "start encode signal of gft and decode get the signal ....." << endl;
#endif //  ZJW_DEBUG

	
	//Ȼ��gft�źž���  rlgb����� ѹ�����ݡ��洢���ļ���
	//rlgr encode and compress to the file
	RLGR rlgr = RLGR(&inputList_u, &resList_u);
	rlgr.encode();
	
#ifdef  ZJW_DEBUG
	cout << "end RLGR motion vector compress !!" << endl;
	cout << "=========================================" << endl;
#endif //  ZJW_DEBUG
}

VectorXcd PCS_RLGR::rlgr_mv_decompress()
{
	resList.clear();
#ifdef  ZJW_DEBUG
	cout << "=========================================" << endl;
	cout << "start RLGR motion vector decompress ...." << endl;
	cout << "start RLGR decoding ....." << endl;
#endif //  ZJW_DEBUG

	//��ѹ���ݣ��ָ���gft�źš���ѹ֮��������resList����
	RLGR rlgr2 = RLGR(&inputList_u, &resList_u);
	rlgr2.decode();
	//�ظ��ɸ���
	PCS_RLGR::restoreNum(resList_u, resList);
#ifdef  ZJW_DEBUG
	cout << "start inverse quantization and igft  ....." << endl;
#endif //  ZJW_DEBUG

	//��ѹ���ݣ����ɴ���ɹ涨gft signal ����ʽ��Ȼ��gft������ ԭʼmv�ź�
	VectorXcd  gftDecodeSignal[3];
	//x  2��ʾʵ�����鲿��3��ʾ�����ź�
	gftDecodeSignal[0].resize(resList.size() / 2 / 3);
	//y
	gftDecodeSignal[1].resize(resList.size() / 2 / 3);
	//z
	gftDecodeSignal[2].resize(resList.size() / 2 / 3);

	for (int i = 0; i < gftDecodeSignal[0].size(); i++)
	{
		gftDecodeSignal[0](i) = complex<double>(resList[(i * 3 + 0) * 2] * stepSize, resList[(i * 3 + 0) * 2 + 1] * stepSize);
		gftDecodeSignal[1](i) = complex<double>(resList[(i * 3 + 1) * 2] * stepSize, resList[(i * 3 + 1) * 2 + 1] * stepSize);
		gftDecodeSignal[2](i) = complex<double>(resList[(i * 3 + 2) * 2] * stepSize, resList[(i * 3 + 2) * 2 + 1] * stepSize);
	}

	//��ţ�igft�����������źŽ��
	VectorXcd mvDecodeSignal[3];
	MatrixXd lapMat(*spLaplacian);
	GFT g = GFT(lapMat);
	
	//x��y,z�����ź�
	for (int i = 0; i < 3; i++)
	{
		g.igft(gftDecodeSignal[i], mvDecodeSignal[i]);	
	}

	//�ϲ�����xyz�źţ�f_Result��result�����Ѿ���ϵ����ɸ�������ʽ��
	VectorXcd mvDecodeResult;
	mvDecodeResult.resize(mvDecodeSignal[0].rows() * 3);

	for (int i = 0; i < mvDecodeSignal[0].rows(); i++)
	{
		mvDecodeResult(3 * i + 0) = mvDecodeSignal[0](i);
		mvDecodeResult(3 * i + 1) = mvDecodeSignal[1](i);
		mvDecodeResult(3 * i + 2) = mvDecodeSignal[2](i);
	}
	
#ifdef  ZJW_DEBUG
	cout << "end RLGR motion vector compress !!" << endl;
	cout << "=========================================" << endl;
#endif //  ZJW_DEBUG

	return mvDecodeResult;
}

VectorXcd PCS_RLGR::testPCS_RLGR()
{
	inputList.clear();
	separateMotionVector();

#ifdef  ZJW_DEBUG
	cout << "=========================================" << endl;
	cout << "start RLGR motion vector compress ...." << endl;
	cout << "start gft (include the compute eigen vector )..." << endl;
#endif //  ZJW_DEBUG

	//mv�źŵ�x,y,z�����źţ��ֱ�ͨ��gft�����gft�źţ�Ȼ��ϲ�
	MatrixXd lapMat = MatrixXd(*(spLaplacian));
	GFT g = GFT(lapMat);
	//---�����źŵĽ��
	//VectorXcd  signalGFTTotal;
	VectorXcd  signalGFTTemp[3];
	for (int signal_type = 0; signal_type < 3; signal_type++)
	{
		VectorXcd  signal(mvSignalXYZ[signal_type]);
		g.gft(signal, signalGFTTemp[signal_type]);
	}

	//round  stepsize�Ĵ��� signalGFT�е����ݣ�ת����numsList�С�
	for (int i = 0; i < mvSignal->rows() / 3; i++)
	{
		/*signalGFTTotal(i * 3 + 0) = signalGFTTemp[0](i);
		signalGFTTotal(i * 3 + 1) = signalGFTTemp[1](i);
		signalGFTTotal(i * 3 + 2) = signalGFTTemp[2](i);*/


		//ʵ�����鲿������ѹ��
		// 3 * 0
		inputList.push_back((int)(signalGFTTemp[0](i).real() / stepSize + 0.5));
		inputList.push_back((int)(signalGFTTemp[0](i).imag() / stepSize + 0.5));
		// 3 * 1
		inputList.push_back((int)(signalGFTTemp[1](i).real() / stepSize + 0.5));
		inputList.push_back((int)(signalGFTTemp[1](i).imag() / stepSize + 0.5));
		// 3 * 2
		inputList.push_back((int)(signalGFTTemp[2](i).real() / stepSize + 0.5));
		inputList.push_back((int)(signalGFTTemp[2](i).imag() / stepSize + 0.5));
	}
	//������
	PCS_RLGR::positiveNum(inputList, inputList_u);

#ifdef  ZJW_DEBUG
	cout << "start encode signal of gft and decode get the signal ....." << endl;
#endif //  ZJW_DEBUG


	//Ȼ��gft�źž���  rlgb����� ѹ�����ݡ��洢���ļ���
	//rlgr encode and compress to the file
	RLGR rlgr = RLGR(&inputList_u, &resList_u);
	rlgr.encode();

#ifdef  ZJW_DEBUG
	cout << "end RLGR motion vector compress !!" << endl;
	cout << "=========================================" << endl;
#endif //  ZJW_DEBUG


	resList.clear();
#ifdef  ZJW_DEBUG
	cout << "=========================================" << endl;
	cout << "start RLGR motion vector decompress ...." << endl;
	cout << "start RLGR decoding ....." << endl;
#endif //  ZJW_DEBUG

	//��ѹ���ݣ��ָ���gft�źš���ѹ֮��������resList����
	RLGR rlgr2 = RLGR(&inputList_u, &resList_u);
	rlgr2.decode();
	//�ظ��ɸ���
	PCS_RLGR::restoreNum(resList_u, resList);

#ifdef  ZJW_DEBUG
	cout << "start inverse quantization and igft  ....." << endl;
#endif //  ZJW_DEBUG

	//��ѹ���ݣ����ɴ���ɹ涨gft signal ����ʽ��Ȼ��gft������ ԭʼmv�ź�
	VectorXcd  gftDecodeSignal[3];
	//x  2��ʾʵ�����鲿��3��ʾ�����ź�
	gftDecodeSignal[0].resize(resList.size() / 2 / 3);
	//y
	gftDecodeSignal[1].resize(resList.size() / 2 / 3);
	//z
	gftDecodeSignal[2].resize(resList.size() / 2 / 3);

	for (int i = 0; i < gftDecodeSignal[0].size(); i++)
	{
		gftDecodeSignal[0](i) = complex<double>(resList[(i * 3 + 0) * 2] * stepSize, resList[(i * 3 + 0) * 2 + 1] * stepSize);
		gftDecodeSignal[1](i) = complex<double>(resList[(i * 3 + 1) * 2] * stepSize, resList[(i * 3 + 1) * 2 + 1] * stepSize);
		gftDecodeSignal[2](i) = complex<double>(resList[(i * 3 + 2) * 2] * stepSize, resList[(i * 3 + 2) * 2 + 1] * stepSize);
	}

	//��ţ�igft�����������źŽ��
	VectorXcd mvDecodeSignal[3];

	//x��y,z�����ź�
	for (int i = 0; i < 3; i++)
	{
		g.igft(gftDecodeSignal[i], mvDecodeSignal[i]);
	}

	//�ϲ�����xyz�źţ�f_Result��result�����Ѿ���ϵ����ɸ�������ʽ��
	VectorXcd mvDecodeResult;
	mvDecodeResult.resize(mvDecodeSignal[0].rows() * 3);

	for (int i = 0; i < mvDecodeSignal[0].rows(); i++)
	{
		mvDecodeResult(3 * i + 0) = mvDecodeSignal[0](i);
		mvDecodeResult(3 * i + 1) = mvDecodeSignal[1](i);
		mvDecodeResult(3 * i + 2) = mvDecodeSignal[2](i);
	}

#ifdef  ZJW_DEBUG
	cout << "end RLGR motion vector compress !!" << endl;
	cout << "=========================================" << endl;
#endif //  ZJW_DEBUG

	return mvDecodeResult;
}

void PCS_RLGR::positiveNum(vector<int>& sourceDataNegativeList)
{
	//RLGR�е����������ᵽ����δ�����и��������ݡ�
	for (int i = 0; i < sourceDataNegativeList.size(); i++)
	{
		if (sourceDataNegativeList[i] >= 0)
		{
			sourceDataNegativeList[i] *= 2;
		}
		else
		{
			sourceDataNegativeList[i] *= (-2);
			sourceDataNegativeList[i] -= -1;

		}
	}
}

void PCS_RLGR::restoreNum(vector<int>& resDecodeData)
{
	//RLGR�е����������ᵽ����δ�����и��������ݡ�
	for (int i = 0; i < resDecodeData.size(); i++)
	{
		if (resDecodeData[i] % 2 == 0)
		{
			resDecodeData[i] /= 2;
		}
		else
		{
			resDecodeData[i] += 1;
			resDecodeData[i] /= -2;
		}
	}
}

void PCS_RLGR::positiveNum(vector<int>& sourceDataNegativeList, vector<uint64_t>& codeData_out)
{
	//RLGR�е����������ᵽ����δ�����и��������ݡ�
	codeData_out.clear();
	for (int i = 0; i < sourceDataNegativeList.size(); i++)
	{
		if (sourceDataNegativeList[i] >= 0)
		{
			codeData_out.push_back(sourceDataNegativeList[i] * 2);
		}
		else
		{
			codeData_out.push_back(sourceDataNegativeList[i] * (-2) - 1);
		}
	}
}

void PCS_RLGR::restoreNum(vector<uint64_t>& sourceDecodeData, vector<int>& resDecodeData_out)
{
	resDecodeData_out.clear();
	//RLGR�е����������ᵽ����δ�����и��������ݡ�
	for (int i = 0; i < sourceDecodeData.size(); i++)
	{
		if (sourceDecodeData[i] % 2 == 0)
		{
			resDecodeData_out.push_back(sourceDecodeData[i] / 2);
		}
		else
		{
			resDecodeData_out.push_back((sourceDecodeData[i] + 1) / -2);
		}
	}
}


bool PCS_RLGR::separateMotionVector()
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