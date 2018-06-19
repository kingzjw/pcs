#pragma once
#include "zjw_pcs_octree.h"

PcsOctree::PcsOctree(int _m, int _Nscales)
{
	m = _m;
	Nscales = _Nscales;
	//	this->objMesh = objMesh;

	minPos = Vec3(0.0f, 0.0f, 0.0f);
	maxPos = Vec3(1.0f + Epsilon, 1.0f + Epsilon, 1.0f + Epsilon);
	cellSize = Vec3(0.1, 0.1, 0.1);

	ctLeaf = nullptr;
	ctGraph = nullptr;
	pcsOct = nullptr;
	kmeans = new KMeans;

#ifdef USE_SPARSE
	spLaplacian = nullptr;
	coefficients = new vector<T>;
#endif // USE_SPARSE

#ifdef SGWT_DEBUG
	fastSgwt = nullptr;
#endif //SGWT_DEBUG
}

PcsOctree::~PcsOctree()
{
	//����Ҫdelete objMesh��objMeshָʾ���ñ��˴����õĶ���

	if (ctLeaf)
		delete ctLeaf;
	if (ctGraph)
		delete ctGraph;
	if (pcsOct)
		delete pcsOct;
	if (kmeans)
		delete kmeans;

#ifdef USE_SPARSE
	if (spLaplacian)
		delete	spLaplacian;

	if (coefficients)
		delete coefficients;

#ifdef SGWT_DEBUG
	if (fastSgwt)
		delete fastSgwt;
#endif //SGWT_DEBUG

#endif // USE_SPARSE
}

void PcsOctree::setParam(Vec3 min, Vec3 max, Vec3 cellSize)
{
	this->minPos = min;
	this->maxPos = max;
	this->cellSize = cellSize;
	if (!pcsOct)
	{
		pcsOct = new Octree<Node>(min, max, cellSize);
	}
	else
	{
		delete pcsOct;
		pcsOct = new Octree<Node>(min, max, cellSize);
	}
	if (ctLeaf)
		delete ctLeaf;
	if (ctGraph)
		delete ctGraph;
	if (pcsOct)
		delete pcsOct;

	ctLeaf = new CallTraverseGetInfoSetLeaf;
	pcsOct = new Octree<Node>(min, max, cellSize);

#ifdef USE_EIGEN
	ctGraph = new CallTGetGraph(&dMat, &weightAMat);
#endif //USE_EIGEN

#ifdef USE_SPARSE
	if (spLaplacian)
		delete	spLaplacian;
	ctGraph = new CallTGetGraph(coefficients);
#endif // USE_SPARSE

#ifdef USE_ARPACK
	ctGraph = new CallTGetGraph(&laplacianMat);
#endif //USE_ARPACK

	ctGraph->initParam(cellSize);
}

void PcsOctree::buildPcsOctFrmPC(ObjMesh * objeMesh)
{
	cout << endl << "Building octree...." << endl;
	// Initialize octree.
	// Minimum coordinates.
	// Add all particles to the octree.
	for (int v_it = 0; v_it < objeMesh->vertexList.size(); v_it++)
	{
		Node& n = pcsOct->getCell(objeMesh->vertexList[v_it]);

		//�ѵ�ŵ�Ҷ�ӽڵ����������档
		n.pointPosList.push_back(objeMesh->vertexList[v_it]);
		n.colorList.push_back(objeMesh->colorList[v_it]);
		n.pointIdxList.push_back(v_it);
	}
	cout << "Building octree done." << endl;
	return;
}

void PcsOctree::getLeafboundary()
{
	cout << "Computing Leavf boundary...." << endl;

	// Prepare the callback class.
	pcsOct->traverse(ctLeaf);

#ifdef ZJW_DEBUG
	cout << "Computing Leavf boundary done!" << endl;
#endif // ZJW_DEUG
}

#ifdef RELATIVE_DIS_SIGNAL
void PcsOctree::changeNodePointsToRelativeDis()
{
	//�������е�Ҷ�ӽ��
	for (int leaf_it = 0; leaf_it < ctLeaf->nodeList.size(); leaf_it++)
	{
		//�������Ҷ�ӽ���е�����points
		Octree<Node>::OctreeNode * tempNode = ctLeaf->nodeList[leaf_it];
		
		for (int p_it = 0; p_it < tempNode->nodeData.pointPosList.size(); p_it++)
		{
			//��ȥ���Node�߽��ϵ���Сֵ���͵õ����Point �����node�е����λ�á�
#ifdef RELATIVE_DIS_SIGNAL
			//������Ծ���
			//��ȥ���Node�߽��ϵ���Сֵ���͵õ����Point �����node�е����λ�á�
			tempNode->nodeData.pointPosList[p_it] = tempNode->nodeData.pointPosList[p_it];
#elif
			//���Ծ���
			//��ȥ���Node�߽��ϵ���Сֵ���͵õ����Point �����node�е����λ�á�
			tempNode->nodeData.pointPosList[p_it] = tempNode->nodeData.pointPosList[p_it] - tempNode->min;
#endif //relative dis signal
		}
	}
}
#endif //relative dis signal

