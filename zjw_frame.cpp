#include "zjw_frame.h"

//Frame::Frame()
//{
//	objMesh = new ObjMesh();
//	pcsOct = new PcsOctree();
//}

Frame::Frame(const int frameId, int _m, int _Nscales)
{
	objMesh = new ObjMesh();
	pcsOct = new PcsOctree(_m, _Nscales);
}

Frame::~Frame()
{
	if (objMesh)
		delete objMesh;
	if (pcsOct)
		delete pcsOct;
}

bool Frame::loadObj(string path)
{
	//return objMesh.loadObjMesh(path);
	return objMesh->loadObjMesh(path);
}

bool Frame::octSgwt(Vec3 cellSize)
{
#ifdef ZJW_TIMER
	ZjwTimer timer2;
	timer2.Start();
#endif

	Vec3 minPos(objMesh->rangeMin);
	Vec3 maxPos((objMesh->rangeMax + Epsilon));
	/*Vec3 minPos(objMesh.rangeMin);
	Vec3 maxPos((objMesh.rangeMax + Epsilon));*/

	//Vec3 cellSize(0.1);
	//设置参数，并构建八叉树
	pcsOct->setParam(minPos, maxPos, cellSize);
	//pcsOct->buildPcsOctFrmPC(objMesh);
	pcsOct->buildPcsOctFrmPC(objMesh);
	//得到叶子节点的边界，并保存相关的信息
	pcsOct->getLeafboundary();

#ifdef RELATIVE_DIS_SIGNAL
	pcsOct->changeNodePointsToRelativeDis();
#endif // RELATIVE_DIS_SIGNAL

	pcsOct->getGraphMat();
	pcsOct->getMatEigenVerValue();

	//得到八叉树上顶点的信号
	pcsOct->setPointTo8Areas();
	pcsOct->getLeafSignal();

	pcsOct->getAllSignalAndSaveSGWTCoeff();

#ifdef ZJW_TIMER
	timer2.Stop();
	timer2.printTimeInMs("build oct and ready to compute the sgwt!");
#endif
	return true;
}

FrameManage::FrameManage()
{
	//下面的参数，以后可以通过界面进行更改。
	opcCompress = new OctreePointCloudCompressionZjw(compression_Profiles_e::MANUAL_CONFIGURATION, true, 0.001, 0.1, false, 30, false, 6);

	batchLoad = false;
	getAllFilePath = false;
	//changeSouceData = true;

	fileBatch = nullptr;
	//oct 中用到的参数
	cellSize.x = cellSize.y = cellSize.z = 0.05;
	clusterNum = 10;
	P = new MatrixXd;

	u = 0.01;
	m=50;
	//sclae的个数，不包括h
	Nscales=4;
}

FrameManage::~FrameManage()
{
	if (opcCompress)
		delete opcCompress;

	if (P)
		delete P;

	if (fileBatch)
		delete fileBatch;
}

void FrameManage::switchSouceData()
{
	batchLoad = false;
	getAllFilePath = false;
	fileBatch = nullptr;
}

void FrameManage::getAllFilesPath(FileNameForMat type, string fileNameFormat, string path)
{
	//--------获取该路径下的所有文件-------------
	if (fileBatch)
		delete fileBatch;
	fileBatch = new FileBatch(path, fileNameFormat);
	fileBatch->fileNum = fileBatch->getFilesNum(fileBatch->filePath);
	frameList.clear();
	getAllFilePath = true;

	for (int f_it = 0; f_it < fileBatch->fileNum; f_it++)
	{
		//拿到所有的路径
		string totalFilePath;
		stringstream ss;
		ss << f_it;

		switch (type)
		{
		case FrameManage::NUM_FRONT:
			totalFilePath.assign(path).append("/").append(ss.str()).append(fileNameFormat).append(".obj");
			break;
		case FrameManage::NUM_TAIL:
			totalFilePath.assign(path).append("/").append(fileNameFormat).append(ss.str()).append(".obj");
			break;
		default:
			break;
		}

		//按照需要从小到大，保存所有的文件路径
		fileBatch->files.push_back(totalFilePath);

		//拿到所有的frameList
		Frame * frame = new Frame(f_it,m,Nscales);
		frameList.push_back(frame);
	}
}

void FrameManage::batchLoadObj(FileNameForMat type, string fileNameFormat, string path)
{
	if (fileBatch)
		delete fileBatch;
	fileBatch = new FileBatch(path, fileNameFormat);
	fileBatch->fileNum = fileBatch->getFilesNum(fileBatch->filePath);
	frameList.clear();
	batchLoad = true;

	//--------获取该路径下的所有文件-------------
	for (int f_it = 0; f_it < fileBatch->fileNum; f_it++)
	{
#ifdef ZJW_DEBUG
		cout << endl << endl << "##################################################" << endl;
#endif
		string totalFilePath;
		stringstream ss;
		ss << f_it;

		switch (type)
		{
		case FrameManage::NUM_FRONT:
			totalFilePath.assign(path).append("/").append(ss.str()).append(fileNameFormat).append(".obj");
			break;
		case FrameManage::NUM_TAIL:
			totalFilePath.assign(path).append("/").append(fileNameFormat).append(ss.str()).append(".obj");
			break;
		default:
			break;
		}

		//按照需要从小到大，保存所有的文件路径
		fileBatch->files.push_back(totalFilePath);

		Frame * frame = new Frame(f_it, m, Nscales);
		frameList.push_back(frame);

		//-----------对每个路径，都进行读取，和sgw操作------------
		ZjwTimer timer;
		timer.Start();
		if (!frameList[f_it]->objMesh->loadObjMeshSimply(totalFilePath))
		{
			continue;
		}
		//frameList[f_it-1].objMesh->loadObjMeshSimply(totalFilePath);
		timer.Stop();
		timer.printTimeInMs("load obj time: ");

		frameList[f_it]->octSgwt(cellSize);
		//frameList[f_it-1].octSgwt();

#ifdef ZJW_DEBUG
		//cout << totalFilePath << endl;
#endif //zjw_debug
	}
}

bool FrameManage::loadContinuousFrames(int frameId1, int frameId2, FileNameForMat type, string fileNameFormat, string path, bool changeData)
{
	assert(frameId1 > -1 && frameId2 > -1);
	assert(abs(frameId1 - frameId2) == 1);

	if (changeData)
	{
		switchSouceData();
	}

	if (batchLoad)
		return true;

	if (!getAllFilePath)
	{
		getAllFilesPath(type, fileNameFormat, path);
	}

	assert(frameId1 < fileBatch->fileNum && frameId2 < fileBatch->fileNum);

	//-----------进行读取，和sgw操作------------
	ZjwTimer timer;
	timer.Start();
#ifdef ZJW_DEBUG
	cout << endl << endl << "###################################################################" << endl << path << endl;
#endif
	if (frameList[frameId1]->objMesh->loadObjMeshSimply(fileBatch->files[frameId1]))
	{
		timer.Stop();
		timer.printTimeInMs("load obj time: ");

		frameList[frameId1]->octSgwt(cellSize);
	}

	ZjwTimer timer3;
	timer3.Start();
#ifdef ZJW_DEBUG
	cout << endl << endl << "###################################################################" << endl << path << endl;
#endif
	if (frameList[frameId2]->objMesh->loadObjMeshSimply(fileBatch->files[frameId2]))
	{
		timer3.Stop();
		timer3.printTimeInMs("load obj time: ");

		frameList[frameId2]->octSgwt(cellSize);
	}
	return true;
}

