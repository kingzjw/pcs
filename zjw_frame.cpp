#include "zjw_frame.h"

//Frame::Frame()
//{
//	objMesh = new ObjMesh();
//	pcsOct = new PcsOctree();
//}

Frame::Frame(const int frameId)
{
	objMesh = new ObjMesh();
	pcsOct = new PcsOctree();
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

bool Frame::octSgwt()
{
#ifdef ZJW_TIMER
	ZjwTimer timer2;
	timer2.Start();
#endif

	Vec3 minPos(objMesh->rangeMin);
	Vec3 maxPos((objMesh->rangeMax + Epsilon));
	/*Vec3 minPos(objMesh.rangeMin);
	Vec3 maxPos((objMesh.rangeMax + Epsilon));*/

	Vec3 cellSize(0.1);
	//设置参数，并构建八叉树
	pcsOct->setParam(minPos, maxPos, cellSize);
	//pcsOct->buildPcsOctFrmPC(objMesh);
	pcsOct->buildPcsOctFrmPC(objMesh);
	//得到叶子节点的边界，并保存相关的信息
	pcsOct->getLeafboundary();
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
	batchLoad = false;
	getAllFilePath = false;
	//changeSouceData = true;

	fileBatch = nullptr;
	//oct 中用到的参数
	cellSize.x = cellSize.y = cellSize.z = 0.1;
	clusterNum = 5;
	P = new MatrixXd;

	u = 1;
}

FrameManage::~FrameManage()
{
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
		Frame * frame = new Frame(f_it);
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

		Frame * frame = new Frame(f_it);
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

		frameList[f_it]->octSgwt();
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

		frameList[frameId1]->octSgwt();
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

		frameList[frameId2]->octSgwt();
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
			tempW = 1 / node1->pointPosList[p_it].Distance(midPoint);
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
			cout << "frame1 node " << leaf_it << " match faied!!" << endl;
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
bool FrameManage::getMatrixP(int frameId1, int frameId2, vector<int>* f1nIdxList, vector<int>* f2nIdxList, MatrixXd * p)
{
	//判断是连续的两帧
	assert(abs(frameId1 - frameId2) == 1);
	assert(f1nIdxList->size() > 0 && f2nIdxList->size() > 0);

	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId2];

	assert(frame1->pcsOct->ctLeaf->nodeList.size() == f1nIdxList->size());

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
#endif // zjw_debug
	}
	else
	{
		*p = covMat.inverse();

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
			double maha = (featureVec1 - featureVec2).transpose() * (*P) * (featureVec1 - featureVec2);
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

//传入的frameId： 是target frame的id
//f1nIdxList f2nIdxList 是传入的最佳匹配参数
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
	//frame2(target frame) 内容，进行do kmeans,取出结果

	frame->pcsOct->doKmeans(clusterNum);
	//遍历每个cluster集的node
	for (int c_it = 0; c_it < kmeans->clusterRes.size(); c_it++)
	{
		//得到这个cluster中的最小maDist,保存每个区域最佳匹配的节点序号
		int bestNodeId = -1;
		double minDis = -1;
		for (int n_it = 0; n_it < kmeans->clusterRes[c_it].size(); n_it++)
		{
			//nodeIdx的范围，就是当前frame 中叶子结点的序号范围  0 - (nodeSize-1)
			int nodeIdx = kmeans->clusterRes[c_it][n_it];

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

		//保存对应关系到sparse ver idx中
		f1SparseIdxList_out->push_back((*f1nIdxList)[bestNodeId]);
		f2SparseIdxList_out->push_back((*f2nIdxList)[bestNodeId]);
	}

	return true;
}

bool FrameManage::getTwoFrameBestSparseMatch(int frameId1, int frameId2, vector<int>* f1SparseIdxList_out,
	vector<int>* f2SparseIdxList_out, FileNameForMat type, string fileNameFormat, string path, bool changeData)
{
	vector<int> f1nIdxList;
	vector<int> f2nIdxList;
	vector<double> maDist;
	
	//训练数据
	loadContinuousFrames(frameId1, frameId2, type, fileNameFormat, path, changeData);
	//拿到frame2中每个node idx在 frame1中的最佳匹配
	getBestMatchPoint(frameId1, frameId2, P, &f1nIdxList, &f2nIdxList, &maDist);
	//对target frame上的帧进行k-means
	doKmeansGetSparseBestMatch(frameId2, &f1nIdxList, &f2nIdxList, &maDist, f1SparseIdxList_out, f2SparseIdxList_out);
	return true;
}

bool FrameManage::trainGetP(int frameId1, int frameId2, FileNameForMat type, string fileNameFormat, string path)
{
	vector<int> f1nIdxList;
	vector<int> f2nIdxList;
	//训练数据，得到矩阵P
	loadContinuousFrames(frameId1, frameId2, type, fileNameFormat, path);
#ifdef ZJW_DEBUG
	cout << "start to traie data for mat P ..." << endl;
#ifdef ZJW_TIMER
	ZjwTimer test;
	test.Start();
#endif //ZJW_TIMER
#endif // ZJW_DEBUG
	matchNode(frameId1, frameId2, &f1nIdxList, &f2nIdxList);
	getMatrixP(frameId1, frameId2, &f1nIdxList, &f2nIdxList, P);

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
	assert(frameId1>0);

	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId1 + 1];
	MnMat_out.resize(3, 3);
	MnMat_out.setZero();

	//得到frame1 点 Mn为中心，two hop之间的顶点序号
	set<int>  MTwoHopList;
	frame1->pcsOct->getTwoHopNeighborhood(MnIdx, &MTwoHopList, frame1->pcsOct->spLaplacian);

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
		MnMat_out += (pDif * pDif.transpose()) / sqrtf(maDis_M_N - maDis_Mn_N);
	}

	MnMat_out /= MTwoHopList.size();
}

void FrameManage::getQ(int frameId1, vector<int>* f1SparseIdxList, vector<int>* f2SparseIdxList, MatrixXd & Q_out)
{
	assert(frameId1>0);
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
		getMnMat(frameId1, Mn, (*f2SparseIdxList)[node_it], MnMat_out);

		//把Mn赋值到Q中对应的位置上
		for (int col_it; col_it < MnMat_out.cols(); col_it++)
		{
			for (int row_it; row_it < MnMat_out.rows(); row_it++)
			{
				Q_out(Mn * 3+ row_it , Mn * 3 + col_it) = MnMat_out(row_it, col_it);
			}
		}
	}
}

void FrameManage::getV0(int frameId1, vector<int>* f1SparseIdxList, vector<int>* f2SparseIdxList, VectorXd & V0_out)
{
	assert(frameId1>0);
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
		Vector3d pDif(pMn.x - pN.x, pMn.y - pN.y, pMn.z - pN.z);

		//把 v(m) 赋值到V0中对应的位置上
		for (int row_it; row_it < pDif.rows(); row_it++)
		{
			V0_out(Mn * 3 + row_it) = pDif(row_it);
		}
	}

}

//index: 取值范围：{1,2,3}
void FrameManage::selectionMatrix(int frameId1, int index, MatrixXd & sMat_out)
{
	assert(index > 0 && index <4);
	assert(frameId1 > -1);

	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId1 + 1];

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

#ifdef ZJW_DEBUG
	//test
	cout << "Q: " << endl;
	cout << Q << endl;
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
