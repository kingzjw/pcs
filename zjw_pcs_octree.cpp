#pragma once
#include "zjw_pcs_octree.h"

PcsOctree::PcsOctree()
{
	minPos =Vec3(0.0f, 0.0f, 0.0f);
	maxPos=Vec3(1.0f + Epsilon, 1.0f + Epsilon, 1.0f + Epsilon);
	cellSize = Vec3(0.1, 0.1, 0.1);

	ctLeaf = nullptr;
	ctGraph = nullptr;
	pcsOct = nullptr;

#ifdef USE_SPARSE
	spLaplacian = nullptr;
#endif // USE_SPARSE
}

PcsOctree::~PcsOctree()
{
	if (ctLeaf)
		delete ctLeaf;
	if (ctGraph)
		delete ctGraph;
	if (pcsOct)
		delete pcsOct;

#ifdef USE_SPARSE
	if (spLaplacian)
		delete	spLaplacian;
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

		//把点放到叶子节点所属的里面。
		n.pointList.push_back(objeMesh->vertexList[v_it]);
	}
	cout << "Building octree done." << endl;
	return;
}

void PcsOctree::getLeafboundary()
{
	cout << "Computing Leavf boundary...." << endl;

	// Prepare the callback class.
	pcsOct->traverse(ctLeaf);

#ifdef ZJW_DEUG
	cout << "Computing Leavf boundary done!" << endl;
#endif // ZJW_DEUG
}

void PcsOctree::initMat()
{
#ifdef ZJW_DEUG
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

#ifdef USE_SPARSE
	spLaplacian = new SpMat(nodeNum, nodeNum);
#endif // USE_SPARSE

#endif //USE_EIGEN
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

	//遍历所有的叶子节点，找到每个叶子节点的相连的节点,并得到权重
	for (int i = 0; i < ctLeaf->nodeList.size(); i++)
	{
		ctGraph->leafIdx = ctLeaf->nodeList[i]->leafFlag;
		//保存叶子节点中中心节点
		ctGraph->leafMidPoint = (ctLeaf->nodeList[i]->max + ctLeaf->nodeList[i]->min) / 2;
		pcsOct->traverse(ctGraph);
	}
#ifdef USE_EIGEN
	LaplacianMat = dMat - weightAMat;
#endif //use eigen

#ifdef  USE_SPARSE
	//生成稀疏的LaplacianMat
	spLaplacian->setFromTriplets(coefficients.begin(), coefficients.end());
	
	for (int k = 0; k<spLaplacian->outerSize(); ++k)
		for (SparseMatrix<double>::InnerIterator it(*spLaplacian, k); it; ++it)
		{
			it.value();
			it.row();   // row index
			it.col();   // col index (here it is equal to k)
			it.index(); // inner index, here it is equal to it.row()
		}
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
#ifdef ZJW_TIMER
	ZjwTimer timer;
	timer.Start();
	cout << "start get Mat Eigen Vertor and Value...." << endl;
#endif

#ifdef USE_EIGEN
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

#endif // USE_EIGEN

#ifdef USE_ARPACK
	laplacianMat.dsaupdEvalsEvecs();

#ifdef  ZJW_PRINT_INFO
	laplacianMat.printMat();
	laplacianMat.printValueVector();
#endif //  ZJW_PRINT_INFO

#endif //use_arpack

#ifdef ZJW_TIMER

	cout << "getMatEigenVerValue time: " << timer.GetInMs() << " ms " << endl;
	cout << "end get Mat Eigen Vertor and Value !" << endl;
	timer.Stop();

#endif
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
#ifdef ZJW_DEUG
	cout << "start set leafnode points to 8 areas ...." << endl;
#endif

	for (int leaf_it = 0; leaf_it < ctLeaf->nodeList.size(); leaf_it++)
	{
		Octree<Node>::OctreeNode* octNode = ctLeaf->nodeList[leaf_it];

		octNode->nodeData.leafNode8Areas.clear();
		octNode->nodeData.leafNode8Areas.resize(8);

		Vec3 mid = 0.5 * (octNode->max + octNode->min);
		//遍历该叶子节点上所有的点
		for (int p_it = 0; p_it < octNode->nodeData.pointList.size(); p_it++)
		{
			//判断出该节点所属的象限
			int index = judege8Aeros(mid, octNode->nodeData.pointList[p_it]);
			octNode->nodeData.leafNode8Areas[index].push_back((Vec3 *)(&octNode->nodeData.pointList[p_it]));
		}
	}

#ifdef ZJW_DEUG
	cout << "end set leafnode points to 8 areas!!!!" << endl;
#endif
}