bool FrameManage::matchNode(int frameId1, int frameId2, vector<int>* f1nIdxList, vector<int>* f2nIdxList)
{
	assert(abs(frameId1 - frameId2) == 1);
	f1nIdxList->clear();
	f2nIdxList->clear();

	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId2];

	//遍历frame1中所有的nodelist
	for (int leaf_it = 0; leaf_it < frame1->pcsOct->ctLeaf->nodeList.size(); leaf_it++)
	{
		//frame1: 计算每个node中，点关于node相对于中点的距离成反比的权重。
		Node * node1 = &(frame1->pcsOct->ctLeaf->nodeList[leaf_it]->nodeData);
		Vec3 midPoint = (*frame1->pcsOct->ctLeaf->midVList)[leaf_it];
		vector<float> wightList;
		double totalWight = 0;
		double tempW;
		//遍历这个叶子节点上所有的点
		for (int p_it = 0; p_it < node1->pointIdxList.size(); p_it++)
		{
#ifdef RELATIVE_DIS_SIGNAL
			//利用相对距离
			tempW = 1 / (node1->pointPosList[p_it]+ frame1->pcsOct->ctLeaf->minVList[leaf_it]).Distance(midPoint);
#else
			//绝对距离
			tempW = 1 / node1->pointPosList[p_it].Distance(midPoint);
#endif //relative dis signal
			totalWight += tempW;
			wightList.push_back(tempW);
		}

		//利用fram2对应点的位置，根据权重预计出对应Node的中点位置
		Vec3 predictNodePos;
		int idx;
		//Node * node2 = &(frame2->pcsOct->ctLeaf->nodeList[leaf_it]->nodeData);
		for (int p_it = 0; p_it < node1->pointIdxList.size(); p_it++)
		{
			//拿到frame1节点中点的序号，在frame2中的点的值
			idx = node1->pointIdxList[p_it];
			predictNodePos += wightList[p_it] * frame2->objMesh->vertexList[idx];
		}
		predictNodePos = predictNodePos / totalWight;

		//根据预计出来的中点位置,通过在frame2上的八叉树，然后得到匹配的node结果
		//先用for 循环实现，后面改用八叉树快速判断
		int macthNodeIdx = -1;
		frame2->pcsOct->judegePointToLeafNode((Vec3*)&predictNodePos, macthNodeIdx);

		if (macthNodeIdx < 0)
		{
			//因为匹配失败了，所以可以不作为训练矩阵的P的样本值。
#ifdef ZJW_PRINT_INFO
			cout << "info: matchNode function: frame1 node " << leaf_it << " match faied!!" << endl;
#endif
		}
		else
		{
			f1nIdxList->push_back(leaf_it);
			f2nIdxList->push_back(macthNodeIdx);
		}
	}
	return true;
}

//对f1nIdxList中记录的所有的对应关系的误差向量都进行了计算
//返回向量是P
bool FrameManage::getMatrixP(int frameId1, int frameId2, vector<int>* f1nIdxList, vector<int>* f2nIdxList, MatrixXd * p_out)
{
	//判断是连续的两帧
	assert(abs(frameId1 - frameId2) == 1);
	assert(f1nIdxList->size() > 0 && f2nIdxList->size() > 0);

	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId2];
	//原来是== ,但是会出现匹配失败的情况
	assert(frame1->pcsOct->ctLeaf->nodeList.size() >= f1nIdxList->size());

	//------------------计算每对对应Node之间的误差向量，并保存矩阵sample中-------------------
	int totalScale = 5;
	int totalSignal = 6;
	int totalQuadrant = 8;

	MatrixXd sample;
	sample.resize(f1nIdxList->size(), totalScale * totalSignal * totalQuadrant);

	VectorXd featureVec1;
	VectorXd featureVec2;

	for (int it = 0; it < f1nIdxList->size(); it++)
	{
		frame1->pcsOct->getFeatureVector2((*f1nIdxList)[it], &featureVec1);
		frame2->pcsOct->getFeatureVector2((*f2nIdxList)[it], &featureVec2);
		//ensure 自变量的个数是一致的
		//assert(sample.cols() == featureVec1.rows());
		//保存这个到矩阵中,相应的行中
		sample.row(it) = featureVec1 - featureVec2;
	}

	//------------------求出sample的协方差矩阵----------------------

	Eigen::MatrixXd covMat;
	//求取列向量均值(求每一个列向量的均值)
	Eigen::MatrixXd meanVec = sample.colwise().mean();

	//将列向量均值从MatrixXf 转换为行向量 RowVectorXf
	Eigen::RowVectorXd meanVecRow(Eigen::RowVectorXd::Map(meanVec.data(), sample.cols()));

	//求取减去均值的列向量矩阵
	Eigen::MatrixXd zeroMeanMat = sample;
	//每一行的样本（有几列就是几个自变量），都减去自身的均值
	zeroMeanMat.rowwise() -= meanVecRow;

	if (sample.rows() == 1)
	{
		//计算协方差
		covMat = (zeroMeanMat.adjoint()*zeroMeanMat) / double(sample.rows());
	}
	else
	{
		//计算协方差
		covMat = (zeroMeanMat.adjoint()*zeroMeanMat) / double(sample.rows() - 1);
	}

	//---------------对协方差矩阵进行求逆，得到P----------------------
#ifdef ZJW_DEBUG
	double size = 1.00001;
	//double size = 1;
	cout << "对covMat的矩阵对角线上元素放大了： " << size << " 倍 " << endl;
	assert(covMat.rows() == covMat.cols());
	if (size > 1)
	{
		for (int i = 0; i < covMat.rows(); i++)
		{
			covMat(i, i) *= size;
		}
	}
#endif // zjw_debug

	FullPivLU<MatrixXd> lu_decomp(covMat);

	//covMat.rank()
	if ((covMat.determinant()) == 0 && (lu_decomp.rank() != covMat.rows()))
	{
		cout << "cov mat 不可逆，can not solve the inverse mat P" << endl;
#ifdef ZJW_DEBUG
		cout << "The rank of covMat is " << lu_decomp.rank() << endl;
		cout << "The size of covMat is " << covMat.rows() << " " << covMat.cols() << endl;
		//cout << "cov mat: " << endl;
		//cout << covMat << endl << endl;
		//cout << covMat.diagonal() << endl;
		//cout << covMat.colwise().maxCoeff() << endl;
		//cout << covMat.colwise().minCoeff() << endl;

		/*for (int i = 0; i < covMat.cols(); i++)
		{
			cout << "=======================================================" << endl;
			cout << i << endl;
			cout << "=======================================================" << endl;
			cout << covMat.col(i)<< endl;
		}*/
#endif // zjw_debug

		//不可逆不需要继续执行了。
		assert(0);
	}
	else
	{
		*p_out = covMat.inverse();

#ifdef ZJW_DEBUG
		cout << "cov mat 矩阵可逆!!" << endl;
		cout << "The rank of covMat is " << lu_decomp.rank() << endl;
		cout << "The size of covMat is " << covMat.rows() << " " << covMat.cols() << endl;

		//print
		//cout << covMat.diagonal() << endl;
		/*cout << "mat P: " << endl;
		cout << *p << endl << endl;*/

#endif //zjw debug
	}

	return true;
}