void PcsOctree::initMat()
{
#ifdef ZJW_DEBUG
	cout << "leaf node num :  " << ctLeaf->nodeList.size() << endl;
	cout << "init the matriex !" << endl;
#endif
	nodeNum = ctLeaf->nodeList.size();
	if (nodeNum == 0)
	{
		cout << "CallTGetGraph::initParam error" << endl;
		return;
	}
#ifdef USE_EIGEN
	dMat = MatrixXd::Zero(nodeNum, nodeNum);
	weightAMat = MatrixXd::Zero(nodeNum, nodeNum);
	LaplacianMat = MatrixXd::Zero(nodeNum, nodeNum);
	eigenVecMat = MatrixXd::Zero(nodeNum, nodeNum);
	eigenValMat = MatrixXd::Zero(nodeNum, nodeNum);
#endif //USE_EIGEN

#ifdef USE_SPARSE
	spLaplacian = new SpMat(nodeNum, nodeNum);
#endif // USE_SPARSE

#ifdef USE_ARPACK
	laplacianMat.initParam(nodeNum);
#endif // USE_ARPACK
}

void PcsOctree::getGraphMat()
{
	initMat();

#ifdef ZJW_TIMER
	ZjwTimer timer;
	timer.Start();
	cout << "start get the D & weight matriex...." << endl;
#endif

	//�������е�Ҷ�ӽڵ㣬�ҵ�ÿ��Ҷ�ӽڵ�������Ľڵ�,���õ�Ȩ��
	for (int i = 0; i < ctLeaf->nodeList.size(); i++)
	{
		ctGraph->leafIdx = ctLeaf->nodeList[i]->leafFlag;
		//����Ҷ�ӽڵ������Ľڵ�
		ctGraph->leafMidPoint = (ctLeaf->nodeList[i]->max + ctLeaf->nodeList[i]->min) / 2;
		pcsOct->traverse(ctGraph);
	}

#ifdef USE_EIGEN
	LaplacianMat = dMat - weightAMat;
#endif //use eigen

#ifdef  USE_SPARSE
#ifdef ZJW_DEBUG
	//cout << " spLaplacian size : " << spLaplacian->innerSize() << "  " << spLaplacian->outerSize() << endl;
#endif //zjw_debug

	//����ϡ���LaplacianMat
	assert(coefficients->size() > 0);
	spLaplacian->setFromTriplets(coefficients->begin(), coefficients->end());

#ifdef ZJW_PRINT_INFO
  //��ӡ���Ƿ�0Ԫ��
	cout << "****************************" << endl;
	cout << "sparse mat lap: " << endl;
	for (int k = 0; k < spLaplacian->outerSize(); ++k)
	{
		for (SpMat::InnerIterator it(*spLaplacian, k); it; ++it)
		{
			//cout << it.value() << " " << it.row() << " " << it.col() << " " << it.index() << endl;   // row index
			cout << it.value() << " ";   // row index
		}
		cout << endl;
	}
#endif //ZJW_PRINT_INFO
#endif //  USE_SPARSE

#ifdef ZJW_TIMER
	timer.Stop();
	cout << "getGraphMat time: " << timer.GetInMs() << " ms " << endl;
	cout << "finish get the D & weight matriex !" << endl;
	//printMat();
#endif
}

void PcsOctree::getMatEigenVerValue()
{
#ifdef USE_EIGEN
	ZjwTimer timer;
	timer.Start();
	cout << "start get Mat Eigen Vertor and Value...." << endl;

	EigenSolver<MatrixXd> es(LaplacianMat);

	//way1
	//Matrix whose columns are the (possibly complex) eigenvectors.
	//es.eigenvectors().col(0)

	//way2
	//Const reference to matrix whose columns are the pseudo-eigenvectors.
	eigenValMat = es.pseudoEigenvalueMatrix();
	eigenVecMat = es.pseudoEigenvectors();

#ifdef ZJW_PRINT_INFO

	/*cout << "The eigenvalues of A are:" << endl << es.eigenvalues() << endl;
	cout << "The matrix of eigenvectors, V, is:" << endl << es.eigenvectors() << endl << endl;*/

	cout << "88888888888888888888888888888888888888888888888888888888888" << endl;
	cout << "L :" << endl << LaplacianMat << endl;
	cout << "The pseudo-eigenvalue matrix D is:" << endl << eigenValMat << endl;
	cout << "The pseudo-eigenvector matrix V is:" << endl << eigenVecMat << endl;
	cout << "Finally, V * D * V^(-1) = " << endl << eigenVecMat * eigenValMat * eigenVecMat.inverse() << endl;
	cout << "88888888888888888888888888888888888888888888888888888888888" << endl;

#endif //ZJW_PRINT_INFO
	cout << "getMatEigenVerValue time: " << timer.GetInMs() << " ms " << endl;
	cout << "end get Mat Eigen Vertor and Value !" << endl;
	timer.Stop();

#endif // USE_EIGEN

#ifdef USE_ARPACK
	ZjwTimer timer;
	timer.Start();
	cout << "start get Mat Eigen Vertor and Value...." << endl;

	laplacianMat.dsaupdEvalsEvecs();

#ifdef  ZJW_PRINT_INFO
	laplacianMat.printMat();
	laplacianMat.printValueVector();
#endif //  ZJW_PRINT_INFO

	cout << "getMatEigenVerValue time: " << timer.GetInMs() << " ms " << endl;
	cout << "end get Mat Eigen Vertor and Value !" << endl;
	timer.Stop();

#endif //use_arpack
}

