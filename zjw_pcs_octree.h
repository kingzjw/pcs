#include "zjw_octree.h"
#include "util\zjw_math.h"
#include "util\zjw_obj.h"

//------------- Class that holds your data.--------------------
#include <vector>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

//------------------------------ Class that holds your data.-----------------------

//节点包含点的数据
class Node
{
public:
	vector<Vec3> pointList; //每个在空间有一堆的 particle
};


//-----------------遍历整个八叉树，把叶子节点的边界值保存到list中，并给叶子节点编号-----------------
class CallTraverseGetInfoSetLeaf : public Octree<Node>::Callback
{
public:
	//保存叶子节点的boundary ，用于渲染所用
	vector<Vec3> minVList;
	vector<Vec3> maxVList;
	//保存也子节点的list,并给叶子节点编号
	vector<Octree<Node>::OctreeNode*> nodeList;

public:
	CallTraverseGetInfoSetLeaf();

	virtual bool operator()(const Vec3 min, const Vec3 max, Node& nodeData) {
		return true;
	}
	//给叶子节点编号
	virtual bool operator()(const Vec3 min, const Vec3 max, Octree<Node>::OctreeNode* currNode);
};

//----------------------把点云得到转换为矩阵----------------------------------

class CallTGetGraph : public Octree<Node>::Callback
{
public:
	//得到该节点周围邻居，及其权重，并保存到weight矩阵中。
	int leafIdx;
	Vec3 leafMidPoint;
	Vec3 octCellSize;
	MatrixXd  * dMatPtr;
	MatrixXd  * weightAMatPtr;

public:
	CallTGetGraph(MatrixXd  * dMatPtr,MatrixXd  * weightAMatPtr);
	~CallTGetGraph();

	void initParam(Vec3 & octreeCellSize);

	virtual bool operator()(const Vec3 min, const Vec3 max, Node& nodeData) {
		return true;
	}
	//得到相邻节点，及其权重
	virtual bool operator()(const Vec3 min, const Vec3 max, Octree<Node>::OctreeNode* currNode);
};


//-----------------------------------------------------
class PcsOctree
{
public:
	Vec3 min;
	Vec3 max;
	Vec3 cellSize;

	Octree<Node> *pcsOct;
	CallTraverseGetInfoSetLeaf * ctLeaf;
	CallTGetGraph * ctGraph;

	//邻接权重矩阵，以及D矩阵，求解特征值和特征向量的存储矩阵。
	int nodeNum;
	MatrixXd  dMat;
	MatrixXd  weightAMat;

public:
	PcsOctree();
	~PcsOctree();
	//设置参数，重新初始化八叉树
	void setParam(Vec3 min, Vec3 max, Vec3 cellSize);
	//建立八叉树
	void buildPcsOctFrmPC(ObjMesh * objeMesh);
	//拿到叶子节点的Boundary，设置叶子节点的信息
	void getLeafboundary();
	//拿到graph的两个矩阵

	//初始化矩阵等信息
	void initParam();
	//得到矩阵
	void getGraphMat();

	void getGraph();
	//清空八叉树
	void clearOct();

	//=============== test ====================
	void printMat();
};