//测试数据：拿到frame2中每个node idx在 frame1中的最佳匹配。最佳匹配关系，从两个vector中返回。
//返回值：maDist ， f1nIdxList f2nIdxList是得到最佳的匹配关系
//f2nIdxList中的顶点序列应该是连续的，除非中间有顶点匹配失败
bool FrameManage::getBestMatchPoint(int frameId1, int frameId2, 
	MatrixXd * P, vector<int>* f1nIdxList_out, vector<int>* f2nIdxList_out, vector<double>* maDist_out)
{
#ifdef ZJW_DEBUG
	//该函数非常消耗时间，需要进度条记录进展。
	bool process = false;
#endif // ZJW_DEBUG

	//判断是连续的两帧
	assert(abs(frameId1 - frameId2) == 1);
	f1nIdxList_out->clear();
	f2nIdxList_out->clear();
	maDist_out->clear();

	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId2];

	//遍历target frame （frame2）中所有Node,找对最佳匹配的node
	for (int node2_it = 0; node2_it < frame2->pcsOct->ctLeaf->nodeList.size(); node2_it++)
	{
		//计算当前frame2 中的feature vector
		//Node * node1 = &(frame1->pcsOct->ctLeaf->nodeList[node2_it]->nodeData);
		VectorXd featureVec2;
		frame2->pcsOct->getFeatureVector2(node2_it, &featureVec2);

		//遍历frame1中所有的叶子节点,
		VectorXd featureVec1;
		vector<double> mahalanobisDist;
		mahalanobisDist.clear();
		for (int node1_it = 0; node1_it < frame1->pcsOct->ctLeaf->nodeList.size(); node1_it++)
		{
			//得到frame1中叶子节点的 fearture vector
			frame1->pcsOct->getFeatureVector2(node1_it, &featureVec1);
			//计算node2 和node1两个feature vector的马氏距离
			
			//use P 
			assert(P->rows() > 0 && P->cols() > 0);
			double maha = (featureVec1 - featureVec2).transpose() * (*P) * (featureVec1 - featureVec2);
			
			//do not use P
			//double maha = (featureVec1 - featureVec2).transpose() * (featureVec1 - featureVec2);

			//正定矩阵P
			assert(maha > 0);
			mahalanobisDist.push_back(maha);
		}

		//找到node2对应到frame1上所有节点的马氏距离，找到最佳匹配（最小）
		std::vector<double>::iterator smallest = std::min_element(std::begin(mahalanobisDist), std::end(mahalanobisDist));
		int indexNode1 = std::distance(std::begin(mahalanobisDist), smallest);

		//std::cout << "min element is " << *smallest << " at position " << std::distance(std::begin(mahalanobisDist), smallest) << std::endl;

		//保存最佳对应关系,及最佳对应关系下面的马氏距离
		f2nIdxList_out->push_back(node2_it);
		f1nIdxList_out->push_back(indexNode1);
		maDist_out->push_back(*smallest);

#ifdef ZJW_DEBUG
		//显示这个函数进度
		if (process)
		{
			printf("\b\b\b\b\b\b");
			cout.width(5);
			cout.precision(2);
			cout << ((node2_it + 1) * 100.0) / frame2->pcsOct->ctLeaf->nodeList.size() << "%";
		}
		else
		{
			cout.width(5);
			cout.precision(2);
			cout << "match process:  " << ((node2_it + 1) * 100.0) / frame2->pcsOct->ctLeaf->nodeList.size() << "%";
			process = true;
		}
#endif // ZJW_DEBUG

	}

#ifdef ZJW_DEBUG
	cout << endl;
#endif // ZJW_DEBUG

	return true;
}

//这个方法可能是存在问题的。测试数据：getBestMatchPoint函数的加速版，利用几何距离快速拒绝匹配
//f1nIdxList_out中-1表示，没有找到最佳匹配。这个时候maDis_out是double的最大值。
bool FrameManage::getBestMatchPointSpeedUp(int frameId1, int frameId2, MatrixXd * P, 
	vector<int>* f1nIdxList_out, vector<int>* f2nIdxList_out, vector<double>* maDist_out)
{
	//如果不使用加速，可以把这个宏，去掉
#define SPPED_UP

#ifdef SPPED_UP
	//几何距离的阈值。
	//node的位置在(0-1之间)
	double disThreshold = cellSize.x * 10;
	if (disThreshold > 1)
		disThreshold = 1;

	cout << "getBestMatchPointSpeedUp: disThreshold is " << disThreshold <<"  this function maybe error becasuse of absolute distance!!!"<< endl;
#endif // SPPED_UP

#ifdef ZJW_DEBUG
	ZjwTimer timer;
	timer.Start();

	//进度条记录进展。
	bool process = false;
#endif // ZJW_DEBUG

	//判断是连续的两帧
	assert(abs(frameId1 - frameId2) == 1);
	f1nIdxList_out->clear();
	f2nIdxList_out->clear();
	maDist_out->clear();

	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId2];

	//遍历target frame （frame2）中所有Node,找对最佳匹配的node
	for (int node2_it = 0; node2_it < frame2->pcsOct->ctLeaf->nodeList.size(); node2_it++)
	{
		//计算当前frame2 中的feature vector
		VectorXd featureVec2;
		frame2->pcsOct->getFeatureVector2(node2_it, &featureVec2);

#ifdef SPPED_UP
		//计算当前这个node 中心的绝对距离
		Vec3 pos2 = (*frame2->pcsOct->ctLeaf->midVList)[node2_it];
#endif // SPPED_UP

		//--------------遍历frame1中所有的叶子节点,---------
		VectorXd featureVec1;
		vector<double> mahalanobisDist;
		mahalanobisDist.clear();

#ifdef SPPED_UP
		vector<int> validFrameNodeIdxList;
		validFrameNodeIdxList.clear();
#endif // SPPED_UP

		for (int node1_it = 0; node1_it < frame1->pcsOct->ctLeaf->nodeList.size(); node1_it++)
		{

#ifdef SPPED_UP
			Vec3 pos1 = (*frame1->pcsOct->ctLeaf->midVList)[node1_it];
			if (pos1.Distance(pos2) >= disThreshold)
				continue;
#endif // SPPED_UP

			//得到frame1中叶子节点的 fearture vector
			frame1->pcsOct->getFeatureVector2(node1_it, &featureVec1);
			//计算node2 和node1两个feature vector的马氏距离

			//use P 
			assert(P->rows() > 0 && P->cols() > 0);
			double maha = (featureVec1 - featureVec2).transpose() * (*P) * (featureVec1 - featureVec2);

			//do not use P
			//double maha = (featureVec1 - featureVec2).transpose() * (featureVec1 - featureVec2);

			//正定矩阵P。mahalanobisDist记录这个node2所对应的node1中的maha
			assert(maha > 0);
			mahalanobisDist.push_back(maha);

#ifdef SPPED_UP
			validFrameNodeIdxList.push_back(node1_it);
#endif // SPPED_UP
		}

		//---------------找到node2对应到frame1上所有节点的马氏距离，找到最佳匹配（最小）-------------

		//indexNode1 表示的所有的node2在frame1上所对应的最小的下标
		std::vector<double>::iterator smallest = std::min_element(std::begin(mahalanobisDist), std::end(mahalanobisDist));
		int indexNode1 = std::distance(std::begin(mahalanobisDist), smallest);
		//std::cout << "min element is " << *smallest << " at position " << std::distance(std::begin(mahalanobisDist), smallest) << std::endl;
		
		//--------------保存最佳对应关系,及最佳对应关系下面的马氏距离-----------

#ifdef SPPED_UP
		if (validFrameNodeIdxList.empty())
		{
			////忽略这个点
			f2nIdxList_out->push_back(node2_it);
			f1nIdxList_out->push_back(-1);
			maDist_out->push_back((numeric_limits<double>::max)());
			cout << "info: target frame node " << node2_it << " do not get best match in refren frame (不影响后面的程序)" << endl;
		}
		else
		{
			f2nIdxList_out->push_back(node2_it);
			f1nIdxList_out->push_back(validFrameNodeIdxList[indexNode1]);
			maDist_out->push_back(*smallest);

		}
#else
		//不试用加速的时候
		f2nIdxList_out->push_back(node2_it);
		f1nIdxList_out->push_back(indexNode1);
		maDist_out->push_back(*smallest);
#endif // SPPED_UP


#ifdef ZJW_DEBUG
		//显示这个函数进度
		if (process)
		{
			printf("\b\b\b\b\b\b");
			cout.width(5);
			cout.precision(2);
			cout << ((node2_it + 1) * 100.0) / frame2->pcsOct->ctLeaf->nodeList.size() << "%";
		}
		else
		{
			cout.width(5);
			cout.precision(2);
			cout << "match process:  " << ((node2_it + 1) * 100.0) / frame2->pcsOct->ctLeaf->nodeList.size() << "%";
			process = true;
		}
#endif // ZJW_DEBUG

	}