//�õ�nodeidx������two hop���õ��ĵ㲻�����Լ�����Ϊ������ͼ�����������two hop����һ����
void PcsOctree::getTwoHopNeighborhood(int nodeIdx, set<int>* nodeList_out, SpMat * spLaplacian)
{
	assert(nodeIdx > -1 && nodeIdx < ctLeaf->nodeList.size());
	//ֻ֧��������
	assert(!spLaplacian->IsRowMajor);
	assert(nodeList_out);
	
	//�����һ���Ľ��
	vector<int> tempList;
	//������һ�е����з�0Ԫ��
	for (SparseMatrix<double>::InnerIterator it(*spLaplacian, nodeIdx); it; ++it)
	{
		if (it.value() != 0)
		{
			tempList.push_back(it.row());
		}
	}

	//set<int> nodeList_out;
	nodeList_out->clear();
	for (int node_it = 0; node_it < tempList.size(); node_it++)
	{
		int index = tempList[node_it];
		for (SparseMatrix<double>::InnerIterator it(*spLaplacian, index); it; ++it)
		{
			if (it.value() != 0)
			{
				nodeList_out->insert(it.row());
			}
		}
	}
	//�Ƴ��Լ�
	nodeList_out->erase(nodeIdx);
}

void PcsOctree::clearOct()
{
	if (pcsOct != nullptr)
		pcsOct->clear();
}

int PcsOctree::judege8Aeros(Vec3 & min, Vec3 & max, Vec3 & point)
{
	unsigned int  i = 0;
	Vec3 mid = 0.5 * (max + min);

	if (point.x >= mid.x)
	{
		i = i | 1;
	}

	if (point.y >= mid.y)
	{
		i = i | 2;
	}

	if (point.z >= mid.z)
	{
		i = i | 4;
	}

	return i;
}

int PcsOctree::judege8Aeros(Vec3 & mid, Vec3 & point)
{
	unsigned int  i = 0;

	if (point.x >= mid.x)
	{
		i = i | 1;
	}

	if (point.y >= mid.y)
	{
		i = i | 2;
	}

	if (point.z >= mid.z)
	{
		i = i | 4;
	}

	return i;
}

void PcsOctree::setPointTo8Areas()
{
#ifdef ZJW_DEBUG
	cout << "start set leafnode points to 8 areas ...." << endl;
#endif

	for (int leaf_it = 0; leaf_it < ctLeaf->nodeList.size(); leaf_it++)
	{
		Octree<Node>::OctreeNode* octNode = ctLeaf->nodeList[leaf_it];

		octNode->nodeData.leafNodePos8Areas.clear();
		octNode->nodeData.leafNodeColor8Areas.clear();

		octNode->nodeData.leafNodePos8Areas.resize(8);
		octNode->nodeData.leafNodeColor8Areas.resize(8);

		Vec3 mid = 0.5 * (octNode->max + octNode->min);
		//������Ҷ�ӽڵ������еĵ�
		for (int p_it = 0; p_it < octNode->nodeData.pointPosList.size(); p_it++)
		{
#ifdef RELATIVE_DIS_SIGNAL
			//�жϳ��ýڵ�����������
			int index = judege8Aeros(mid, octNode->nodeData.pointPosList[p_it] + octNode->min);

#else
			//���Ծ���
			//�жϳ��ýڵ�����������
			int index = judege8Aeros(mid, octNode->nodeData.pointPosList[p_it]);
#endif //relative dis signal
			//���Ծ����ʱ�򣬷ŵ��Ǿ۶Ѿ�����źš����λ�õ�ʱ�򣬷ŵ����λ�õ��źš�
			octNode->nodeData.leafNodePos8Areas[index].push_back((Vec3 *)(&octNode->nodeData.pointPosList[p_it]));
			octNode->nodeData.leafNodeColor8Areas[index].push_back((Vec3 *)(&octNode->nodeData.colorList[p_it]));
		}
	}

#ifdef ZJW_DEBUG
	cout << "end set leafnode points to 8 areas!!!!" << endl;
#endif
}

