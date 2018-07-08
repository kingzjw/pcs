#include "zjw_pcsRLGR.h"

PCS_RLGR::PCS_RLGR(VectorXd * mvSignal, Eigen::SparseMatrix<double>* lapMat)
{
	//RLGR论文中有提到  stepSize deta from 0.01 - 1 范围。越低，保真度越高。
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

	//mv信号通过gft处理成gft信号
	VectorXcd  signal(*mvSignal);
	VectorXcd  signalGFT;
	MatrixXd lapMat = MatrixXd(*(spLaplacian));
	GFT g = GFT(lapMat);
	g.gft(signal, signalGFT);

	
#ifdef  ZJW_DEBUG
	cout << "start encode signal of gft and decode get the signal ....." << endl;
#endif //  ZJW_DEBUG

	//round  stepsize的处理
	//signalGFT中的内容，转化到numsList中。然后gft信号经过  rlgb处理成 压缩内容。存储在文件中
	for (int i = 0; i < signalGFT.rows(); i++)
	{
		//实部和虚部都进行压缩
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

	//解压数据，恢复成gft信号。解压之后数据再resList中了
	RLGR rlgr2 = RLGR(&inputList, &resList);
	rlgr2.decode();

#ifdef  ZJW_DEBUG
	cout << "start inverse quantization and igft  ....." << endl;
#endif //  ZJW_DEBUG

	//解压数据，吸纳打包成规定gft signal 的形式，然后gft解析成 原始mv信号
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

	//f_Result的result可能已经从系数变成复数的形式了
	//可能需要进行处理。


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

	//mv信号通过gft处理成gft信号
	VectorXcd  signal(*mvSignal);
	VectorXcd  signalGFT;
	MatrixXd lapMat = MatrixXd(*(spLaplacian));
	GFT g = GFT(lapMat);
	g.gft(signal, signalGFT);


#ifdef  ZJW_DEBUG
	cout << "start encode signal of gft and decode get the signal ....." << endl;
#endif //  ZJW_DEBUG

	//round  stepsize的处理
	//signalGFT中的内容，转化到numsList中。然后gft信号经过  rlgb处理成 压缩内容。存储在文件中
	for (int i = 0; i < signalGFT.rows(); i++)
	{
		//实部和虚部都进行压缩
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

	//解压数据，恢复成gft信号。解压之后数据再resList中了
	RLGR rlgr2 = RLGR(&inputList, &resList);
	rlgr2.decode();

#ifdef  ZJW_DEBUG
	cout << "start inverse quantization and igft  ....." << endl;
#endif //  ZJW_DEBUG

	//解压数据，吸纳打包成规定gft signal 的形式，然后gft解析成 原始mv信号
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

	//f_Result的result可能已经从系数变成复数的形式了
	//可能需要进行处理。
	
#ifdef  ZJW_DEBUG
	cout << "end RLGR motion vector compress !!" << endl;
	cout << "=========================================" << endl;
#endif //  ZJW_DEBUG

	return mvDecodeSignal;
	return VectorXcd();
}