#ifdef ZJW_DEBUG
	timer.Stop();
	cout << endl;
	timer.printTimeInMs("best match timer : ");
	cout << endl;
#endif // ZJW_DEBUG

	return true;
}

//传入的frameId： 是target frame的id
//f1nIdxList f2nIdxList 是传入的最佳匹配参数。f1nIdxList的传入是可能存在无效值的。
//maDist保存的是最佳匹配的距离
//返回值：f1SparseIdxList  f2SparseIdxList   稀疏的最佳匹配
bool FrameManage::doKmeansGetSparseBestMatch(int frameId, vector<int>* f1nIdxList, vector<int>* f2nIdxList,
	vector<double>* maDist, vector<int>* f1SparseIdxList_out, vector<int>* f2SparseIdxList_out)
{
	assert(frameId >= 0);
	assert(f1nIdxList->size() > 0 && f1nIdxList->size() > 0 && maDist->size() > 0);

	f1SparseIdxList_out->clear();
	f2SparseIdxList_out->clear();

	Frame* frame = frameList[frameId];
	KMeans * kmeans = frame->pcsOct->kmeans;

	//frame2(target frame) 所有的叶子节点，进行do kmeans,取出结果
	frame->pcsOct->doKmeans(clusterNum);

	//遍历每个cluster集的node
	for (int c_it = 0; c_it < kmeans->clusterRes.size(); c_it++)
	{
		//得到这个cluster中的最小maDist,保存每个区域最佳匹配的节点序号
		int bestNodeId = -1;
		double minDis = -1;
		for (int n_it = 0; n_it < kmeans->clusterRes[c_it].size(); n_it++)
		{
			//nodeIdx的范围，就是当前frame2 中叶子结点的序号范围  0 - (nodeSize-1)
			int nodeIdx = kmeans->clusterRes[c_it][n_it];
			
			//判断这个target frame 2中的Nodeidx 是否在最佳匹配的列表中(后面这个操作只要最佳匹配的列表不是连续的，就会产生消耗)
			//new way
			/*int tpIdx = nodeIdx;
			assert(tpIdx == nodeIdx);
			while ((*f2nIdxList)[tpIdx] >= nodeIdx)
			{
				if ((*f2nIdxList)[tpIdx] == nodeIdx)
				{
					if ((*maDist)[tpIdx] < minDis || minDis == -1)
					{
						bestNodeId = tpIdx;
						minDis = (*maDist)[tpIdx];
					}
				}
				tpIdx--;
			}*/
			
			//old way
			if ((*maDist)[nodeIdx] < minDis || minDis == -1)
			{
				bestNodeId = nodeIdx;
				minDis = (*maDist)[nodeIdx];
			}
		}

#ifdef  ZJW_DEBUG
		//因为getBestMatchPoint中得到的f2应该序号是从低到高排序的
		assert(bestNodeId == (*f2nIdxList)[bestNodeId]);
		//test
		cout << "sparse match : " << (*f1nIdxList)[bestNodeId] << " " << (*f2nIdxList)[bestNodeId] << endl;
		//end test
#endif //  zjw_debug

		//排除getBestMatchPoint函数中，没有找到对应最佳匹配的情况。
		if ((*f1nIdxList)[bestNodeId] == -1)
			continue;

		//保存对应关系到sparse ver idx中
		f1SparseIdxList_out->push_back((*f1nIdxList)[bestNodeId]);
		f2SparseIdxList_out->push_back((*f2nIdxList)[bestNodeId]);
	}

	return true;
}

bool FrameManage::getTwoFrameBestSparseMatch(int frameId1, int frameId2, vector<int>* f1SparseIdxList_out,
	vector<int>* f2SparseIdxList_out, FileNameForMat type, string fileNameFormat, string path, bool changeData)
{
	//path必须被指定
	assert(!path.empty());
	//if (path.empty())
	//{
	//	cout << "you need to choose a file path!!!!!!!!!" << endl;
	//	return false;
	//}

	loadContinuousFrames(frameId1, frameId2, type, fileNameFormat, path, changeData);
	//拿到frame2中每个node idx在 frame1中的最佳匹配
	//getBestMatchPoint(frameId1, frameId2, P, &f1nIdxList, &f2nIdxList, &maDist);
	getBestMatchPointSpeedUp(frameId1, frameId2, P, &f1nIdxList, &f2nIdxList, &maDist);
	//对target frame上的帧进行k-means
	doKmeansGetSparseBestMatch(frameId2, &f1nIdxList, &f2nIdxList, &maDist, f1SparseIdxList_out, f2SparseIdxList_out);
	return true;
}