vector<VectorXd> PcsOctree::getSignalF(SignalType sType)
{
	//��ʼ����Ϣ
	vector<VectorXd> posSignal;
	VectorXd initF(nodeNum);
	initF.setZero();
	for (int i = 0; i < 8; i++)
	{
		posSignal.push_back(initF);
	}

	switch (sType)
	{
	case SignalX:
		//�������е�Ҷ�ӽڵ�
		for (int i = 0; i < ctLeaf->nodeList.size(); i++)
		{
			//�����˸�����, �ö�Ӧ���޵��źŵ�ֵ
			for (int j = 0; j < 8; j++)
			{
				//�ų�������û�е�����
				if (ctLeaf->nodeList[i]->nodeData.pos8Flag[j])
				{
					posSignal[j](i) = ctLeaf->nodeList[i]->nodeData.pos8AreasSignal[j].x;
				}
			}
		}
		break;
	case SignalY:
		//�������е�Ҷ�ӽڵ�
		for (int i = 0; i < ctLeaf->nodeList.size(); i++)
		{
			//�����˸�����
			for (int j = 0; j < 8; j++)
			{
				//�ų�������û�е�����
				if (ctLeaf->nodeList[i]->nodeData.pos8Flag[j])
				{
					posSignal[j](i) = ctLeaf->nodeList[i]->nodeData.pos8AreasSignal[j].y;
				}
			}
		}
		break;
	case SignalZ:
		//�������е�Ҷ�ӽڵ�
		for (int i = 0; i < ctLeaf->nodeList.size(); i++)
		{
			//�����˸�����
			for (int j = 0; j < 8; j++)
			{
				//�ų�������û�е�����
				if (ctLeaf->nodeList[i]->nodeData.pos8Flag[j])
				{
					posSignal[j](i) = ctLeaf->nodeList[i]->nodeData.pos8AreasSignal[j].z;
				}
			}
		}
		break;
	case SignalR:
		//�������е�Ҷ�ӽڵ�
		for (int i = 0; i < ctLeaf->nodeList.size(); i++)
		{
			//�����˸�����
			for (int j = 0; j < 8; j++)
			{
				//�ų�������û�е�����
				if (ctLeaf->nodeList[i]->nodeData.pos8Flag[j])
				{
					posSignal[j](i) = ctLeaf->nodeList[i]->nodeData.color8AreasSignal[j].x;
				}
			}
		}
		break;
	case SignalG:
		//�������е�Ҷ�ӽڵ�
		for (int i = 0; i < ctLeaf->nodeList.size(); i++)
		{
			//�����˸�����
			for (int j = 0; j < 8; j++)
			{
				//�ų�������û�е�����
				if (ctLeaf->nodeList[i]->nodeData.pos8Flag[j])
				{
					posSignal[j](i) = ctLeaf->nodeList[i]->nodeData.color8AreasSignal[j].y;
				}
			}
		}
		break;
	case SignalB:
		//�������е�Ҷ�ӽڵ�
		for (int i = 0; i < ctLeaf->nodeList.size(); i++)
		{
			//�����˸�����
			for (int j = 0; j < 8; j++)
			{
				//�ų�������û�е�����
				if (ctLeaf->nodeList[i]->nodeData.pos8Flag[j])
				{
					posSignal[j](i) = ctLeaf->nodeList[i]->nodeData.color8AreasSignal[j].z;
				}
			}
		}
		break;
	default:
		break;
	}

	return posSignal;
}

bool PcsOctree::getAllSignalAndSaveSGWTCoeff()
{

#ifdef SGWT_DEBUG
	if (!fastSgwt)
	{
		assert(m > 0 && Nscales > 0);
		fastSgwt = new SgwtCheby(m, Nscales, *spLaplacian);
	}

#ifdef ZJW_TIMER
	ZjwTimer test;
	test.Start();
#endif //ZJW_TIMER

	//�õ�����źţ������������е��źţ����н����źš�
	vector<SignalType> typeList;
	typeList.push_back(SignalX);
	typeList.push_back(SignalY);
	typeList.push_back(SignalZ);
	typeList.push_back(SignalR);
	typeList.push_back(SignalG);
	typeList.push_back(SignalB);

	int totalQuadrant = 8;

	//8��vectorXd�� �ֱ��ʾ��ͬ���޵��ź�
	vector<VectorXd> fSignal;
	//�������е��ź�
	for (int type_it = 0; type_it < typeList.size(); type_it++)
	{
		//8��vectorXd�� �ֱ��ʾ��ͬ���޵��źš�ÿ��vector��ʾ���ǣ������ڽڵ�����������еĸ��ͺŵľ����ֵ
		fSignal = getSignalF(typeList[type_it]);

		//�������е�����
		for (int quadrant_it = 0; quadrant_it < totalQuadrant; quadrant_it++)
		{
			//vector�б�ʾ��5����ͬ�߶��������ж��������
			(*fastSgwt).sgwt->saveSgwtCoeff(typeList[type_it], quadrant_it, &fSignal[quadrant_it], &((*fastSgwt).sgwt->coeff));
		}
	}

#ifdef ZJW_TIMER
	test.Stop();
	test.printTimeInMs("getAllSignalAndSaveSGWTCoeff time : ");
#endif //zjw_timer

#endif //SGWT_DEBUG
	return true;
}