void PcsOctree::getLeafSignal()
{
#ifdef ZJW_DEUG
	cout << "start get Leaf Signal ...." << endl;
#endif

	//遍历每个叶子节点
	for (int leaf_it = 0; leaf_it < ctLeaf->nodeList.size(); leaf_it++)
	{
		Octree<Node>::OctreeNode* octNode = ctLeaf->nodeList[leaf_it];

		//遍历当前叶子节点的八个象限
		for (int i = 0; i < octNode->nodeData.leafNode8Areas.size(); i++)
		{
			//这个象限没有point
			if (octNode->nodeData.leafNode8Areas[i].size() == 0)
			{
				octNode->nodeData.pos8Areas.push_back(Vec3(-1, -1, -1));
				break;
			}

			//遍历这个象限的所有point,来计算信号。下面用的是求评价的方法，来计算每个象限的所有点的平均信号
			Vec3 posSignal(0, 0, 0);
			for (int p_it = 0; p_it < octNode->nodeData.leafNode8Areas[i].size(); p_it++)
			{
				posSignal += *(octNode->nodeData.leafNode8Areas[i][p_it]);
			}
			posSignal /= octNode->nodeData.leafNode8Areas[i].size();

			octNode->nodeData.pos8Areas.push_back(posSignal);
		}
	}

#ifdef ZJW_DEUG
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
	minVList.clear();
	maxVList.clear();
}

bool CallTraverseGetInfoSetLeaf::operator()(const Vec3 min, const Vec3 max, Octree<Node>::OctreeNode * currNode)
{
	bool flag = true;
	//判断是会否是叶子节点
	if (currNode->children[0] || currNode->children[1] || currNode->children[2] || currNode->children[3]
		|| currNode->children[4] || currNode->children[5] || currNode->children[6] || currNode->children[7])
	{
		flag = true;
	}
	else
	{
		//叶子节点的计数
		static int leafIncr = 0;
		//是叶子节点，那么保留叶子节点的矩形范围，然后退出
		flag = false;
		minVList.push_back(currNode->min);
		maxVList.push_back(currNode->max);
		//
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
	delete spLap;
#endif // USE_SPARSE

#ifdef  USE_ARPACK
	delete laplacianMat;
#endif //  use_arpack

}
void CallTGetGraph::initParam(Vec3 & octreeCellSize)
{
	this->octCellSize = octreeCellSize;
}
//返回false,终止递归遍历;返回true,继续递归遍历子节点
bool CallTGetGraph::operator()(const Vec3 min, const Vec3 max, Octree<Node>::OctreeNode * currNode)
{
	double curNodeLegth = Length(max - min);
	//根据碰撞检测，来考虑这个值
	double maxDist = curNodeLegth / 2 + Length(octCellSize) / 2 + Epsilon;

	//-----------快速拒绝---------
	Vec3 midPoint = (currNode->min + currNode->max) / 2;
	Vec3 delta = midPoint - leafMidPoint;
	//太远了，不需要遍历子节点
	if (Length(delta) > maxDist)
		return false;

	//-------------如果可能找到邻居--------------

	//不是叶子节点，需要遍历子节点，继续进行快速拒绝
	int idx = currNode->leafFlag;
	if (idx == -1)
		return true;

	//当前节点是叶子节点（因为是无向图，是对称矩阵，只需要考虑一边）
	if (idx >= leafIdx)
		return false;

	//得到矩阵
#ifdef  USE_EIGEN
	(*weightAMatPtr)(idx, leafIdx) = 1 / Length(delta);
	(*weightAMatPtr)(leafIdx, idx) = 1 / Length(delta);
	(*dMatPtr)(leafIdx, leafIdx) += 1 / Length(delta);
	(*dMatPtr)(idx, idx) += 1 / Length(delta);
#endif //  USE_EIGEN

#ifdef USE_SPARSE
	coeff.push_back(T(idx, leafIdx, 1 / Length(delta)));
	coeff.push_back(T(leafIdx, idx, 1 / Length(delta)));
	coeff.push_back(T(leafIdx, leafIdx, 1 / Length(delta)));
	coeff.push_back(T(idx, idx, 1 / Length(delta)));
#endif // USE_SPARSE

#ifdef USE_ARPACK
	//L  = D-W 下面直接保存为L了

	//-W
	laplacianMat->setMat(idx, leafIdx, -1 / Length(delta));
	laplacianMat->setMat(leafIdx, idx, -1 / Length(delta));

	//D
	laplacianMat->setAddMat(leafIdx, 1 / Length(delta));
	laplacianMat->setAddMat(idx, 1 / Length(delta));
#endif // USE_ARPACK

	return true;
}