bool FrameManage::trainGetP(int frameId1, int frameId2, FileNameForMat type, string fileNameFormat, string path)
{
#ifdef ZJW_DEBUG
	cout << "start to traie data for mat P ..." << endl;
#ifdef ZJW_TIMER

	ZjwTimer test;
	test.Start();
#endif //ZJW_TIMER
#endif // ZJW_DEBUG
	
	//价值训练数据
	loadContinuousFrames(frameId1, frameId2, type, fileNameFormat, path);
	//得到匹配关系
	matchNode(frameId1, frameId2, &f1TrainList, &f2TrainList);
	//得到矩阵P
	getMatrixP(frameId1, frameId2, &f1TrainList, &f2TrainList, P);

#ifdef ZJW_DEBUG
#ifdef ZJW_TIMER
	test.printTimeInMs("traing data for mat P ");
#endif //zjw_timer
	cout << "end to traie data for mat P !!!" << endl;
#endif // ZJW_DEBUG

	return true;
}

//frameId1是referen frame idx.
//Mn是frame1上的稀疏最佳匹配的下标
//N是frame2上的稀疏最佳匹配的下标
void FrameManage::getMnMat(int frameId1, int MnIdx, int NIdx, MatrixXd & MnMat_out)
{
	//判断是连续的两帧
	assert(frameId1>=0);

	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId1 + 1];
	MnMat_out.resize(3, 3);
	MnMat_out.setZero();

	//得到frame1 点 Mn为中心，two hop之间的顶点序号
	set<int>  MTwoHopList;

	frame1->pcsOct->getTwoHopNeighborhoodWithOneStep(MnIdx, &MTwoHopList, frame1->pcsOct->spLaplacian);

	//遍历所有的two hop上的顶点，计算平均的Mn矩阵
	set<int>::iterator it = MTwoHopList.begin();
	VectorXd featureVector1;
	VectorXd featureVector2;

	for (; it != MTwoHopList.end(); it++)
	{
		//two hop node m
		int M = *it;

		//σ(mn, n)的计算
		frame1->pcsOct->getFeatureVector2(MnIdx, &featureVector1);
		frame2->pcsOct->getFeatureVector2(NIdx, &featureVector2);
		double maDis_Mn_N = (featureVector1 - featureVector2).transpose() * (*P) * (featureVector1 - featureVector2);

		//σ(m, n) 
		frame1->pcsOct->getFeatureVector2(M, &featureVector1);
		frame2->pcsOct->getFeatureVector2(NIdx, &featureVector2);
		double maDis_M_N = (featureVector1 - featureVector2).transpose() * (*P) * (featureVector1 - featureVector2);

		//(pt(m) − pt(mn))
		Vec3 pM = (*(frame1->pcsOct->ctLeaf->midVList))[M];
		Vec3 pMn = (*(frame1->pcsOct->ctLeaf->midVList))[MnIdx];
		Vector3d pDif(pM.x - pMn.x, pM.y - pMn.y, pM.z - pMn.z);

		//求和 sqrt
		MnMat_out += (pDif * pDif.transpose()) / (maDis_M_N - maDis_Mn_N);
	}

	MnMat_out /= MTwoHopList.size();
}

void FrameManage::getMnMatSpeedUP(int frameId1, int MnIdx, int NIdx, MatrixXd & MnMat_out)
{
	//判断是连续的两帧
	assert(frameId1 >= 0);

	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId1 + 1];
	MnMat_out.resize(3, 3);
	MnMat_out.setZero();

	//得到frame1 点 Mn为中心，two hop之间的顶点序号
	set<int>  MTwoHopList;

	frame1->pcsOct->getTwoHopNeighborhoodWithOneStep(MnIdx, &MTwoHopList, frame1->pcsOct->spLaplacian);
	//frame1->pcsOct->getTwoHopNeighborhood(MnIdx, &MTwoHopList, frame1->pcsOct->spLaplacian);

	//遍历所有的two hop上的顶点，计算平均的Mn矩阵
	set<int>::iterator it = MTwoHopList.begin();
	VectorXd featureVectorMn;
	VectorXd featureVectorM;
	VectorXd featureVectorN;
	
	//σ(mn, n)的计算
	frame1->pcsOct->getFeatureVector2(MnIdx, &featureVectorMn);
	frame2->pcsOct->getFeatureVector2(NIdx, &featureVectorN);
	double maDis_Mn_N = (featureVectorMn - featureVectorN).transpose() * (*P) * (featureVectorMn - featureVectorN);

	Vec3 pMn = (*(frame1->pcsOct->ctLeaf->midVList))[MnIdx];

	for (; it != MTwoHopList.end(); it++)
	{
		//two hop node m
		int M = *it;
		
		//σ(m, n) 
		frame1->pcsOct->getFeatureVector2(M, &featureVectorM);
		double maDis_M_N = (featureVectorM - featureVectorN).transpose() * (*P) * (featureVectorM - featureVectorN);

		//(pt(m) − pt(mn))
		Vec3 pM = (*(frame1->pcsOct->ctLeaf->midVList))[M];
		Vector3d pDif(pM.x - pMn.x, pM.y - pMn.y, pM.z - pMn.z);

		//求和 sqrt
		MnMat_out += (pDif * pDif.transpose()) / (maDis_M_N - maDis_Mn_N);
	}

	if (MTwoHopList.size()==0)
	{
		cout << "getMnMatSpeedUP:  MnIdx " << MnIdx << " NIdx " << NIdx << endl;
		cout << "this MnIdx do not have any MTwoHopList.size()" << endl;
	}

	MnMat_out /= MTwoHopList.size();
}

void FrameManage::getQ(int frameId1, vector<int>* f1SparseIdxList, vector<int>* f2SparseIdxList, MatrixXd & Q_out)
{
	assert(frameId1>=0);
	Frame* frame1 = frameList[frameId1];
	//Frame* frame2 = frameList[frameId1 + 1];

	int Nt = frame1->pcsOct->ctLeaf->nodeList.size();
	Q_out.resize(3 * Nt, 3 * Nt);
	Q_out.setZero();

	//遍历sparse list,
	for (int node_it = 0; node_it < f1SparseIdxList->size(); node_it++)
	{
		// 拿到应该拿的Mn
		MatrixXd MnMat_out;
		int Mn = (*f1SparseIdxList)[node_it];
		getMnMatSpeedUP(frameId1, Mn, (*f2SparseIdxList)[node_it], MnMat_out);
		
#ifdef ZJW_DEBUG
		////test
		//cout << "Mn first:" << endl;
		//cout << MnMat_out << endl;
		////end test
		////test
		//cout << "Mn first inverse:" << endl;
		//cout << MnMat_out.inverse() << endl;
		//end test

		//可能需要对MnMat_out进行归一化？？？？？？？？？？？？？？？
		MnMat_out.normalize();
		cout << "getQ: Mn Mat 矩阵这里进行了归一化" << endl;

		//test
		/*cout << "normal:" << endl;
		cout << MnMat_out << endl;*/
		//end test

#endif //zje_debug

		//Mn 矩阵求逆
		MnMat_out = MnMat_out.inverse();
		if (MnMat_out.determinant() == 0)
		{
			cout << "this Mn mat 是奇异的，不能求逆。但是求逆了。(求逆之后的结果)";
		}
		//test
		/*cout << "inverse:" << endl;
		cout << MnMat_out << endl;*/
		//end test

		//把Mn赋值到Q中对应的位置上
		for (int col_it = 0; col_it < MnMat_out.cols(); col_it++)
		{
			for (int row_it=0; row_it < MnMat_out.rows(); row_it++)
			{
				Q_out(Mn * 3+ row_it , Mn * 3 + col_it) = MnMat_out(row_it, col_it);
			}
		}
	}
}