//��������ڵ����ض��ź��������������źţ�����ֵӦ����8��ϵ������������Ӧÿ������
bool PcsOctree::getSignalF(SignalType sType, int nodeIdx, VectorXd * sigVQ_out)
{
	//��ʼ����Ϣ
	if (sigVQ_out)
		delete sigVQ_out;
	sigVQ_out = new VectorXd(8);
	sigVQ_out->setZero();

	switch (sType)
	{
	case SignalX:
		//�����˸�����, �ö�Ӧ���޵��źŵ�ֵ
		for (int j = 0; j < 8; j++)
		{
			//�ų�������û�е�����
			if (ctLeaf->nodeList[nodeIdx]->nodeData.pos8Flag[j])
			{
				(*sigVQ_out)(j) = ctLeaf->nodeList[nodeIdx]->nodeData.pos8AreasSignal[j].x;
			}
		}

		break;
	case SignalY:
		//�����˸�����, �ö�Ӧ���޵��źŵ�ֵ
		for (int j = 0; j < 8; j++)
		{
			//�ų�������û�е�����
			if (ctLeaf->nodeList[nodeIdx]->nodeData.pos8Flag[j])
			{
				(*sigVQ_out)(j) = ctLeaf->nodeList[nodeIdx]->nodeData.pos8AreasSignal[j].y;
			}
		}
		break;
	case SignalZ:
		//�����˸�����, �ö�Ӧ���޵��źŵ�ֵ
		for (int j = 0; j < 8; j++)
		{
			//�ų�������û�е�����
			if (ctLeaf->nodeList[nodeIdx]->nodeData.pos8Flag[j])
			{
				(*sigVQ_out)(j) = ctLeaf->nodeList[nodeIdx]->nodeData.pos8AreasSignal[j].z;
			}
		}
		break;
	case SignalR:
		//�����˸�����, �ö�Ӧ���޵��źŵ�ֵ
		for (int j = 0; j < 8; j++)
		{
			//�ų�������û�е�����
			if (ctLeaf->nodeList[nodeIdx]->nodeData.pos8Flag[j])
			{
				(*sigVQ_out)(j) = ctLeaf->nodeList[nodeIdx]->nodeData.color8AreasSignal[j].x;
			}
		}
		break;
	case SignalG:
		for (int j = 0; j < 8; j++)
		{
			//�ų�������û�е�����
			if (ctLeaf->nodeList[nodeIdx]->nodeData.pos8Flag[j])
			{
				(*sigVQ_out)(j) = ctLeaf->nodeList[nodeIdx]->nodeData.color8AreasSignal[j].y;
			}
		}
		break;
	case SignalB:
		for (int j = 0; j < 8; j++)
		{
			//�ų�������û�е�����
			if (ctLeaf->nodeList[nodeIdx]->nodeData.pos8Flag[j])
			{
				(*sigVQ_out)(j) = ctLeaf->nodeList[nodeIdx]->nodeData.color8AreasSignal[j].z;
			}
		}
		break;
	default:
		break;
	}

	return true;
}

vector<VectorXd> PcsOctree::getSgwtCoeffWS(SignalType type, int quadrant)
{
	//�õ�����źţ������������е��źţ����н����źš�
	vector<VectorXd> fSignal = getSignalF(type);

#ifdef SGWT_DEBUG
	if (!fastSgwt)
	{
		assert(m > 0 && Nscales > 0);
		fastSgwt = new SgwtCheby(m, Nscales, *spLaplacian);
	}
	//�õ�ָ�����޵ģ�����źŵ� sgwt��ϵ��

	/*ZjwTimer test;
	test.Start();*/
	vector<VectorXd> wf_s = (*fastSgwt)(fSignal[quadrant]);
	/*test.Stop();
	test.printTimeInMs("sgwt_cheby_op: get sgw coeff for signal f ");*/

#ifdef ZJW_DEBUG
	cout << "********* getSgwtCoeffWS *********" << endl;
	cout << "sgwt for signal " << type << " in quadrant " << quadrant << endl;
	fastSgwt->sgwt->getVectorVectorXdInfo(wf_s);
	cout << "********************************" << endl;
#endif //ZJW_DEUG

#endif //SGWT_DEBUG
	return wf_s;
}

