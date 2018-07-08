#include "zjw_pcsRLGR.h"

PCS_RLGR::PCS_RLGR(VectorXd * mvSignal, Eigen::SparseMatrix<double>* lapMat)
{
	//RLGR���������ᵽ  stepSize deta from 0.01 - 1 ��Χ��Խ�ͣ������Խ�ߡ�
	stepSize = 0.01;

	this->mvSignal = mvSignal;
	this->spLaplacian = spLaplacian;
}

PCS_RLGR::~PCS_RLGR()
{

}

void PCS_RLGR::rlgr_mv_compress()
{
	inputList.clear();

#ifdef  ZJW_DEBUG
	cout << "=========================================" << endl;
	cout << "start RLGR motion vector compress ...." << endl;
	cout << "start gft (include the compute eigen vector )..." << endl;
#endif //  ZJW_DEBUG

	//mv�ź�ͨ��gft�����gft�ź�
	VectorXcd  signal(*mvSignal);
	VectorXcd  signalGFT;
	MatrixXd lapMat = MatrixXd(*(spLaplacian));
	GFT g = GFT(lapMat);
	g.gft(signal, signalGFT);

	
#ifdef  ZJW_DEBUG
	cout << "start encode signal of gft and decode get the signal ....." << endl;
#endif //  ZJW_DEBUG

	//round  stepsize�Ĵ���
	//signalGFT�е����ݣ�ת����numsList�С�Ȼ��gft�źž���  rlgb����� ѹ�����ݡ��洢���ļ���
	for (int i = 0; i < signalGFT.rows(); i++)
	{
		//ʵ�����鲿������ѹ��
		inputList.push_back(floor(signalGFT[i].real() / stepSize + 0.5));
		inputList.push_back(floor(signalGFT[i].imag() / stepSize + 0.5));
	}

	//rlgr encode and compress to the file
	RLGR rlgr = RLGR(&inputList, &resList);
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
	RLGR rlgr2 = RLGR(&inputList, &resList);
	rlgr2.decode();

#ifdef  ZJW_DEBUG
	cout << "start inverse quantization and igft  ....." << endl;
#endif //  ZJW_DEBUG

	//��ѹ���ݣ����ɴ���ɹ涨gft signal ����ʽ��Ȼ��gft������ ԭʼmv�ź�
	VectorXcd  gftDecodeSignal;
	gftDecodeSignal.resize(resList.size() / 2);
	for (int i = 0; i < gftDecodeSignal.size(); i++)
	{
		complex<double> temp(resList[i * 2] * stepSize, resList[i * 2 + 1] *stepSize);
		gftDecodeSignal[i] = temp;
	}
	VectorXcd mvDecodeSignal;
	MatrixXd lapMat(*spLaplacian);
	GFT g = GFT(lapMat);
	g.igft(gftDecodeSignal, mvDecodeSignal);

	//f_Result��result�����Ѿ���ϵ����ɸ�������ʽ��
	//������Ҫ���д���


#ifdef  ZJW_DEBUG
	cout << "end RLGR motion vector compress !!" << endl;
	cout << "=========================================" << endl;
#endif //  ZJW_DEBUG

	return mvDecodeSignal;
}

VectorXcd PCS_RLGR::testPCS_RLGR()
{
	inputList.clear();

#ifdef  ZJW_DEBUG
	cout << "=========================================" << endl;
	cout << "start RLGR motion vector compress ...." << endl;
	cout << "start gft (include the compute eigen vector )..." << endl;
#endif //  ZJW_DEBUG

	//mv�ź�ͨ��gft�����gft�ź�
	VectorXcd  signal(*mvSignal);
	VectorXcd  signalGFT;
	MatrixXd lapMat = MatrixXd(*(spLaplacian));
	GFT g = GFT(lapMat);
	g.gft(signal, signalGFT);


#ifdef  ZJW_DEBUG
	cout << "start encode signal of gft and decode get the signal ....." << endl;
#endif //  ZJW_DEBUG

	//round  stepsize�Ĵ���
	//signalGFT�е����ݣ�ת����numsList�С�Ȼ��gft�źž���  rlgb����� ѹ�����ݡ��洢���ļ���
	for (int i = 0; i < signalGFT.rows(); i++)
	{
		//ʵ�����鲿������ѹ��
		inputList.push_back(floor(signalGFT[i].real() / stepSize + 0.5));
		inputList.push_back(floor(signalGFT[i].imag() / stepSize + 0.5));
	}

	//rlgr encode and compress to the file
	RLGR rlgr = RLGR(&inputList, &resList);
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
	RLGR rlgr2 = RLGR(&inputList, &resList);
	rlgr2.decode();

#ifdef  ZJW_DEBUG
	cout << "start inverse quantization and igft  ....." << endl;
#endif //  ZJW_DEBUG

	//��ѹ���ݣ����ɴ���ɹ涨gft signal ����ʽ��Ȼ��gft������ ԭʼmv�ź�
	VectorXcd  gftDecodeSignal;
	gftDecodeSignal.resize(resList.size() / 2);
	for (int i = 0; i < gftDecodeSignal.size(); i++)
	{
		complex<double> temp(resList[i * 2] * stepSize, resList[i * 2 + 1] * stepSize);
		gftDecodeSignal[i] = temp;
	}
	VectorXcd mvDecodeSignal;
	//MatrixXd lapMat(*spLaplacian);
	//GFT g = GFT(lapMat);
	g.igft(gftDecodeSignal, mvDecodeSignal);

	//f_Result��result�����Ѿ���ϵ����ɸ�������ʽ��
	//������Ҫ���д���
	
#ifdef  ZJW_DEBUG
	cout << "end RLGR motion vector compress !!" << endl;
	cout << "=========================================" << endl;
#endif //  ZJW_DEBUG

	return mvDecodeSignal;
	return VectorXcd();
}