void FrameManage::getV0(int frameId1, vector<int>* f1SparseIdxList, vector<int>* f2SparseIdxList, VectorXd & V0_out)
{
	assert(frameId1>=0);
	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId1 + 1];

	int Nt = frame1->pcsOct->ctLeaf->nodeList.size();
	V0_out.resize(3 * Nt);
	V0_out.setZero();

	//遍历sparse list
	for (int node_it = 0; node_it < f1SparseIdxList->size(); node_it++)
	{
		// 拿到应该拿的Mn
		VectorXd MnMat_out;
		int Mn = (*f1SparseIdxList)[node_it];
		//拿到对应点的motion vector
		Vec3 pMn = (*(frame1->pcsOct->ctLeaf->midVList))[(*f1SparseIdxList)[node_it]];
		Vec3 pN = (*(frame2->pcsOct->ctLeaf->midVList))[(*f2SparseIdxList)[node_it]];
		//Vector3d pDif(pMn.x - pN.x, pMn.y - pN.y, pMn.z - pN.z);
		Vector3d pDif(pN.x - pMn.x, pN.y -pMn.y, pN.z- pMn.z);

		//把 v(m) 赋值到V0中对应的位置上
		for (int row_it=0; row_it < pDif.rows(); row_it++)
		{
			V0_out(Mn * 3 + row_it) = pDif(row_it);
		}
		
		//test
		cout << "getV0:  node idx " << Mn << "posDif " << pDif[0] << " "<<pDif[1] << " " << pDif[2] << endl;
		//end
	}

}

//index: 取值范围：{1,2,3}
void FrameManage::selectionMatrix(int frameId1, int index, MatrixXd & sMat_out)
{
	assert(index > 0 && index <4);
	assert(frameId1 > -1);

	Frame* frame1 = frameList[frameId1];

	int Nt = frame1->pcsOct->ctLeaf->nodeList.size();
	sMat_out.resize(Nt, 3 * Nt);
	sMat_out.setZero();

	for (int n_it = 0; n_it < Nt; n_it++)
	{
		sMat_out(n_it, n_it * 3 + index - 1) = 1;
	}
}

void FrameManage::computeMotinVector(int frameId1, vector<int>* f1SparseIdxList,
	vector<int>* f2SparseIdxList, VectorXd & Vt_out)
{
#ifdef ZJW_DEBUG
	cout << "start compute MotinVector ..." << endl;
#endif //zjw_debug
	assert(frameId1 > -1);
	Frame* frame1 = frameList[frameId1];

	MatrixXd  Q;
	VectorXd V0;
	getQ(frameId1,f1SparseIdxList,f2SparseIdxList,Q);
	getV0(frameId1,f1SparseIdxList,f2SparseIdxList, V0);
	
	MatrixXd totalS;
	MatrixXd S;
	selectionMatrix(frameId1, 1, S);
	totalS = S.transpose() * (*frame1->pcsOct->spLaplacian) * S;
	selectionMatrix(frameId1, 2, S);
	totalS += S.transpose() * (*frame1->pcsOct->spLaplacian) * S;
	selectionMatrix(frameId1, 3, S);
	totalS += S.transpose() * (*frame1->pcsOct->spLaplacian) * S;

	Vt_out = (Q + u * totalS).inverse() * Q * V0;
	//MinresQLP::zjw_minres_QLP(Vt_out, Q + u * totalS, Q*V0);

#ifdef ZJW_DEBUG
	//test
	cout << "Q: " << endl;
	//cout << Q << endl;
	cout << endl << endl;
	cout << "V0: " << endl;
	cout << V0 << endl;
	cout << endl << endl;
	//end test

	//test
	FullPivLU<MatrixXd> lu_decomp((Q + u * totalS));
	cout << "mat: " << (Q + u * totalS).rows() <<" "<< (Q + u * totalS).cols() << endl;
	cout << "mat rank: " << lu_decomp.rank() << endl;
	cout << "motion vector: " << endl;
	cout << Vt_out << endl;
	//end test

	cout << "end compute MotinVector !!" << endl;
#endif //zjw_debug
}

void FrameManage::computeMotinVectorMinresQLP(int frameId1, vector<int>* f1SparseIdxList, vector<int>* f2SparseIdxList, VectorXd & Vt_out)
{
#ifdef ZJW_DEBUG
	cout << "start compute MotinVector ..." << endl;
#endif //zjw_debug
	assert(frameId1 > -1);
	Frame* frame1 = frameList[frameId1];

	MatrixXd  Q;
	VectorXd V0;
	getQ(frameId1, f1SparseIdxList, f2SparseIdxList, Q);
	getV0(frameId1, f1SparseIdxList, f2SparseIdxList, V0);

	MatrixXd totalS;
	MatrixXd S;
	selectionMatrix(frameId1, 1, S);
	totalS = S.transpose() * (*frame1->pcsOct->spLaplacian) * S;
	selectionMatrix(frameId1, 2, S);
	totalS += S.transpose() * (*frame1->pcsOct->spLaplacian) * S;
	selectionMatrix(frameId1, 3, S);
	totalS += S.transpose() * (*frame1->pcsOct->spLaplacian) * S;

	//Vt_out = (Q + u * totalS).inverse() * Q * V0;
	//放大对角元素
	//cout << "没有放大对角线元素...." << endl;
	/*VectorXd v;
	v.resize(Q.rows());
	v.fill(0.00001);
	MatrixXd enLargeMat(v.asDiagonal());*/
	
	//MinresQLP::zjw_minres_QLP(Vt_out, Q + u * totalS + enLargeMat, Q*V0);
	MinresQLP::zjw_minres_QLP(Vt_out, Q + u * totalS, Q*V0);

#ifdef ZJW_DEBUG
	////test
	//FullPivLU<MatrixXd> lu2((*frame1->pcsOct->spLaplacian));
	//cout << "laplacian: " << totalS.rows() << " " << totalS.cols() << endl;
	//cout << "laplacian rank: " << lu2.rank() << endl;

	//test
	/*FullPivLU<MatrixXd> lu((Q + u * totalS));
	cout << "total S: " << totalS.rows() << " " << totalS.cols() << endl;
	cout << "total s rank: " << lu.rank() << endl;*/

	//test
	FullPivLU<MatrixXd> lu_decomp((Q + u * totalS));
	cout << "mat: " << (Q + u * totalS).rows() << " " << (Q + u * totalS).cols() << endl;
	cout << "mat rank: " << lu_decomp.rank() << endl;
	cout << "motion vector: " << endl;
	cout << Vt_out << endl;
	//end test

	cout << "end compute MotinVector !!" << endl;
#endif //zjw_debug
}