bool PcsOctree::getFeatureVector(int nodeIdx, VectorXd *featureVector)
{
#ifdef SGWT_DEBUG
	if (!fastSgwt)
	{
		assert(m > 0 && Nscales > 0);
		fastSgwt = new SgwtCheby(m, Nscales, *spLaplacian);
	}

#ifdef ZJW_TIMER
	/*ZjwTimer test;
	test.Start();*/
#endif //ZJW_TIMER

	//�õ�����źţ������������е��źţ����н����źš�
	vector<SignalType> typeList;
	typeList.push_back(SignalX);
	typeList.push_back(SignalY);
	typeList.push_back(SignalZ);
	typeList.push_back(SignalR);
	typeList.push_back(SignalG);
	typeList.push_back(SignalB);

	//��¼g func��scale�ĸ������ڼ���һ��h func
	int totalScale = (fastSgwt->sgwt->t.size() + 1);
	int totalSignal = typeList.size();
	int totalQuadrant = 8;
	featureVector->resize(totalQuadrant * totalSignal * totalScale);

	//�����±�
	int idx = -1;
	//8��vectorXd�� �ֱ��ʾ��ͬ���޵��ź�
	vector<VectorXd> fSignal;
	//�������е��ź�
	for (int type_it = 0; type_it < typeList.size(); type_it++)
	{
		//8��vectorXd�� �ֱ��ʾ��ͬ���޵��źš�ÿ��vector��ʾ���ǣ������ڽڵ�����������еĸ��ͺŵľ����ֵ
		fSignal = getSignalF(typeList[type_it]);

		//�������е�����
		for (int quadrant_it = 0; quadrant_it < 8; quadrant_it++)
		{
			//vecotr�б�ʾ��5����ͬ�߶��������ж��������
			vector<VectorXd> wf_s = (*fastSgwt)(fSignal[quadrant_it]);

			//�õ��ýڵ�������߶��µ�ϵ����
			for (int s_it = 0; s_it < totalScale; s_it++)
			{
				idx = type_it * totalQuadrant * totalScale + quadrant_it * totalScale + s_it;
				(*featureVector)(idx) = wf_s[s_it](nodeIdx);
			}
		}
	}

#ifdef ZJW_TIMER
	/*test.Stop();
	cout << nodeIdx;
	test.printTimeInMs(" get feature vector time : ");*/
#endif //zjw_timer

#endif //SGWT_DEBUG
	return true;
}

bool PcsOctree::getFeatureVector2(int nodeIdx, VectorXd * featureVector)
{
#ifdef SGWT_DEBUG
	if (!fastSgwt)
	{
		assert(m > 0 && Nscales > 0);
		fastSgwt = new SgwtCheby(m, Nscales, *spLaplacian);
	}

	//#ifdef ZJW_TIMER
	//	ZjwTimer test;
	//	test.Start();
	//#endif //ZJW_TIMER

		//�õ�����źţ������������е��źţ����н����źš�
	vector<SignalType> typeList;
	typeList.push_back(SignalX);
	typeList.push_back(SignalY);
	typeList.push_back(SignalZ);
	typeList.push_back(SignalR);
	typeList.push_back(SignalG);
	typeList.push_back(SignalB);

	int totalScale = (fastSgwt->sgwt->t.size() + 1);
	int totalSignal = typeList.size();
	int totalQuadrant = 8;
	featureVector->resize(totalQuadrant * totalSignal * totalScale);

	//�����±�
	int idx = -1;
	//��ʾ�������������źţ�������޵�ϵ�� ά�ȣ�5*1
	VectorXd w_s;
	//�������е��ź�
	for (int type_it = 0; type_it < typeList.size(); type_it++)
	{
		//�������е�����
		for (int quadrant_it = 0; quadrant_it < 8; quadrant_it++)
		{
			//wf_s������ǣ� vecotr�б�ʾ��5����ͬ�߶��������ж���ĵ�ϵ��
			(*fastSgwt)(nodeIdx, typeList[type_it], quadrant_it, &w_s);
			//�õ��ýڵ�������߶��µ�ϵ����
			for (int s_it = 0; s_it < totalScale; s_it++)
			{
				//int nodeIdx, int signalType, int quadrantType, VectorXd* sgwt_out
				idx = type_it * totalQuadrant * totalScale + quadrant_it * totalScale + s_it;
				(*featureVector)(idx) = w_s(s_it);
			}
		}
	}

	//#ifdef ZJW_TIMER
	//	test.Stop();
	//	cout << nodeIdx;
	//	test.printTimeInMs(" getFeatureVector2  time : ");
	//#endif //zjw_timer

#endif //SGWT_DEBUG

	return true;
}

void PcsOctree::getSgwtCoeffWS()
{
	posSignalX = getSignalF(SignalType::SignalX);
	posSignalY = getSignalF(SignalType::SignalY);
	posSignalZ = getSignalF(SignalType::SignalZ);

#ifdef SGWT_DEBUG
	if (!fastSgwt)
	{
		assert(m > 0 && Nscales > 0);
		fastSgwt = new SgwtCheby(m, Nscales, *spLaplacian);
	}
#endif //SGWT_DEBUG

	//�õ��ź�X ��8�������е��ź�
	for (int i = 0; i < 8; i++)
	{
		//�������ޣ������ź��µģ�wf
#ifdef SGWT_DEBUG
		vector<VectorXd> wf_s = (*fastSgwt)(posSignalX[i]);
#ifdef ZJW_DEBUG
		fastSgwt->sgwt->getVectorVectorXdInfo(wf_s);
		//fastSgwt->sgwt->printVectorVectorXd(wf_s);
#endif //ZJW_DEUG
#endif //SGWT_DEBUG
	}
}

