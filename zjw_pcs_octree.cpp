#pragma once
#include "zjw_pcs_octree.h"



PcsOctree::PcsOctree()
{
	Vec3 min(0.0f, 0.0f, 0.0f);
	Vec3 max(1.0f + Epsilon, 1.0f + Epsilon, 1.0f + Epsilon);
	Vec3 cellSize(0.1, 0.1, 0.1);

	ctLeaf = nullptr;
	ctGraph = nullptr;
	pcsOct = nullptr;
}

PcsOctree::~PcsOctree()
{
	delete pcsOct;
	delete ctLeaf;
	delete ctGraph;
}

void PcsOctree::setParam(Vec3 min, Vec3 max, Vec3 cellSize)
{
	this->min = min;
	this->max = max;
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

	ctGraph = new CallTGetGraph(&dMat, &weightAMat);
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
		cout << " CallTGetGraph::initParam error" << endl;
		return;
	}

	dMat = MatrixXd::Zero(nodeNum, nodeNum);
	weightAMat = MatrixXd::Zero(nodeNum, nodeNum);
	LaplacianMat = MatrixXd::Zero(nodeNum, nodeNum);
	eigenVecMat = MatrixXd::Zero(nodeNum, nodeNum);
	eigenValMat = MatrixXd::Zero(nodeNum, nodeNum);

}

void PcsOctree::getGraphMat()
{
	
	initMat();

#ifdef ZJW_DEUG
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

	LaplacianMat = dMat - weightAMat;

#ifdef ZJW_DEUG
	//printMat();
	timer.Stop();
	cout << "getGraphMat time: " << timer.GetInMs() << " ms " << endl;
	
	cout << "finish get the D & weight matriex !" << endl;
#endif 

}

void PcsOctree::getMatEigenVerValue()
{

#ifdef ZJW_DEUG
	ZjwTimer timer;
	timer.Start();
	cout << "start get Mat Eigen Vertor and Value...." << endl;
#endif 

	EigenSolver<MatrixXd> es(LaplacianMat);

	//way1
	//Matrix whose columns are the (possibly complex) eigenvectors.
	//es.eigenvectors().col(0)


	//way2
	//Const reference to matrix whose columns are the pseudo-eigenvectors.
	eigenValMat = es.pseudoEigenvalueMatrix();
	eigenVecMat = es.pseudoEigenvectors();

#ifdef ZJW_DEUG
	//cout << "L :" << endl<<LaplacianMat <<endl;

	/*cout << "The eigenvalues of A are:" << endl << es.eigenvalues() << endl;
	cout << "The matrix of eigenvectors, V, is:" << endl << es.eigenvectors() << endl << endl;*/

	/*cout << "The pseudo-eigenvalue matrix D is:" << endl << eigenValMat << endl;
	cout << "The pseudo-eigenvector matrix V is:" << endl << eigenVecMat << endl;
	cout << "Finally, V * D * V^(-1) = " << endl << eigenVecMat * eigenValMat * eigenVecMat.inverse() << endl;*/
	
	timer.Stop();
	cout << "getMatEigenVerValue time: " << timer.GetInMs() << " ms " << endl;
	cout << "end get Mat Eigen Vertor and Value !" << endl;
#endif
}

void PcsOctree::getGraph()
{
}



void PcsOctree::clearOct()
{
	if (pcsOct != nullptr)
		pcsOct->clear();
}

void PcsOctree::printMat()
{
	cout << "****************************" << endl;
	cout << "node num : " << nodeNum << endl;
	cout << "****************************" << endl;
	cout << "D mat : " << endl;
	cout << dMat << endl;
	cout << "****************************" << endl;
	cout << "weight mat: " << endl;
	cout << weightAMat << endl;
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

CallTGetGraph::CallTGetGraph(MatrixXd * dMatPtr, MatrixXd * weightAMatPtr)
{
	
	this->dMatPtr = dMatPtr;
	this->weightAMatPtr = weightAMatPtr;
}

CallTGetGraph::~CallTGetGraph()
{	
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
	double maxDist = curNodeLegth/2 + Length(octCellSize)/2 + Epsilon;

	//-----------快速拒绝---------
	Vec3 midPoint = (currNode->min + currNode->max) / 2;
	Vec3 delta = midPoint - leafMidPoint;
	//太远了，不需要遍历子节点
	if (Length(delta) >  maxDist)
		return false;

	//-------------如果可能找到邻居--------------

	//不是叶子节点，需要遍历子节点，继续进行快速拒绝
	int idx = currNode->leafFlag;
	if (idx == -1 )
		return true;

	//当前节点是叶子节点（因为是无向图，是对称矩阵，只需要考虑一边）
	if (idx >= leafIdx)
		return false;
	
	//得到矩阵
	(*weightAMatPtr)(idx, leafIdx) = 1 / Length(delta);
	(*weightAMatPtr)(leafIdx, idx) = 1 / Length(delta);
	(*dMatPtr)(leafIdx, leafIdx) += 1 / Length(delta);
	(*dMatPtr)(idx, idx) += 1 / Length(delta);

	return true;

}