void FrameManage::pridicTargetFrameVertex(int frameId1, VectorXd Vt)
{
#ifdef ZJW_DEBUG
	cout << "start compute pridic TargetFrame Vertex postion ...." << endl;
#endif //debug
	assert(frameId1 > -1);

	Frame* frame1 = frameList[frameId1];
	//确定ctleaf信息是否得到了
	assert(frame1->pcsOct->ctLeaf->nodeList.size()==0);
	ObjMesh * objMesh = frame1->objMesh;

	//初始化,分配空间
	objMesh->vertexPredictTargetList.clear();
	objMesh->vertexPredictTargetList.resize(objMesh->vertexList.size());

	//int count = 0;
	//遍历八叉树中的每个叶子节点，根据这个叶子节点的 motion vector 对该叶子节点中所有的点，进行偏移
	for (int node_it = 0; node_it < frame1->pcsOct->ctLeaf->nodeList.size(); node_it++)
	{
		Vec3 mv = Vec3(Vt(3 * node_it), Vt(3 * node_it + 1), Vt(3 * node_it + 2));
		
		//遍历这个叶子节点中的所有的点
		Node * node1 = &(frame1->pcsOct->ctLeaf->nodeList[node_it]->nodeData);
		for (int p_it = 0; p_it < node1->pointIdxList.size(); p_it++)
		{
			objMesh->vertexPredictTargetList[node1->pointIdxList[p_it]] = objMesh->vertexList[node1->pointIdxList[p_it]] + mv;
		}
	}

#ifdef ZJW_DEBUG
	//test
	//cout << "count : " << count << endl;
	//cout << "point number: "<<objMesh->vertexList.size() << endl;
	////end test

	cout << "end compute pridic TargetFrame Vertex postion !!!" << endl;
#endif //debug
}

void FrameManage::testOctreePCCompress(ObjMesh &frameObj_ref_out)
{

	//opcCompress->useCase0_Encoder();
	//opcCompress->useCase0_Decoder(frameObj_ref_out);

	opcCompress->useCase1_Encoder();
	opcCompress->useCase1_Decoder(frameObj_ref_out);
}


void FrameManage::encoderDiffBetweenSwapTargetFrame(int frameId1, VectorXd  &Vt_in, int frameId2)
{
	Frame* frame1 = frameList[frameId1];
	ObjMesh * objMesh1 = frame1->objMesh;
	Frame* frame2 = frameList[frameId2];
	ObjMesh * objMesh2 = frame2->objMesh;

	//frameId1得到相应的swapframe的vertex list,预测结果保存到objmesh对象中的 vertexPredictTargetList
	pridicTargetFrameVertex(frameId1, Vt_in);

	//build swap mesh
	ObjMesh swapObjMesh;
	swapObjMesh.vertexList = objMesh1->vertexPredictTargetList;

	//pcf: point cloud frame byte stream 修改
	string pathName = getPosCompressFileName(frameId1);
	std::ofstream of(pathName, std::ios_base::binary);
	if (of)
	{
		cout << "open  " << pathName << endl;
	}
	string pathName2 = getPosCompressFileName(frameId2);
	std::ofstream of2(pathName2, std::ios_base::binary);
	if (of2)
	{
		cout << "open  "<< pathName2 << endl;
	}
	
	opcCompress->encodePointCloud(swapObjMesh, of);
	opcCompress->encodePointCloud(*objMesh2, of2);

	of.close();
	of2.close();
}

void FrameManage::decoderDiffBetweenSwapTargetFrame(int frameId1, VectorXd & Vt_in, int frameId2, ObjMesh & frameObj_ref_out)
{
	string pathName = getPosCompressFileName(frameId1);
	std::ifstream in(pathName, std::ios_base::binary);
	if (in)
	{
		cout << "open  " << pathName << endl;
	}
	
	string pathName2 = getPosCompressFileName(frameId2);
	std::ifstream in2(pathName2, std::ios_base::binary);
	if (in2)
	{
		cout << "open  " << pathName2 << endl;
	}

	opcCompress->decodePointCloud(frameObj_ref_out, in);
	opcCompress->decodePointCloud(frameObj_ref_out, in2);
	in.close();
	in2.close();
}

//单帧的压缩
void FrameManage::encoderDiffByteStreamForFirstFrame(int frameId1)
{
	Frame* frame1 = frameList[frameId1];
	ObjMesh * objMesh1 = frame1->objMesh;
	//pcf: point cloud frame byte stream 修改
	string pathName = getPosCompressFileName(frameId1);
	std::ofstream of(pathName, std::ios_base::binary);
	if (of)
	{
		cout << "open  " << pathName << endl;
	}
	//保证第一帧在double buffer octree中只有自己这么一帧
	opcCompress->i_frame_ = true;
	//opcCompress->dbOctree->clearDBufferOctree();
	opcCompress->encodePointCloud(*objMesh1, of);
	of.close();
}
//单帧的解压
void FrameManage::decoderDiffByteStreamForFirstFrame(int frameId1, ObjMesh & frameObj_ref_out)
{
	string pathName = getPosCompressFileName(frameId1);
	std::ifstream in(pathName, std::ios_base::binary);
	if (in)
	{
		cout << "open  " << pathName << endl;
	}
	//保证第一帧在double buffer octree中只有自己这么一帧
	opcCompress->i_frame_ = true;
	//opcCompress->dbOctree->clearDBufferOctree();
	opcCompress->decodePointCloud(frameObj_ref_out, in);
	in.close();
}

//利用swap frame来进行压缩和解压的后续帧
void FrameManage::encoderDiffBetweenSwapTargetFrame2(int frameId1, VectorXd & Vt_in, int frameId2)
{
	ObjMesh * objMesh1 = frameList[frameId1]->objMesh;

	//frameId1得到相应的swapframe的vertex list,预测结果保存到objmesh对象中的 vertexPredictTargetList
	pridicTargetFrameVertex(frameId1, Vt_in);

	//build swap mesh
	ObjMesh swapObjMesh;
	swapObjMesh.vertexList = objMesh1->vertexPredictTargetList;

	//build octree for swap frame (false 表示reference frame)
	opcCompress->dbOctree->clearDBufferOctree();
	opcCompress->dbOctree->buildDBufferOctree(false, &swapObjMesh);
	
	//压缩和传输frame2 (bytestream and pos diff)
	Frame* frame2 = frameList[frameId2];
	ObjMesh * objMesh2 = frame2->objMesh;
	string pathName2 = getPosCompressFileName(frameId2);
	std::ofstream of2(pathName2, std::ios_base::binary);
	if (of2)
	{
		cout << "open  " << pathName2 << endl;
	}

	//保证已经利用的双Buffer Octree
	opcCompress->i_frame_ = false;
	opcCompress->encodePointCloud(*objMesh2, of2);

	of2.close();
}