void PcsOctree::doKmeans(int clusterNum)
{
#ifdef ZJW_TIMER
	cout << "start do Kmeans ...." << endl;
	cout << "kmeans cluster set to " << clusterNum << endl;
	ZjwTimer timer2;
	timer2.Start();
#endif

	//���÷��������
	kmeans->setClusterNum(clusterNum);
	//k means���о���
	kmeans->cluster(ctLeaf->midVList);

	//���������clusterRes��������Ҷ�ӽڵ�����
	//dosomething

#ifdef ZJW_DEBUG
	//��ӡ������Ϣ
	cout << "kmeans result: " << endl;
	for (int c_it = 0; c_it < kmeans->clusterRes.size(); c_it++)
	{
		cout << "cluster " << c_it << " : " << endl;

		for (int n_it = 0; n_it < kmeans->clusterRes[c_it].size(); n_it++)
		{
			cout << kmeans->clusterRes[c_it][n_it]<<" ";
		}
		cout <<endl;
	}
#endif // ZJW_DEBUG

#ifdef ZJW_TIMER
	timer2.Stop();
	timer2.printTimeInMs("end do Kmeans: ");
#endif
}

int PcsOctree::judegePointToLeafNode(Vec3 * point, int & idx)
{
	CallTraverseJudePoint judgeP(*point);
	pcsOct->traverse(&judgeP);
	idx = judgeP.nodeIdx;

	//���û��ƥ��ɹ������ص���-1.
	return idx;
}

void PcsOctree::getLeafSignal()
{
#ifdef ZJW_DEBUG
	cout << "start get Leaf Signal ...." << endl;
#endif

	//����ÿ��Ҷ�ӽڵ�
	for (int leaf_it = 0; leaf_it < ctLeaf->nodeList.size(); leaf_it++)
	{
		Octree<Node>::OctreeNode* octNode = ctLeaf->nodeList[leaf_it];

		//������ǰҶ�ӽڵ�İ˸�����
		for (int i = 0; i < octNode->nodeData.leafNodePos8Areas.size(); i++)
		{
			//�������û��point
			if (octNode->nodeData.leafNodePos8Areas[i].size() == 0)
			{
				octNode->nodeData.pos8Flag.push_back(false);
				octNode->nodeData.pos8AreasSignal.push_back(Vec3(0, 0, 0));
				octNode->nodeData.color8AreasSignal.push_back(Vec3(0, 0, 0));
				continue;
			}

			octNode->nodeData.pos8Flag.push_back(true);

			//����������޵�����point,�������źš������õ�����ƽ���ķ�����������ÿ�����޵����е��ƽ���ź�
			Vec3 posSignal(0, 0, 0);
			Vec3 colorSignal(0, 0, 0);

			for (int p_it = 0; p_it < octNode->nodeData.leafNodePos8Areas[i].size(); p_it++)
			{
				posSignal += *(octNode->nodeData.leafNodePos8Areas[i][p_it]);
				colorSignal += *(octNode->nodeData.leafNodeColor8Areas[i][p_it]);
			}
			posSignal /= octNode->nodeData.leafNodePos8Areas[i].size();
			colorSignal /= octNode->nodeData.leafNodeColor8Areas[i].size();

			octNode->nodeData.pos8AreasSignal.push_back(posSignal);
			octNode->nodeData.color8AreasSignal.push_back(colorSignal);
		}
	}

#ifdef ZJW_DEBUG
	cout << "end get Leaf Signal!!!!" << endl;
#endif
}

void PcsOctree::printMat()
{
	cout << "****************************" << endl;
	cout << "node num : " << nodeNum << endl;
	cout << "****************************" << endl;

#ifdef USE_EIGEN
	cout << "D mat : " << endl;
	cout << dMat << endl;
	cout << "****************************" << endl;
	cout << "weight mat: " << endl;
	cout << weightAMat << endl;
#endif // USE_EIGEN
}

//------------------------------------------------------------------

CallTraverseGetInfoSetLeaf::CallTraverseGetInfoSetLeaf()
{
	midVList = new vector<Vec3>;
	minVList.clear();
	maxVList.clear();
	leafIncr = 0;
}

CallTraverseGetInfoSetLeaf::~CallTraverseGetInfoSetLeaf()
{
	if (midVList)
		delete midVList;

	for (int i = 0; i < midVList->size(); i++)
	{
		delete (*midVList)[i];
	}
}

