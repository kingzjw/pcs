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
	//���ò������������˲���
	pcsOct->setParam(minPos, maxPos, cellSize);
	//pcsOct->buildPcsOctFrmPC(objMesh);
	pcsOct->buildPcsOctFrmPC(objMesh);
	//�õ�Ҷ�ӽڵ�ı߽磬��������ص���Ϣ
	pcsOct->getLeafboundary();

	//test
	//cout << pcsOct << endl;
	//end test
	pcsOct->getGraphMat();
	pcsOct->getMatEigenVerValue();

	//�õ��˲����϶�����ź�
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

	fileBatch = nullptr;
	P = new MatrixXd;
}

FrameManage::~FrameManage()
{
	if (P)
		delete P;

	if (fileBatch)
		delete fileBatch;
}

void FrameManage::getAllFilesPath(FileNameForMat type, string fileNameFormat, string path)
{
	//--------��ȡ��·���µ������ļ�-------------
	if (fileBatch)
		delete fileBatch;
	fileBatch = new FileBatch(path, fileNameFormat);
	fileBatch->fileNum = fileBatch->getFilesNum(fileBatch->filePath);
	frameList.clear();
	getAllFilePath = true;

	for (int f_it = 0; f_it < fileBatch->fileNum; f_it++)
	{
		//�õ����е�·��
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

		//������Ҫ��С���󣬱������е��ļ�·��
		fileBatch->files.push_back(totalFilePath);

		//�õ����е�frameList
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

	//--------��ȡ��·���µ������ļ�-------------
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

		//������Ҫ��С���󣬱������е��ļ�·��
		fileBatch->files.push_back(totalFilePath);

		Frame * frame = new Frame(f_it);
		frameList.push_back(frame);

		//-----------��ÿ��·���������ж�ȡ����sgw����------------
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

bool FrameManage::loadContinuousFrames(int frameId1, int frameId2, FileNameForMat type, string fileNameFormat, string path)
{
	assert(frameId1 > -1 && frameId2 > -1);
	assert(abs(frameId1-frameId2)==1);


	if (batchLoad)
		return true;

	if (!getAllFilePath)
	{
		getAllFilesPath(type, fileNameFormat, path);
	}

	assert(frameId1 < fileBatch->fileNum && frameId2 < fileBatch->fileNum);

	//-----------���ж�ȡ����sgw����------------
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

	//����frame1�����е�nodelist
	for (int leaf_it = 0; leaf_it < frame1->pcsOct->ctLeaf->nodeList.size(); leaf_it++)
	{
		//frame1: ����ÿ��node�У������node������е�ľ���ɷ��ȵ�Ȩ�ء�
		Node * node1 = &(frame1->pcsOct->ctLeaf->nodeList[leaf_it]->nodeData);
		Vec3 midPoint = (*frame1->pcsOct->ctLeaf->midVList)[leaf_it];
		vector<float> wightList;
		double totalWight = 0;
		double tempW;
		//�������Ҷ�ӽڵ������еĵ�
		for (int p_it = 0; p_it < node1->pointIdxList.size(); p_it++)
		{
			tempW = 1 / node1->pointPosList[p_it].Distance(midPoint);
			totalWight += tempW;
			wightList.push_back(tempW);
		}

		//����fram2��Ӧ���λ�ã�����Ȩ��Ԥ�Ƴ���ӦNode���е�λ��
		Vec3 predictNodePos;
		int idx;
		//Node * node2 = &(frame2->pcsOct->ctLeaf->nodeList[leaf_it]->nodeData);
		for (int p_it = 0; p_it < node1->pointIdxList.size(); p_it++)
		{
			//�õ�frame1�ڵ��е����ţ���frame2�еĵ��ֵ
			idx = node1->pointIdxList[p_it];
			predictNodePos += wightList[p_it] * frame2->objMesh->vertexList[idx];
		}
		predictNodePos = predictNodePos / totalWight;

		//����Ԥ�Ƴ������е�λ��,ͨ����frame2�ϵİ˲�����Ȼ��õ�ƥ���node���
		//����for ѭ��ʵ�֣�������ð˲��������ж�
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

//��f1nIdxList�м�¼�����еĶ�Ӧ��ϵ����������������˼���
//����������P
bool FrameManage::getMatrixP(int frameId1, int frameId2, vector<int>* f1nIdxList, vector<int>* f2nIdxList, MatrixXd * p)
{
	//�ж�����������֡
	assert(abs(frameId1 - frameId2) == 1);
	assert(f1nIdxList->size() > 0 && f2nIdxList->size() > 0);

	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId2];

	assert(frame1->pcsOct->ctLeaf->nodeList.size() == f1nIdxList->size());

	//------------------����ÿ�Զ�ӦNode֮���������������������sample��-------------------
	int totalScale = 5;
	int totalSignal = 6;
	int totalQuadrant = 8;

	MatrixXd sample;
	sample.resize(f1nIdxList->size(), totalScale * totalSignal * totalQuadrant);

	VectorXd featureVec1;
	VectorXd featureVec2;

	for (int it = 0; it < f1nIdxList->size(); it++)
	{
		frame1->pcsOct->getFeatureVector((*f1nIdxList)[it], &featureVec1);
		frame2->pcsOct->getFeatureVector((*f2nIdxList)[it], &featureVec2);

		//ensure �Ա����ĸ�����һ�µ�
		//assert(sample.cols() == featureVec1.rows());
		//���������������,��Ӧ������
		sample.row(it) = featureVec1-featureVec2;
	}

	//------------------���sample��Э�������----------------------

	Eigen::MatrixXd covMat;
	//��ȡ��������ֵ(��ÿһ���������ľ�ֵ)
	Eigen::MatrixXd meanVec = sample.colwise().mean();

	//����������ֵ��MatrixXf ת��Ϊ������ RowVectorXf
	Eigen::RowVectorXd meanVecRow(Eigen::RowVectorXd::Map(meanVec.data(), sample.cols()));

	//��ȡ��ȥ��ֵ������������
	Eigen::MatrixXd zeroMeanMat = sample;
	//ÿһ�е��������м��о��Ǽ����Ա�����������ȥ����ľ�ֵ
	zeroMeanMat.rowwise() -= meanVecRow;

	if (sample.rows() == 1)
	{
		//����Э����
		covMat = (zeroMeanMat.adjoint()*zeroMeanMat) / double(sample.rows());
	}
	else
	{
		//����Э����
		covMat = (zeroMeanMat.adjoint()*zeroMeanMat) / double(sample.rows() - 1);
	}

	//---------------��Э�������������棬�õ�P----------------------
#ifdef ZJW_DEBUG
	double size = 1.00001;
	cout << "��covMat�ľ���Խ�����Ԫ�طŴ��ˣ� "<< size<<" �� " << endl;
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
	if ((covMat.determinant())==0 && (lu_decomp.rank()!=covMat.rows()))
	{
		cout << "cov mat �����棬can not solve the inverse mat P" << endl;
#ifdef ZJW_DEBUG
		cout << "The rank of covMat is " << lu_decomp.rank() << endl;
		cout << "The size of covMat is " << covMat.rows() << " "<< covMat.cols() << endl;
		cout << "cov mat: " << endl;
		//cout << covMat << endl << endl;
		cout << covMat.diagonal() << endl;
#endif // zjw_debug

	}
	else
	{
		*p = covMat.inverse();

#ifdef ZJW_DEBUG
		cout << "cov mat �������!!" << endl;
		cout << "The rank of covMat is " << lu_decomp.rank() << endl;
		cout << "The size of covMat is " << covMat.rows() << " " << covMat.cols() << endl;

		//print
		cout << covMat.diagonal() << endl;
		cout << "mat P: " << endl;
		cout << *p << endl << endl;
#endif //zjw debug
	}

	return true;
}

//�������ݣ��õ�frame2��ÿ��node idx�� frame1�е����ƥ�䡣���ƥ���ϵ��������vector�з��ء�
//����ֵ��maDist �� f1nIdxList f2nIdxList�ǵõ���ѵ�ƥ���ϵ
//f2nIdxList�еĶ�������Ӧ���������ģ������м��ж���ƥ��ʧ��
bool FrameManage::getBestMatchPoint(int frameId1, int frameId2, MatrixXd * P, vector<int>* f1nIdxList, vector<int>* f2nIdxList, vector<double>* maDist)
{
#ifdef ZJW_DEBUG
	bool process = false;
#endif // ZJW_DEBUG

	//�ж�����������֡
	assert(abs(frameId1 - frameId2) == 1);
	f1nIdxList->clear();
	f2nIdxList->clear();
	maDist->clear();

	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId2];

	//����target frame ��frame2��������Node,�Ҷ����ƥ���node
	for (int node2_it = 0; node2_it < frame2->pcsOct->ctLeaf->nodeList.size(); node2_it++)
	{
		//���㵱ǰframe2 �е�feature vector
		//Node * node1 = &(frame1->pcsOct->ctLeaf->nodeList[node2_it]->nodeData);
		VectorXd featureVec2;
		frame2->pcsOct->getFeatureVector(node2_it, &featureVec2);

		//����frame1�����е�Ҷ�ӽڵ�,
		VectorXd featureVec1;
		vector<double> mahalanobisDist;
		mahalanobisDist.clear();
		for (int node1_it = 0; node1_it < frame1->pcsOct->ctLeaf->nodeList.size(); node1_it++)
		{
			//�õ�frame1��Ҷ�ӽڵ�� fearture vector
			frame1->pcsOct->getFeatureVector(node1_it, &featureVec1);

			//����node2 ��node1����feature vector�����Ͼ���
			double maha = (featureVec1 - featureVec2).transpose() * (*P) * (featureVec1 - featureVec2);
			mahalanobisDist.push_back(maha);
		}

		//�ҵ�node2��Ӧ��frame1�����нڵ�����Ͼ��룬�ҵ����ƥ�䣨��С��
		std::vector<double>::iterator smallest = std::min_element(std::begin(mahalanobisDist), std::end(mahalanobisDist));
		int indexNode1 = std::distance(std::begin(mahalanobisDist), smallest);

		//std::cout << "min element is " << *smallest << " at position " << std::distance(std::begin(mahalanobisDist), smallest) << std::endl;

		//������Ѷ�Ӧ��ϵ,����Ѷ�Ӧ��ϵ��������Ͼ���
		f2nIdxList->push_back(node2_it);
		f1nIdxList->push_back(indexNode1);
		maDist->push_back(*smallest);
#ifdef ZJW_DEBUG
		if (process)
		{
			printf("\b\b\b\b\b\b");
			cout.width(5);
			cout.precision(2);
			cout << ((node2_it + 1) * 100.0) / frame2->pcsOct->ctLeaf->nodeList.size() <<"%";
		}
		else
		{
			cout.width(5);
			cout.precision(2);
			cout << "match process:  " << ((node2_it + 1) * 100.0) / frame2->pcsOct->ctLeaf->nodeList.size()<< "%";
			process = true;
		}
#endif // ZJW_DEBUG
	}

#ifdef ZJW_DEBUG
	cout << endl;
#endif // ZJW_DEBUG

	return true;
}

//�����frameId�� ��target frame��id
//f1nIdxList f2nIdxList �Ǵ�������ƥ�����
//maDist����������ƥ��ľ���
//����ֵ��f1SparseIdxList  f2SparseIdxList   ϡ������ƥ��
bool FrameManage::doKmeansGetSparseBestMatch(int frameId, vector<int>* f1nIdxList, vector<int>* f2nIdxList,
	vector<double>* maDist, vector<int>* f1SparseIdxList, vector<int>* f2SparseIdxList)
{
	assert(frameId >= 0);
	assert(f1nIdxList->size() > 0 && f1nIdxList->size() > 0 && maDist->size() > 0);

	f1SparseIdxList->clear();
	f2SparseIdxList->clear();

	Frame* frame = frameList[frameId];
	KMeans * kmeans = frame->pcsOct->kmeans;
	//frame2(target frame) ���ݣ�����do kmeans,ȡ�����

	frame->pcsOct->doKmeans(5);
	//����ÿ��cluster����node
	for (int c_it = 0; c_it < kmeans->clusterRes.size(); c_it++)
	{
		//�õ����cluster�е���СmaDist,����ÿ���������ƥ��Ľڵ����
		int bestNodeId = -1;
		double minDis = -1;
		for (int n_it = 0; n_it < kmeans->clusterRes[c_it].size(); n_it++)
		{
			int nodeIdx = kmeans->clusterRes[c_it][n_it];

			if ((*maDist)[nodeIdx] < minDis || minDis == -1)
			{
				bestNodeId = nodeIdx;
				minDis = (*maDist)[nodeIdx];
			}
		}
#ifdef  ZJW_DEBUG
		//��ΪgetBestMatchPoint�еõ���f2Ӧ������Ǵӵ͵��������
		assert(bestNodeId == (*f2nIdxList)[bestNodeId]);
#endif //  zjw_debug
		//�����Ӧ��ϵ��sparse ver idx��
		f1SparseIdxList->push_back((*f1nIdxList)[bestNodeId]);
		f2SparseIdxList->push_back((*f2nIdxList)[bestNodeId]);
	}

	return true;
}

bool FrameManage::trainGetP(int frameId1, int frameId2, FileNameForMat type, string fileNameFormat, string path)
{


	vector<int> f1nIdxList;
	vector<int> f2nIdxList;
	//ѵ�����ݣ��õ�����P
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
