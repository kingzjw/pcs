#include "zjw_octree.h"
#include "util\zjw_math.h"
#include "util\zjw_obj.h"
#include "zjwtimer.h"
#include "zjw_dsaupd.h"

//------------- Class that holds your data.--------------------
#include <vector>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include <Eigen/Dense>
using namespace Eigen;
using namespace std;

#ifdef USE_SPARSE
	#include <eigen/Sparse>
	typedef Eigen::SparseMatrix<double> SpMat;
	typedef Eigen::Triplet<double> T;
#endif // USE_SPARSE


//------------------------------ Class that holds your data.-----------------------

//节点包含点的数据
class Node
{
public:
	//每个在空间有一堆的 particle
	vector<Vec3> pointList;

	//保存每个叶子节点下面8个象限的叶子节点
	vector<vector<Vec3*>> leafNode8Areas;

	//保存该叶子节点的在每个象限中的信号,如果没有信号，那么都是-1,-1,-1
	//vector <bool> posFlag;
	vector<Vec3> pos8Areas;
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
#ifdef USE_EIGEN
	MatrixXd  * dMatPtr;
	MatrixXd  * weightAMatPtr;
#endif // USE_EIGEN

#ifdef USE_SPARSE
	SpMat * spLap;
	std::vector<T> * coeff;
#endif // USE_SPARSE

#ifdef  USE_ARPACK
	Dsaupd * laplacianMat;
#endif //  use_arpack


public:
#ifdef USE_EIGEN
	CallTGetGraph(MatrixXd  * dMatPtr, MatrixXd  * weightAMatPtr)
	{
		this->dMatPtr = dMatPtr;
		this->weightAMatPtr = weightAMatPtr;
	}
#endif // USE_EIGEN

#ifdef USE_SPARSE
	CallTGetGraph(std::vector<T> * coefficent, SpMat * sp = nullptr)
	{
		this->coeff = coefficent;
		this->spLap = sp;
	}
#endif // USE_SPARSE

#ifdef  USE_ARPACK
	CallTGetGraph(Dsaupd *laplacianMat)
	{
		this->laplacianMat = laplacianMat;
	}
#endif //  use_arpack

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
	Vec3 minPos;
	Vec3 maxPos;
	Vec3 cellSize;

	Octree<Node> *pcsOct;
	CallTraverseGetInfoSetLeaf * ctLeaf;
	CallTGetGraph * ctGraph;

	//邻接权重矩阵，以及D矩阵，求解特征值和特征向量的存储矩阵。
	int nodeNum;
#ifdef USE_EIGEN
	MatrixXd  dMat;
	MatrixXd  weightAMat;
	MatrixXd  LaplacianMat;
	MatrixXd  eigenVecMat;
	MatrixXd  eigenValMat;
#endif // use_ei

#ifdef USE_SPARSE
	SpMat * spLaplacian;
	std::vector<T> coefficients;
#endif // USE_SPARSE

#ifdef USE_ARPACK
	Dsaupd laplacianMat;
#endif // use_arpa

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

	//初始化矩阵等信息，在getGraphMat之前调用
	void initMat();
	//得到矩阵
	void getGraphMat();

	//得到矩阵的特征向量和特征值
	void getMatEigenVerValue();

	//清空八叉树
	void clearOct();

	//判断顶点point 在包围盒中的8个象限中的哪一个
	int judege8Aeros(Vec3 &min, Vec3 &max, Vec3 &point);
	int judege8Aeros(Vec3& mid, Vec3 &point);

	//把每个叶子节点中的点划分到8个子象限中
	void setPointTo8Areas();

	//计算叶子节点在每个象限上的信号
	void getLeafSignal();

	//=============== test ====================
	void printMat();
};