bool CallTraverseGetInfoSetLeaf::operator()(const Vec3 min, const Vec3 max, Octree<Node>::OctreeNode * currNode)
{
	bool flag = true;
	//�ж��ǻ����Ҷ�ӽڵ�
	if (currNode->children[0] || currNode->children[1] || currNode->children[2] || currNode->children[3]
		|| currNode->children[4] || currNode->children[5] || currNode->children[6] || currNode->children[7])
	{
		flag = true;
	}
	else
	{
		//��Ҷ�ӽڵ㣬��ô����Ҷ�ӽڵ�ľ��η�Χ��Ȼ���˳�
		flag = false;
		minVList.push_back(currNode->min);
		maxVList.push_back(currNode->max);

		//����Ҷ�ӽڵ���м�
		Vec3 temp = (currNode->min + currNode->max) / 2;
		midVList->push_back(temp);
		nodeList.push_back(currNode);
		currNode->leafFlag = leafIncr;
		leafIncr++;
	}
	return flag;
}

//------------------------------------------------------------------

//CallTGetGraph::CallTGetGraph(Vec3 octreeCellSize)
//{
//	this->octCellSize = octreeCellSize;
//}

CallTGetGraph::~CallTGetGraph()
{
#ifdef USE_EIGEN
	delete dMatPtr;
	delete weightAMatPtr;
#endif // USE_EIGEN

#ifdef USE_SPARSE
	if (spLap)
		spLap = nullptr;
	//��Ϊcoeffָ������������õĵ�ַ������ֱ��delete,���Ǹ�ֵΪ�գ���Ϊdelete���ָ��ָ����ڴ��Ҳɾ����
	if (coeff)
		coeff = nullptr;

#endif // USE_SPARSE

#ifdef  USE_ARPACK
	delete laplacianMat;
#endif //  use_arpack
}
void CallTGetGraph::initParam(Vec3 & octreeCellSize)
{
	this->octCellSize = octreeCellSize;
}
//����false,��ֹ�ݹ����;����true,�����ݹ�����ӽڵ�
bool CallTGetGraph::operator()(const Vec3 min, const Vec3 max, Octree<Node>::OctreeNode * currNode)
{
	double curNodeLegth = Length(max - min);
	//������ײ��⣬���������ֵ
	double maxDist = curNodeLegth / 2 + Length(octCellSize) / 2 + Epsilon;

	//-----------���پܾ�---------
	Vec3 midPoint = (currNode->min + currNode->max) / 2;
	Vec3 delta = midPoint - leafMidPoint;
	//̫Զ�ˣ�����Ҫ�����ӽڵ�
	if (Length(delta) > maxDist)
		return false;

	//-------------��������ҵ��ھ�--------------

	//����Ҷ�ӽڵ㣬��Ҫ�����ӽڵ㣬�������п��پܾ�
	int idx = currNode->leafFlag;
	if (idx == -1)
		return true;

	//��ǰ�ڵ���Ҷ�ӽڵ㣨��Ϊ������ͼ���ǶԳƾ���ֻ��Ҫ����һ�ߣ�
	if (idx >= leafIdx)
		return false;

	//�õ�����
#ifdef  USE_EIGEN
	(*weightAMatPtr)(idx, leafIdx) = 1 / Length(delta);
	(*weightAMatPtr)(leafIdx, idx) = 1 / Length(delta);
	(*dMatPtr)(leafIdx, leafIdx) += 1 / Length(delta);
	(*dMatPtr)(idx, idx) += 1 / Length(delta);
#endif //  USE_EIGEN

#ifdef USE_SPARSE
	//L  = D-W ����ֱ�ӱ���ΪL��

	coeff->push_back(T(idx, leafIdx, -1 / Length(delta)));
	coeff->push_back(T(leafIdx, idx, -1 / Length(delta)));
	coeff->push_back(T(leafIdx, leafIdx, 1 / Length(delta)));
	coeff->push_back(T(idx, idx, 1 / Length(delta)));
#endif // USE_SPARSE

#ifdef USE_ARPACK
	//L  = D-W ����ֱ�ӱ���ΪL��

	//-W
	laplacianMat->setMat(idx, leafIdx, -1 / Length(delta));
	laplacianMat->setMat(leafIdx, idx, -1 / Length(delta));

	//D
	laplacianMat->setAddMat(leafIdx, 1 / Length(delta));
	laplacianMat->setAddMat(idx, 1 / Length(delta));
#endif // USE_ARPACK

	return true;
}

CallTraverseJudePoint::CallTraverseJudePoint(Vec3 point)
{
	nodeIdx = -1;
	this->point = point;
}

CallTraverseJudePoint::~CallTraverseJudePoint()
{
}

//����false,��ֹ�ݹ����;����true,�����ݹ�����ӽڵ�
bool CallTraverseJudePoint::operator()(const Vec3 min, const Vec3 max, Octree<Node>::OctreeNode * currNode)
{
	//�������node���棬����Ҫ�����ӽڵ�
	if (!(point >= min && point < max))
		return false;

	//-------------��������Node����--------------

	//����Ҷ�ӽڵ㣬��Ҫ�����ӽڵ㣬�������п��پܾ�
	int idx = currNode->leafFlag;
	if (idx == -1)
		return true;

	//��ǰ�ڵ���Ҷ�ӽڵ㣬�õ�idx
	nodeIdx = idx;
	return true;
}