void FrameManage::decoderDiffBetweenSwapTargetFrame2(int frameId1, VectorXd & Vt_in, int frameId2, ObjMesh & frameObj_ref_out)
{
	ObjMesh * objMesh1 = frameList[frameId1]->objMesh;
	assert(objMesh1->vertexList.size());
	assert(frameList[frameId1]->pcsOct->ctLeaf->nodeList.size() == 0);

	//frameId1得到相应的swapframe的vertex list,预测结果保存到objmesh对象中的 vertexPredictTargetList
	pridicTargetFrameVertex(frameId1, Vt_in);

	//build swap mesh
	ObjMesh swapObjMesh;
	swapObjMesh.vertexList = objMesh1->vertexPredictTargetList;

	//build octree for swap frame (false 表示reference frame)
	opcCompress->dbOctree->clearDBufferOctree();
	opcCompress->dbOctree->buildDBufferOctree(false, &swapObjMesh);

	//解压frame2 (bytestream and pos diff)
	string pathName2 = getPosCompressFileName(frameId2);
	std::ifstream in2(pathName2, std::ios_base::binary);
	if (in2)
	{
		cout << "open  " << pathName2 << endl;
	}

	//保证已经利用的双Buffer Octree
	opcCompress->i_frame_ = false;
	opcCompress->decodePointCloud(frameObj_ref_out, in2);
	in2.close();
}

string FrameManage::getPosCompressFileName(int frameId, string filePrefix, string fileSuffix)
{
	string temp;
	temp.append(filePrefix);

	//change the frame id to string
	std::stringstream stream;
	std::string result;
	stream << frameId;
	stream >> result;
	temp.append(result);

	temp.append(fileSuffix);
	return temp;
}

//==========================color compressiong =====================================
string FrameManage::getColorCompressFileName(int frameId, string filePrefix, string fileSuffix)
{
	string temp;
	temp.append(filePrefix);

	//change the frame id to string
	std::stringstream stream;
	std::string result;
	stream << frameId;
	stream >> result;
	temp.append(result);

	temp.append(fileSuffix);
	return temp;
}

bool FrameManage::getColorDiff(ObjMesh & swapObjMesh_in, int targetFrameId_in, vector<Color>& colorDiffList_out)
{
	//配置需要求多少个swap frame上最近的
	const int nearestNodeNum = 3;
	Frame * targetFrame = frameList[targetFrameId_in];
	ObjMesh * objMesh1 = targetFrame->objMesh;
	
	//针对swap的信息，构建swap frame的八叉树
	PcsOctree *swapOct = new PcsOctree(m, Nscales);
	for (int v_it = 0; v_it < swapObjMesh_in.vertexList.size(); v_it++)
	{
		Node& n = swapOct->pcsOct->getCell(swapObjMesh_in.vertexList[v_it]);

		//把点放到叶子节点所属的里面。
		n.pointPosList.push_back(swapObjMesh_in.vertexList[v_it]);
		n.colorList.push_back(swapObjMesh_in.colorList[v_it]);
		n.pointIdxList.push_back(v_it);
	}
	//并得到相关叶子节点的信息
	swapOct->pcsOct->traverse(swapOct->ctLeaf);

	//遍历所有的叶子节点
	for (int n_it = 0; n_it < targetFrame->pcsOct->ctLeaf->nodeList.size(); n_it++)
	{
		//针对target frame中的每个Node计算在swap frame中的最近的几个node，并得到这几个node上颜色值的之和
		Vec3 nodePos = (*targetFrame->pcsOct->ctLeaf->midVList)[n_it];
		Color predictColor(0, 0, 0);
		vector<double> disList;
		//int表示序号，double表示距离,map自动按照key进行排序,但是Key是唯一的，要保证
		
		vector<pair<int, double>> vecDisAndIndex;
		for (int i = 0; i < swapOct->ctLeaf->nodeList.size(); i++)
		{
			//遍历所有的node的点计算与nodePos的距离，保留最近的nearestNodeNum个
			vecDisAndIndex.push_back(pair<double,int>(i, nodePos.Distance((*swapOct->ctLeaf->midVList)[i])));
		}
		//转化成vector，利用sort函数针对value进行排序
		sort(vecDisAndIndex.begin(), vecDisAndIndex.end(), CmpByValue());
		auto it = vecDisAndIndex.begin();
		for (int i = 0; i < nearestNodeNum; i++, it++)
		{
			predictColor += swapOct->ctLeaf->nodeColorList[it->second];
		}

		//求平均，计算出target frame中的node颜色的预测值
		predictColor = predictColor / nearestNodeNum;

		//得到targetframe在这个node上的实际颜色值(不是swap上的颜色信息)
		Color actualColor = targetFrame->pcsOct->ctLeaf->nodeColorList[n_it];

		//保存实际值和预测值的差异
		colorDiffList_out.push_back(actualColor - predictColor);
	}
	
	//free some thing
	delete swapOct;
	return true;
}

void FrameManage::encoderColorDiffInfo(int frameId1, VectorXd & Vt_in, int frameId2)
{
	assert((frameId2 - frameId1) == 1);
	ObjMesh * objMesh1 = frameList[frameId1]->objMesh;

	//frameId1得到相应的swapframe的vertex list,预测结果保存到objmesh对象中的 vertexPredictTargetList
	pridicTargetFrameVertex(frameId1, Vt_in);

	//build swap mesh (position and color)
	ObjMesh swapObjMesh;
	swapObjMesh.vertexList = objMesh1->vertexPredictTargetList;
	swapObjMesh.colorList = objMesh1->colorList;

	//计算得到color diff信息
	//add some thing
	
	int addSomeThing = 0;
	
	//调用压缩接口，保存压缩内容到文件中
	string pathName2 = getPosCompressFileName(frameId2);
	std::ofstream of2(pathName2, std::ios_base::binary);
	if (of2)
	{
		cout << "open  " << pathName2 << endl;
	}

	//call the compress interface
	//opcCompress->encodePointCloud(*objMesh2, of2);
	int addCompressInterface = 0;

	of2.close();
}

void FrameManage::decoderColorDiffInfo(int frameId1, VectorXd & Vt_in, int frameId2, ObjMesh & frameObj_ref_out)
{
	ObjMesh * objMesh1 = frameList[frameId1]->objMesh;
	assert(objMesh1->vertexList.size());
	assert(frameList[frameId1]->pcsOct->ctLeaf->nodeList.size() == 0);

	//build swap mesh
	//frameId1得到相应的swapframe的vertex list,预测结果保存到objmesh对象中的 vertexPredictTargetList
	pridicTargetFrameVertex(frameId1, Vt_in);
	ObjMesh swapObjMesh;
	swapObjMesh.vertexList = objMesh1->vertexPredictTargetList;


	//调用解压接口，读取压缩内容到恢复frame2的color信息。
	string pathName2 = getPosCompressFileName(frameId2);
	std::ifstream in2(pathName2, std::ios_base::binary);
	if (in2)
	{
		cout << "open  " << pathName2 << endl;
	}

	//保证已经利用的双Buffer Octree
	//call the compress interface
	//opcCompress->decodePointCloud(frameObj_ref_out, in2);
	int addCompressInterface = 0;

	//根据得到的信息，恢复出相应的color信息，这个也可以解压接口中做好
	//add some thing
	int addSomeThing = 0;

	in2.close();
	
}




